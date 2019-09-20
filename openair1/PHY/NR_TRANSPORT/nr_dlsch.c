/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file PHY/NR_TRANSPORT/dlsch_decoding.c
* \brief Top-level routines for transmission of the PDSCH 38211 v 15.2.0
* \author Guy De Souza
* \date 2018
* \version 0.1
* \company Eurecom
* \email: desouza@eurecom.fr
* \note
* \warning
*/

#include "nr_dlsch.h"
#include "nr_dci.h"
#include "nr_sch_dmrs.h"
#include "PHY/MODULATION/nr_modulation.h"

//#define DEBUG_DLSCH
//#define DEBUG_DLSCH_MAPPING

void nr_pdsch_codeword_scrambling(uint8_t *in,
                         uint16_t size,
                         uint8_t q,
                         uint32_t Nid,
                         uint32_t n_RNTI,
                         uint32_t* out) {

  uint8_t reset, b_idx;
  uint32_t x1, x2, s=0;

  reset = 1;
  x2 = (n_RNTI<<15) + (q<<14) + Nid;

  for (int i=0; i<size; i++) {
    b_idx = i&0x1f;
    if (b_idx==0) {
      s = lte_gold_generic(&x1, &x2, reset);
      reset = 0;
      if (i)
        out++;
    }
    *out ^= (((in[i])&1) ^ ((s>>b_idx)&1))<<b_idx;
    //printf("i %d b_idx %d in %d s 0x%08x out 0x%08x\n", i, b_idx, in[i], s, *out);
  }

}


uint8_t nr_generate_pdsch(NR_gNB_DLSCH_t *dlsch,
                          NR_gNB_DCI_ALLOC_t *dci_alloc,
                          uint32_t ***pdsch_dmrs,
                          int32_t** txdataF,
                          int16_t amp,
                          int     frame,
                          uint8_t slot,
                          NR_DL_FRAME_PARMS *frame_parms,
                          nfapi_nr_config_request_t *config,
                          time_stats_t *dlsch_encoding_stats,
                          time_stats_t *dlsch_scrambling_stats,
                          time_stats_t *dlsch_modulation_stats) {

  NR_DL_gNB_HARQ_t *harq = dlsch->harq_processes[dci_alloc->harq_pid];
  nfapi_nr_dl_config_dlsch_pdu_rel15_t *rel15 = &harq->dlsch_pdu.dlsch_pdu_rel15;
  nfapi_nr_dl_config_pdcch_parameters_rel15_t pdcch_params = dci_alloc->pdcch_params;
  uint32_t scrambled_output[NR_MAX_NB_CODEWORDS][NR_MAX_PDSCH_ENCODED_LENGTH>>5];
  int16_t **mod_symbs = (int16_t**)dlsch->mod_symbs;
  int16_t **tx_layers = (int16_t**)dlsch->txdataF;
  int8_t Wf[2], Wt[2], l0, l_prime[2], delta;
  uint16_t nb_symbols = rel15->nb_mod_symbols;
  uint8_t Qm = rel15->modulation_order;
  uint16_t encoded_length = nb_symbols*Qm;

  /// CRC, coding, interleaving and rate matching
  AssertFatal(harq->pdu!=NULL,"harq->pdu is null\n");
  start_meas(dlsch_encoding_stats);
  nr_dlsch_encoding(harq->pdu, frame, slot, dlsch, frame_parms);
  stop_meas(dlsch_encoding_stats);
#ifdef DEBUG_DLSCH
printf("PDSCH encoding:\nPayload:\n");
for (int i=0; i<harq->B>>7; i++) {
  for (int j=0; j<16; j++)
    printf("0x%02x\t", harq->pdu[(i<<4)+j]);
  printf("\n");
}
printf("\nEncoded payload:\n");
for (int i=0; i<encoded_length>>3; i++) {
  for (int j=0; j<8; j++)
    printf("%d", harq->f[(i<<3)+j]);
  printf("\t");
}
printf("\n");
#endif

  /// scrambling
  start_meas(dlsch_scrambling_stats);
  for (int q=0; q<rel15->nb_codewords; q++)
    memset((void*)scrambled_output[q], 0, (encoded_length>>5)*sizeof(uint32_t));
  uint16_t n_RNTI = (pdcch_params.search_space_type == NFAPI_NR_SEARCH_SPACE_TYPE_UE_SPECIFIC)? \
  ((pdcch_params.scrambling_id==0)?pdcch_params.rnti:0) : 0;
  uint16_t Nid = (pdcch_params.search_space_type == NFAPI_NR_SEARCH_SPACE_TYPE_UE_SPECIFIC)? \
  pdcch_params.scrambling_id : config->sch_config.physical_cell_id.value;
  for (int q=0; q<rel15->nb_codewords; q++)
    nr_pdsch_codeword_scrambling(harq->f,
                         encoded_length,
                         q,
                         Nid,
                         n_RNTI,
                         scrambled_output[q]);
  stop_meas(dlsch_scrambling_stats);
#ifdef DEBUG_DLSCH
printf("PDSCH scrambling:\n");
for (int i=0; i<encoded_length>>8; i++) {
  for (int j=0; j<8; j++)
    printf("0x%08x\t", scrambled_output[0][(i<<3)+j]);
  printf("\n");
}
#endif
 
  /// Modulation
  start_meas(dlsch_modulation_stats);
  for (int q=0; q<rel15->nb_codewords; q++)
    nr_modulation(scrambled_output[q],
                         encoded_length,
                         Qm,
                         mod_symbs[q]);
  stop_meas(dlsch_modulation_stats);
#ifdef DEBUG_DLSCH
printf("PDSCH Modulation: Qm %d(%d)\n", Qm, nb_symbols);
for (int i=0; i<nb_symbols>>3; i++) {
  for (int j=0; j<8; j++) {
    printf("%d %d\t", mod_symbs[0][((i<<3)+j)<<1], mod_symbs[0][(((i<<3)+j)<<1)+1]);
  }
  printf("\n");
}
#endif


  /// Layer mapping
  nr_layer_mapping(mod_symbs,
                         rel15->nb_layers,
                         nb_symbols,
                         tx_layers);
#ifdef DEBUG_DLSCH
printf("Layer mapping (%d layers):\n", rel15->nb_layers);
for (int l=0; l<rel15->nb_layers; l++)
  for (int i=0; i<(nb_symbols/rel15->nb_layers)>>3; i++) {
    for (int j=0; j<8; j++) {
      printf("%d %d\t", tx_layers[l][((i<<3)+j)<<1], tx_layers[l][(((i<<3)+j)<<1)+1]);
    }
    printf("\n");
  }
#endif

  /// Antenna port mapping
    //to be moved to init phase potentially, for now tx_layers 1-8 are mapped on antenna ports 1000-1007

  /// DMRS QPSK modulation

 int nb_re_dmrs = rel15->dmrs_Type==1?6:4;
 uint16_t n_dmrs = (rel15->n_prb*nb_re_dmrs)<<1;
 int16_t mod_dmrs[n_dmrs<<1];
 uint8_t dmrs_type = rel15->dmrs_Type;
 uint8_t mapping_type = rel15->mapping_type;
 
 l0 = get_l0(mapping_type, 2);//config->pdsch_config.dmrs_typeA_position.value);
 nr_modulation(pdsch_dmrs[l0][0], n_dmrs, DMRS_MOD_ORDER, mod_dmrs); // currently only codeword 0 is modulated. Qm = 2 as DMRS is QPSK modulated

#ifdef DEBUG_DLSCH
printf("DMRS modulation (single symbol %d, %d symbols, type %d):\n", l0, n_dmrs>>1, dmrs_type);
for (int i=0; i<n_dmrs>>4; i++) {
  for (int j=0; j<8; j++) {
    printf("%d %d\t", mod_dmrs[((i<<3)+j)<<1], mod_dmrs[(((i<<3)+j)<<1)+1]);
  }
  printf("\n");
}
#endif


  /// Resource mapping

  // Non interleaved VRB to PRB mapping
  uint16_t start_sc = frame_parms->first_carrier_offset + rel15->start_prb*NR_NB_SC_PER_RB;
  if (start_sc >= frame_parms->ofdm_symbol_size)
    start_sc -= frame_parms->ofdm_symbol_size;

#ifdef DEBUG_DLSCH_MAPPING
 printf("PDSCH resource mapping started (start SC %d\tstart symbol %d\tN_PRB %d\tnb_symbols %d)\n",
	start_sc, rel15->start_symbol, rel15->n_prb, rel15->nb_symbols);
#endif

  for (int ap=0; ap<rel15->nb_layers; ap++) {

    // DMRS params for this ap
    get_Wt(Wt, ap, dmrs_type);
    get_Wf(Wf, ap, dmrs_type);
    delta = get_delta(ap, dmrs_type);
    l_prime[0] = 0; // single symbol ap 0
    uint8_t dmrs_symbol = l0+l_prime[0];
#ifdef DEBUG_DLSCH_MAPPING
printf("DMRS params for ap %d: Wt %d %d \t Wf %d %d \t delta %d \t l_prime %d \t l0 %d\tDMRS symbol %d\n",
ap, Wt[0], Wt[1], Wf[0], Wf[1], delta, l_prime[0], l0, dmrs_symbol);
#endif
    uint8_t k_prime=0;
    uint16_t m=0, n=0, dmrs_idx=0, k=0;

    for (int l=rel15->start_symbol; l<rel15->start_symbol+rel15->nb_symbols; l++) {
      k = start_sc;
      for (int i=0; i<rel15->n_prb*NR_NB_SC_PER_RB; i++) {
        if ((l == dmrs_symbol) && (k == ((start_sc+get_dmrs_freq_idx(n, k_prime, delta, dmrs_type))%(frame_parms->ofdm_symbol_size)))) {
          ((int16_t*)txdataF[ap])[(l*frame_parms->ofdm_symbol_size + k)<<1] = (Wt[l_prime[0]]*Wf[k_prime]*amp*mod_dmrs[dmrs_idx<<1]) >> 15;
          ((int16_t*)txdataF[ap])[((l*frame_parms->ofdm_symbol_size + k)<<1) + 1] = (Wt[l_prime[0]]*Wf[k_prime]*amp*mod_dmrs[(dmrs_idx<<1) + 1]) >> 15;
#ifdef DEBUG_DLSCH_MAPPING
printf("dmrs_idx %d\t l %d \t k %d \t k_prime %d \t n %d \t txdataF: %d %d\n",
dmrs_idx, l, k, k_prime, n, ((int16_t*)txdataF[ap])[(l*frame_parms->ofdm_symbol_size + k)<<1],
((int16_t*)txdataF[ap])[((l*frame_parms->ofdm_symbol_size + k)<<1) + 1]);
#endif
          dmrs_idx++;
          k_prime++;
          k_prime&=1;
          n+=(k_prime)?0:1;
        }

        else {

          ((int16_t*)txdataF[ap])[(l*frame_parms->ofdm_symbol_size + k)<<1] = (amp * tx_layers[ap][m<<1]) >> 15;
          ((int16_t*)txdataF[ap])[((l*frame_parms->ofdm_symbol_size + k)<<1) + 1] = (amp * tx_layers[ap][(m<<1) + 1]) >> 15;
#ifdef DEBUG_DLSCH_MAPPING
printf("m %d\t l %d \t k %d \t txdataF: %d %d\n",
m, l, k, ((int16_t*)txdataF[ap])[(l*frame_parms->ofdm_symbol_size + k)<<1],
((int16_t*)txdataF[ap])[((l*frame_parms->ofdm_symbol_size + k)<<1) + 1]);
#endif
          m++;
        }
        if (++k >= frame_parms->ofdm_symbol_size)
          k -= frame_parms->ofdm_symbol_size;
      }
    }
  }
  return 0;
}
