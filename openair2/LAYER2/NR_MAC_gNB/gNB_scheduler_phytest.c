/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
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

/*! \file gNB_scheduler_phytest.c
 * \brief gNB scheduling procedures in phy_test mode
 * \author  Guy De Souza
 * \date 07/2018
 * \email: desouza@eurecom.fr
 * \version 1.0
 * @ingroup _mac
 */

#include "nr_mac_gNB.h"
#include "SCHED_NR/sched_nr.h"
#include "mac_proto.h"
#include "PHY/NR_TRANSPORT/nr_dlsch.h"
#include "PHY/NR_TRANSPORT/nr_dci.h"
extern RAN_CONTEXT_t RC;

/*Scheduling of DLSCH with associated DCI in common search space
 * current version has only a DCI for type 1 PDCCH for C_RNTI*/
void nr_schedule_css_dlsch_phytest(module_id_t   module_idP,
                                   frame_t       frameP,
                                   sub_frame_t   slotP)
{
  uint8_t  CC_id;

  gNB_MAC_INST                        *nr_mac      = RC.nrmac[module_idP];
  NR_COMMON_channels_t                *cc          = nr_mac->common_channels;
  nfapi_nr_dl_config_request_body_t   *dl_req;
  nfapi_nr_dl_config_request_pdu_t  *dl_config_dci_pdu;
  nfapi_nr_dl_config_request_pdu_t  *dl_config_dlsch_pdu;
  nfapi_tx_request_pdu_t            *TX_req;

  nfapi_nr_config_request_t *cfg = &nr_mac->config[0];
  uint16_t rnti = 0x1234;

  uint16_t sfn_sf = frameP << 7 | slotP;
  int dlBWP_carrier_bandwidth = cfg->initialBWP_config.dl_bandwidth.value;

  // everything here is hard-coded to 30 kHz
  int scs               = get_dlscs(cfg);
  int slots_per_frame   = get_spf(cfg);

  for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    LOG_D(MAC, "Scheduling common search space DCI type 1 dlBWP BW.firstRB %d.%d\n",cfg->initialBWP_config.dl_bandwidth.value,cfg->initialBWP_config.dl_offset.value);


    dl_req = &nr_mac->DL_req[CC_id].dl_config_request_body;
    dl_config_dci_pdu = &dl_req->dl_config_pdu_list[dl_req->number_pdu];
    memset((void*)dl_config_dci_pdu,0,sizeof(nfapi_nr_dl_config_request_pdu_t));
    dl_config_dci_pdu->pdu_type = NFAPI_NR_DL_CONFIG_DCI_DL_PDU_TYPE;
    dl_config_dci_pdu->pdu_size = (uint8_t)(2+sizeof(nfapi_nr_dl_config_dci_dl_pdu));

    dl_config_dlsch_pdu = &dl_req->dl_config_pdu_list[dl_req->number_pdu+1];
    memset((void*)dl_config_dlsch_pdu,0,sizeof(nfapi_nr_dl_config_request_pdu_t));
    dl_config_dlsch_pdu->pdu_type = NFAPI_NR_DL_CONFIG_DLSCH_PDU_TYPE;
    dl_config_dlsch_pdu->pdu_size = (uint8_t)(2+sizeof(nfapi_nr_dl_config_dlsch_pdu));

    nfapi_nr_dl_config_dci_dl_pdu_rel15_t *pdu_rel15 = &dl_config_dci_pdu->dci_dl_pdu.dci_dl_pdu_rel15;
    nfapi_nr_dl_config_pdcch_parameters_rel15_t *params_rel15 = &dl_config_dci_pdu->dci_dl_pdu.pdcch_params_rel15;
    nfapi_nr_dl_config_dlsch_pdu_rel15_t *dlsch_pdu_rel15 = &dl_config_dlsch_pdu->dlsch_pdu.dlsch_pdu_rel15;

    dlsch_pdu_rel15->start_prb = 0;
    dlsch_pdu_rel15->n_prb = 6;

    dlsch_pdu_rel15->rnti = rnti;
    dlsch_pdu_rel15->nb_layers =1;
    dlsch_pdu_rel15->nb_codewords = 1;
    dlsch_pdu_rel15->mcs_idx = 9;
    dlsch_pdu_rel15->ndi = 1;
    dlsch_pdu_rel15->redundancy_version = 0;

    dlsch_pdu_rel15->dmrs_TypeA_Position = cfg->pdsch_config.dmrs_TypeA_Position.value;

    nr_configure_css_dci_initial(params_rel15,
				 scs, 
				 scs, 
				 nr_FR1, 
				 0, 
				 0, 
				 0,
				 sfn_sf, slotP,
				 slots_per_frame,
				 dlBWP_carrier_bandwidth);

    params_rel15->first_slot = 0;

    pdu_rel15->frequency_domain_assignment = get_RIV(dlsch_pdu_rel15->start_prb, dlsch_pdu_rel15->n_prb, cfg->rf_config.dl_carrier_bandwidth.value);
    pdu_rel15->time_domain_assignment = 0;

        
    dlsch_pdu_rel15->start_symbol = extract_startSymbol(cfg->pdsch_config.PDSCHTimeDomainResourceAllocation_startSymbolAndLength[pdu_rel15->time_domain_assignment].value);
    dlsch_pdu_rel15->nb_symbols   = extract_length(cfg->pdsch_config.PDSCHTimeDomainResourceAllocation_startSymbolAndLength[pdu_rel15->time_domain_assignment].value);
    pdu_rel15->vrb_to_prb_mapping = 1;
    pdu_rel15->mcs = 9;
    pdu_rel15->tb_scaling = 1;
    
    pdu_rel15->ra_preamble_index = 25;
    pdu_rel15->format_indicator = 1;
    pdu_rel15->ndi = 1;
    pdu_rel15->rv = 0;
    pdu_rel15->harq_pid = 0;
    pdu_rel15->dai = 2;
    pdu_rel15->tpc = 2;
    pdu_rel15->pucch_resource_indicator = 7;
    pdu_rel15->pdsch_to_harq_feedback_timing_indicator = 7;

    LOG_D(MAC, "[gNB scheduler phytest] DCI type 1 payload: freq_alloc %d, time_alloc %d, vrb to prb %d, mcs %d tb_scaling %d ndi %d rv %d\n",
                pdu_rel15->frequency_domain_assignment,
                pdu_rel15->time_domain_assignment,
                pdu_rel15->vrb_to_prb_mapping,
                pdu_rel15->mcs,
                pdu_rel15->tb_scaling,
                pdu_rel15->ndi,
                pdu_rel15->rv);

    params_rel15->rnti = rnti;
    params_rel15->rnti_type = NFAPI_NR_RNTI_C;
    params_rel15->dci_format = NFAPI_NR_DL_DCI_FORMAT_1_0;
    //params_rel15->aggregation_level = 1;
    LOG_D(MAC, "DCI type 1 params: rnti %x, rnti_type %d, dci_format %d\n \
                coreset params: mux_pattern %d, n_rb %d, n_symb %d, rb_offset %d  \n \
                ss params : nb_ss_sets_per_slot %d, first symb %d, nb_slots %d, sfn_mod2 %d, first slot %d\n",
                params_rel15->rnti,
                params_rel15->rnti_type,
                params_rel15->dci_format,
                params_rel15->mux_pattern,
                params_rel15->n_rb,
                params_rel15->n_symb,
                params_rel15->rb_offset,
                params_rel15->nb_ss_sets_per_slot,
                params_rel15->first_symbol,
                params_rel15->nb_slots,
                params_rel15->sfn_mod2,
                params_rel15->first_slot);
  nr_get_tbs(&dl_config_dlsch_pdu->dlsch_pdu, dl_config_dci_pdu->dci_dl_pdu);
  LOG_D(MAC, "DLSCH PDU: start PRB %d n_PRB %d start symbol %d nb_symbols %d nb_layers %d nb_codewords %d mcs %d\n",
  dlsch_pdu_rel15->start_prb,
  dlsch_pdu_rel15->n_prb,
  dlsch_pdu_rel15->start_symbol,
  dlsch_pdu_rel15->nb_symbols,
  dlsch_pdu_rel15->nb_layers,
  dlsch_pdu_rel15->nb_codewords,
  dlsch_pdu_rel15->mcs_idx);

  dl_req->number_dci++;
  dl_req->number_pdsch_rnti++;
  dl_req->number_pdu+=2;

  TX_req = &nr_mac->TX_req[CC_id].tx_request_body.tx_pdu_list[nr_mac->TX_req[CC_id].tx_request_body.number_of_pdus];
  TX_req->pdu_length = 6;
  TX_req->pdu_index = nr_mac->pdu_index[CC_id]++;
  TX_req->num_segments = 1;
  TX_req->segments[0].segment_length = 8;
  TX_req->segments[0].segment_data   = &cc[CC_id].RAR_pdu.payload[0];
  nr_mac->TX_req[CC_id].tx_request_body.number_of_pdus++;
  nr_mac->TX_req[CC_id].sfn_sf = sfn_sf;
  nr_mac->TX_req[CC_id].tx_request_body.tl.tag = NFAPI_TX_REQUEST_BODY_TAG;
  nr_mac->TX_req[CC_id].header.message_id = NFAPI_TX_REQUEST;

    
  }
}

/*Scheduling of DLSCH with associated DCI in user specific search space
 * current version has only a DCI for type 1 PDCCH for C_RNTI*/
void nr_schedule_uss_dlsch_phytest(module_id_t   module_idP,
                                   frame_t       frameP,
                                   sub_frame_t   slotP)
{
  uint8_t  CC_id;

  gNB_MAC_INST                        *nr_mac      = RC.nrmac[module_idP];
  //NR_COMMON_channels_t                *cc           = nr_mac->common_channels;
  nfapi_nr_dl_config_request_body_t   *dl_req;
  nfapi_nr_dl_config_request_pdu_t  *dl_config_dci_pdu;
  nfapi_nr_dl_config_request_pdu_t  *dl_config_dlsch_pdu;
  nfapi_tx_request_pdu_t            *TX_req;

  nfapi_nr_config_request_t *cfg = &nr_mac->config[0];
  uint16_t rnti = 0x1234;

  uint16_t sfn_sf = frameP << 7 | slotP;
  int dl_carrier_bandwidth = cfg->rf_config.dl_carrier_bandwidth.value;

  // everything here is hard-coded to 30 kHz
  //int scs = get_dlscs(cfg);
  //int slots_per_frame = get_spf(cfg);
  for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    LOG_D(MAC, "Scheduling UE specific search space DCI type 1 for CC_id %d\n",CC_id);

    nfapi_nr_coreset_t* coreset = &nr_mac->coreset[CC_id][1];
    nfapi_nr_search_space_t* search_space = &nr_mac->search_space[CC_id][1];

    dl_req = &nr_mac->DL_req[CC_id].dl_config_request_body;
    dl_config_dci_pdu = &dl_req->dl_config_pdu_list[dl_req->number_pdu];
    memset((void*)dl_config_dci_pdu,0,sizeof(nfapi_nr_dl_config_request_pdu_t));
    dl_config_dci_pdu->pdu_type = NFAPI_NR_DL_CONFIG_DCI_DL_PDU_TYPE;
    dl_config_dci_pdu->pdu_size = (uint8_t)(2+sizeof(nfapi_nr_dl_config_dci_dl_pdu));

    dl_config_dlsch_pdu = &dl_req->dl_config_pdu_list[dl_req->number_pdu+1];
    memset((void*)dl_config_dlsch_pdu,0,sizeof(nfapi_nr_dl_config_request_pdu_t));
    dl_config_dlsch_pdu->pdu_type = NFAPI_NR_DL_CONFIG_DLSCH_PDU_TYPE;
    dl_config_dlsch_pdu->pdu_size = (uint8_t)(2+sizeof(nfapi_nr_dl_config_dlsch_pdu));

    nfapi_nr_dl_config_dci_dl_pdu_rel15_t *pdu_rel15 = &dl_config_dci_pdu->dci_dl_pdu.dci_dl_pdu_rel15;
    nfapi_nr_dl_config_pdcch_parameters_rel15_t *params_rel15 = &dl_config_dci_pdu->dci_dl_pdu.pdcch_params_rel15;
    nfapi_nr_dl_config_dlsch_pdu_rel15_t *dlsch_pdu_rel15 = &dl_config_dlsch_pdu->dlsch_pdu.dlsch_pdu_rel15;

    dlsch_pdu_rel15->start_prb = 0;
    dlsch_pdu_rel15->n_prb = 50;
    dlsch_pdu_rel15->start_symbol = 2;
    dlsch_pdu_rel15->nb_symbols = 9;
    dlsch_pdu_rel15->rnti = rnti;
    dlsch_pdu_rel15->nb_layers =1;
    dlsch_pdu_rel15->nb_codewords = 1;
    dlsch_pdu_rel15->mcs_idx = 9;
    dlsch_pdu_rel15->ndi = 1;
    dlsch_pdu_rel15->redundancy_version = 0;

    nr_configure_dci_from_pdcch_config(params_rel15,
                                       coreset,
                                       search_space,
                                       *cfg,
                                       dl_carrier_bandwidth);

    pdu_rel15->frequency_domain_assignment = get_RIV(dlsch_pdu_rel15->start_prb, dlsch_pdu_rel15->n_prb, cfg->rf_config.dl_carrier_bandwidth.value);
    pdu_rel15->time_domain_assignment = 3; // row index used here instead of SLIV;
    pdu_rel15->vrb_to_prb_mapping = 1;
    pdu_rel15->mcs = 9;
    pdu_rel15->tb_scaling = 1;

    pdu_rel15->ra_preamble_index = 25;
    pdu_rel15->format_indicator = 1;
    pdu_rel15->ndi = 1;
    pdu_rel15->rv = 0;
    pdu_rel15->harq_pid = 0;
    pdu_rel15->dai = 2;
    pdu_rel15->tpc = 2;
    pdu_rel15->pucch_resource_indicator = 7;
    pdu_rel15->pdsch_to_harq_feedback_timing_indicator = 7;

    LOG_D(MAC, "[gNB scheduler phytest] DCI type 1 payload: freq_alloc %d, time_alloc %d, vrb to prb %d, mcs %d tb_scaling %d ndi %d rv %d\n",
                pdu_rel15->frequency_domain_assignment,
                pdu_rel15->time_domain_assignment,
                pdu_rel15->vrb_to_prb_mapping,
                pdu_rel15->mcs,
                pdu_rel15->tb_scaling,
                pdu_rel15->ndi,
                pdu_rel15->rv);

    params_rel15->rnti = rnti;
    params_rel15->rnti_type = NFAPI_NR_RNTI_C;
    params_rel15->dci_format = NFAPI_NR_DL_DCI_FORMAT_1_0;

    //params_rel15->aggregation_level = 1;
    LOG_D(MAC, "DCI params: rnti %d, rnti_type %d, dci_format %d, config type %d\n \
                coreset params: mux_pattern %d, n_rb %d, n_symb %d, rb_offset %d  \n \
                ss params : first symb %d, ss type %d\n",
                params_rel15->rnti,
                params_rel15->rnti_type,
                params_rel15->config_type,
                params_rel15->dci_format,
                params_rel15->mux_pattern,
                params_rel15->n_rb,
                params_rel15->n_symb,
                params_rel15->rb_offset,
                params_rel15->first_symbol,
                params_rel15->search_space_type);
  nr_get_tbs(&dl_config_dlsch_pdu->dlsch_pdu, dl_config_dci_pdu->dci_dl_pdu);
  LOG_D(MAC, "DLSCH PDU: start PRB %d n_PRB %d start symbol %d nb_symbols %d nb_layers %d nb_codewords %d mcs %d\n",
  dlsch_pdu_rel15->start_prb,
  dlsch_pdu_rel15->n_prb,
  dlsch_pdu_rel15->start_symbol,
  dlsch_pdu_rel15->nb_symbols,
  dlsch_pdu_rel15->nb_layers,
  dlsch_pdu_rel15->nb_codewords,
  dlsch_pdu_rel15->mcs_idx);

  dl_req->number_dci++;
  dl_req->number_pdsch_rnti++;
  dl_req->number_pdu+=2;

  TX_req = &nr_mac->TX_req[CC_id].tx_request_body.tx_pdu_list[nr_mac->TX_req[CC_id].tx_request_body.number_of_pdus];
  TX_req->pdu_length = dlsch_pdu_rel15->transport_block_size;
  TX_req->pdu_index = nr_mac->pdu_index[CC_id]++;
  TX_req->num_segments = 1;

  // HOT FIX for all zero pdu problem
  // ------------------------------------------------------------------------------------------------
  for(int i = 0; i < dlsch_pdu_rel15->transport_block_size/8; i++) {
    ((uint8_t *)nr_mac->UE_list.DLSCH_pdu[CC_id][0][0].payload[0])[i] = (unsigned char) rand();
  }
  // ------------------------------------------------------------------------------------------------

  TX_req->segments[0].segment_data   = nr_mac->UE_list.DLSCH_pdu[CC_id][0][0].payload[0];
  TX_req->segments[0].segment_length = dlsch_pdu_rel15->transport_block_size+2;
  nr_mac->TX_req[CC_id].tx_request_body.number_of_pdus++;
  nr_mac->TX_req[CC_id].sfn_sf = sfn_sf;
  nr_mac->TX_req[CC_id].tx_request_body.tl.tag = NFAPI_TX_REQUEST_BODY_TAG;
  nr_mac->TX_req[CC_id].header.message_id = NFAPI_TX_REQUEST;

  }
}
