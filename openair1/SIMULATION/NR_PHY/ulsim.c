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

#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "common/ran_context.h"
#include "common/config/config_userapi.h"
#include "common/utils/LOG/log.h"
#include "PHY/defs_gNB.h"
#include "PHY/defs_nr_common.h"
#include "PHY/defs_nr_UE.h"
#include "PHY/phy_vars_nr_ue.h"
#include "PHY/types.h"
#include "PHY/INIT/phy_init.h"
#include "PHY/MODULATION/modulation_UE.h"
#include "PHY/MODULATION/nr_modulation.h"
#include "PHY/NR_REFSIG/dmrs_nr.h"
#include "PHY/NR_REFSIG/nr_mod_table.h"
#include "PHY/NR_REFSIG/refsig_defs_ue.h"
#include "PHY/NR_TRANSPORT/nr_dlsch.h"
#include "PHY/NR_TRANSPORT/nr_sch_dmrs.h"
#include "PHY/NR_TRANSPORT/nr_transport.h"
#include "PHY/NR_TRANSPORT/nr_transport_proto.h"
#include "PHY/NR_TRANSPORT/nr_ulsch.h"
#include "PHY/NR_UE_TRANSPORT/nr_transport_proto_ue.h"
#include "PHY/TOOLS/tools_defs.h"
#include "SCHED_NR/sched_nr.h"
#include "SCHED_NR_UE/defs.h"
#include "SCHED_NR_UE/fapi_nr_ue_l1.h"
#include "openair1/SIMULATION/TOOLS/sim.h"
#include "openair1/SIMULATION/RF/rf.h"
#include "openair1/SIMULATION/NR_PHY/nr_unitary_defs.h"
#include "openair1/SIMULATION/NR_PHY/nr_dummy_functions.c"
#include "openair2/LAYER2/NR_MAC_UE/mac_proto.h"

//#define DEBUG_ULSIM

PHY_VARS_gNB *gNB;
PHY_VARS_NR_UE *UE;
RAN_CONTEXT_t RC;
double cpuf;
int nfapi_mode = 0;

int oai_nfapi_hi_dci0_req(nfapi_hi_dci0_request_t *hi_dci0_req) {
  return (0);
}
int oai_nfapi_tx_req(nfapi_tx_request_t *tx_req) {
  return (0);
}

int oai_nfapi_dl_config_req(nfapi_dl_config_request_t *dl_config_req) {
  return (0);
}

int oai_nfapi_ul_config_req(nfapi_ul_config_request_t *ul_config_req) {
  return (0);
}

int oai_nfapi_nr_dl_config_req(nfapi_nr_dl_config_request_t *dl_config_req) {
  return (0);
}

int32_t get_uldl_offset(int eutra_bandP) {
  return (0);
}

NR_IF_Module_t *
NR_IF_Module_init(int Mod_id) {
  return (NULL);
}

void exit_function(const char *file, const char *function, const int line, const char *s) {
  const char *msg = s == NULL ? "no comment" : s;
  printf("Exiting at: %s:%d %s(), %s\n", file, line, function, msg);
  exit(-1);
}

// dummy functions
int8_t nr_mac_rrc_data_ind_ue(const module_id_t module_id, const int CC_id, const uint8_t gNB_index,
                              const int8_t channel, const uint8_t* pduP, const sdu_size_t pdu_len) { return 0; }

// needed for some functions
uint16_t n_rnti = 0x1234;
openair0_config_t openair0_cfg[MAX_CARDS];

int main(int argc, char **argv)
{
  char c;
  int i,sf;
  double SNR, snr0 = -2.0, snr1 = 2.0;
  double sigma, sigma_dB;
  double snr_step = 0.1;
  uint8_t snr1set = 0;
  int slot = 0;
  FILE *output_fd = NULL;
  //uint8_t write_output_file = 0;
  int trial, n_trials = 1, n_errors = 0, n_false_positive = 0;
  uint8_t n_tx = 1, n_rx = 1;
  //uint8_t transmission_mode = 1;
  uint16_t Nid_cell = 0;
  channel_desc_t *gNB2UE;
  uint8_t extended_prefix_flag = 0;
  //int8_t interf1 = -21, interf2 = -21;
  FILE *input_fd = NULL;
  SCM_t channel_model = AWGN;  //Rayleigh1_anticorr;
  uint16_t N_RB_DL = 106, N_RB_UL = 106, mu = 1;
  //unsigned char frame_type = 0;
  int frame = 0;
  int frame_length_complex_samples;
  NR_DL_FRAME_PARMS *frame_parms;
  int loglvl = OAILOG_WARNING;
  uint64_t SSB_positions=0x01;
  uint16_t nb_symb_sch = 12;
  int start_symbol = NR_SYMBOLS_PER_SLOT - nb_symb_sch;
  uint16_t nb_rb = 50;
  uint8_t Imcs = 9;
  uint8_t precod_nbr_layers = 1;
  int gNB_id = 0;
  int ap;
  int tx_offset;
  double txlev_float;
  int32_t txlev;
  int start_rb = 30;
  int UE_id =0; // [hna] only works for UE_id = 0 because NUMBER_OF_NR_UE_MAX is set to 1 (phy_init_nr_gNB causes segmentation fault)


  cpuf = get_cpu_freq_GHz();


  UE_nr_rxtx_proc_t UE_proc;


  if ( load_configmodule(argc,argv,CONFIG_ENABLECMDLINEONLY) == 0 ) {
    exit_fun("[NR_ULSIM] Error, configuration module init failed\n");
  }

  //logInit();
  randominit(0);

  while ((c = getopt(argc, argv, "df:hpg:i:j:n:l:m:r:s:S:y:z:M:N:F:R:P:")) != -1) {
    switch (c) {
      /*case 'f':
         write_output_file = 1;
         output_fd = fopen(optarg, "w");

         if (output_fd == NULL) {
             printf("Error opening %s\n", optarg);
             exit(-1);
         }

         break;*/

      /*case 'd':
        frame_type = 1;
        break;*/

      case 'g':
        switch ((char) *optarg) {
          case 'A':
            channel_model = SCM_A;
            break;

          case 'B':
            channel_model = SCM_B;
            break;

          case 'C':
            channel_model = SCM_C;
            break;

          case 'D':
            channel_model = SCM_D;
            break;

          case 'E':
            channel_model = EPA;
            break;

          case 'F':
            channel_model = EVA;
            break;

          case 'G':
            channel_model = ETU;
            break;

          default:
            printf("Unsupported channel model!\n");
            exit(-1);
        }

        break;

      /*case 'i':
        interf1 = atoi(optarg);
        break;

      case 'j':
        interf2 = atoi(optarg);
        break;*/

      case 'n':
        n_trials = atoi(optarg);
        break;

      case 's':
        snr0 = atof(optarg);
        printf("Setting SNR0 to %f\n", snr0);
        break;

      case 'S':
        snr1 = atof(optarg);
        snr1set = 1;
        printf("Setting SNR1 to %f\n", snr1);
        break;

      case 'p':
        extended_prefix_flag = 1;
        break;

      /*
       case 'r':
       ricean_factor = pow(10,-.1*atof(optarg));
       if (ricean_factor>1) {
       printf("Ricean factor must be between 0 and 1\n");
       exit(-1);
       }
       break;
       */

      case 'y':
        n_tx = atoi(optarg);

        if ((n_tx == 0) || (n_tx > 2)) {
          printf("Unsupported number of tx antennas %d\n", n_tx);
          exit(-1);
        }

        break;

      case 'z':
        n_rx = atoi(optarg);

        if ((n_rx == 0) || (n_rx > 2)) {
          printf("Unsupported number of rx antennas %d\n", n_rx);
          exit(-1);
        }

        break;

      case 'M':
        SSB_positions = atoi(optarg);
        break;

      case 'N':
        Nid_cell = atoi(optarg);
        break;

      case 'R':
        N_RB_DL = atoi(optarg);
        N_RB_UL = N_RB_DL;
        break;

      case 'F':
        input_fd = fopen(optarg, "r");

        if (input_fd == NULL) {
            printf("Problem with filename %s\n", optarg);
            exit(-1);
        }

        break;

      case 'm':
        Imcs = atoi(optarg);
        break;

      case 'l':
        nb_symb_sch = atoi(optarg);
        break;

      case 'r':
        nb_rb = atoi(optarg);
        break;

      /*case 'x':
        transmission_mode = atoi(optarg);
        break;*/

      default:
        case 'h':
          printf("%s -h(elp) -p(extended_prefix) -N cell_id -f output_filename -F input_filename -g channel_model -n n_frames -t Delayspread -s snr0 -S snr1 -x transmission_mode -y TXant -z RXant -i Intefrence0 -j Interference1 -A interpolation_file -C(alibration offset dB) -N CellId\n", argv[0]);
          printf("-h This message\n");
          printf("-p Use extended prefix mode\n");
          //printf("-d Use TDD\n");
          printf("-n Number of frames to simulate\n");
          printf("-s Starting SNR, runs from SNR0 to SNR0 + 5 dB.  If n_frames is 1 then just SNR is simulated\n");
          printf("-S Ending SNR, runs from SNR0 to SNR1\n");
          printf("-t Delay spread for multipath channel\n");
          printf("-g [A,B,C,D,E,F,G] Use 3GPP SCM (A,B,C,D) or 36-101 (E-EPA,F-EVA,G-ETU) models (ignores delay spread and Ricean factor)\n");
          //printf("-x Transmission mode (1,2,6 for the moment)\n");
          printf("-y Number of TX antennas used in eNB\n");
          printf("-z Number of RX antennas used in UE\n");
          //printf("-i Relative strength of first intefering eNB (in dB) - cell_id mod 3 = 1\n");
          //printf("-j Relative strength of second intefering eNB (in dB) - cell_id mod 3 = 2\n");
          printf("-M Multiple SSB positions in burst\n");
          printf("-N Nid_cell\n");
          printf("-R N_RB_DL\n");
          printf("-O oversampling factor (1,2,4,8,16)\n");
          printf("-A Interpolation_filname Run with Abstraction to generate Scatter plot using interpolation polynomial in file\n");
          //printf("-C Generate Calibration information for Abstraction (effective SNR adjustment to remove Pe bias w.r.t. AWGN)\n");
          //printf("-f Output filename (.txt format) for Pe/SNR results\n");
          printf("-F Input filename (.txt format) for RX conformance testing\n");
          exit(-1);
          break;
    }
  }

  logInit();
  set_glog(loglvl);
  T_stdout = 1;

  if (snr1set == 0)
    snr1 = snr0 + 10;

  gNB2UE = new_channel_desc_scm(n_tx, n_rx, channel_model,
                                61.44e6, //N_RB2sampling_rate(N_RB_DL),
                                40e6, //N_RB2channel_bandwidth(N_RB_DL),
                                0, 0, 0);

  if (gNB2UE == NULL) {
    printf("Problem generating channel model. Exiting.\n");
    exit(-1);
  }

  RC.gNB = (PHY_VARS_gNB **) malloc(sizeof(PHY_VARS_gNB *));
  RC.gNB[0] = malloc(sizeof(PHY_VARS_gNB));
  gNB = RC.gNB[0];
  //gNB_config = &gNB->gNB_config;
  gNB_L1_rxtx_proc_t gNB_proc;

  gNB_proc.frame_rx = frame;
  gNB_proc.slot_rx  = slot;

  UE_proc.nr_tti_tx = slot;
  UE_proc.frame_tx = frame;

  frame_parms = &gNB->frame_parms; //to be initialized I suppose (maybe not necessary for PBCH)
  frame_parms->nb_antennas_tx = n_tx;
  frame_parms->nb_antennas_rx = n_rx;
  frame_parms->N_RB_DL = N_RB_DL;
  frame_parms->N_RB_UL = N_RB_UL;
  frame_parms->Ncp = extended_prefix_flag ? EXTENDED : NORMAL;

  crcTableInit();

  nr_phy_config_request_sim(gNB, N_RB_DL, N_RB_DL, mu, Nid_cell, SSB_positions);

  phy_init_nr_gNB(gNB, 0, 0);
  //init_eNB_afterRU();

  frame_length_complex_samples = frame_parms->samples_per_subframe;
  //frame_length_complex_samples_no_prefix = frame_parms->samples_per_subframe_wCP;

  //configure UE
  UE = malloc(sizeof(PHY_VARS_NR_UE));
  memset((void*)UE,0,sizeof(PHY_VARS_NR_UE));
  PHY_vars_UE_g = malloc(sizeof(PHY_VARS_NR_UE**));
  PHY_vars_UE_g[0] = malloc(sizeof(PHY_VARS_NR_UE*));
  PHY_vars_UE_g[0][0] = UE;
  memcpy(&UE->frame_parms, frame_parms, sizeof(NR_DL_FRAME_PARMS));

  //phy_init_nr_top(frame_parms);
  if (init_nr_ue_signal(UE, 1, 0) != 0) {
    printf("Error at UE NR initialisation\n");
    exit(-1);
  }

  //nr_init_frame_parms_ue(&UE->frame_parms);
  //init_nr_ue_transport(UE, 0);
  for (sf = 0; sf < 2; sf++) {
    for (i = 0; i < 2; i++) {

        UE->ulsch[sf][0][i] = new_nr_ue_ulsch(N_RB_UL, 8, 0);

        if (!UE->ulsch[sf][0][i]) {
          printf("Can't get ue ulsch structures\n");
          exit(-1);
        }

    }
  }


  unsigned char harq_pid = 0;
  unsigned int TBS = 8424;
  unsigned int available_bits;
  uint8_t nb_re_dmrs = UE->dmrs_UplinkConfig.pusch_maxLength*(UE->dmrs_UplinkConfig.pusch_dmrs_type == pusch_dmrs_type1)?6:4;
  uint8_t length_dmrs = 1;
  unsigned char mod_order;

  mod_order      = nr_get_Qm(Imcs, 1);
  available_bits = nr_get_G(nb_rb, nb_symb_sch, nb_re_dmrs, length_dmrs, mod_order, 1);
  TBS            = nr_compute_tbs(Imcs, nb_rb, nb_symb_sch, nb_re_dmrs, length_dmrs, precod_nbr_layers);

  NR_gNB_ULSCH_t *ulsch_gNB = gNB->ulsch[UE_id+1][0];
  nfapi_nr_ul_config_ulsch_pdu *rel15_ul = &ulsch_gNB->harq_processes[harq_pid]->ulsch_pdu;
  
  NR_UE_ULSCH_t **ulsch_ue = UE->ulsch[0][0];

  // --------- setting rel15_ul parameters for gNB --------
  rel15_ul->rnti                           = n_rnti;
  rel15_ul->ulsch_pdu_rel15.start_rb       = start_rb;
  rel15_ul->ulsch_pdu_rel15.number_rbs     = nb_rb;
  rel15_ul->ulsch_pdu_rel15.start_symbol   = start_symbol;
  rel15_ul->ulsch_pdu_rel15.number_symbols = nb_symb_sch;
  rel15_ul->ulsch_pdu_rel15.nb_re_dmrs     = nb_re_dmrs;
  rel15_ul->ulsch_pdu_rel15.length_dmrs    = length_dmrs;
  rel15_ul->ulsch_pdu_rel15.Qm             = mod_order;
  rel15_ul->ulsch_pdu_rel15.mcs            = Imcs;
  rel15_ul->ulsch_pdu_rel15.rv             = 0;
  rel15_ul->ulsch_pdu_rel15.ndi            = 0;
  rel15_ul->ulsch_pdu_rel15.n_layers       = precod_nbr_layers;
  ///////////////////////////////////////////////////

  nr_scheduled_response_t scheduled_response;
  fapi_nr_ul_config_request_t ul_config;
  //fapi_nr_tx_request_t tx_request;

  scheduled_response.module_id = 0;
  scheduled_response.CC_id = 0;
  scheduled_response.frame = frame;
  scheduled_response.slot = slot;
  scheduled_response.dl_config = NULL;
  scheduled_response.ul_config = &ul_config;
  scheduled_response.dl_config = NULL;
  

  ul_config.sfn_slot = slot;
  ul_config.number_pdus = 1;
  ul_config.ul_config_list[0].pdu_type = FAPI_NR_UL_CONFIG_TYPE_ULSCH;
  ul_config.ul_config_list[0].ulsch_config_pdu.rnti = n_rnti;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.number_rbs = nb_rb;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.start_rb = start_rb;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.number_symbols = nb_symb_sch;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.start_symbol = start_symbol;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.mcs = Imcs;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.ndi = 0;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.rv = 0;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.n_layers = precod_nbr_layers;
  ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.harq_process_nbr = harq_pid;
  //there are plenty of other parameters that we don't seem to be using for now. e.g.
  //ul_config.ul_config_list[0].ulsch_config_pdu.ulsch_pdu_rel15.absolute_delta_PUSCH = 0; 

  // set FAPI parameters for UE, put them in the scheduled response and call 
  nr_ue_scheduled_response(&scheduled_response);

  unsigned char *estimated_output_bit;
  unsigned char *test_input_bit;
  unsigned int errors_bit = 0;
  uint32_t errors_scrambling = 0;

  test_input_bit       = (unsigned char *) malloc16(sizeof(unsigned char) * 16 * 68 * 384);
  estimated_output_bit = (unsigned char *) malloc16(sizeof(unsigned char) * 16 * 68 * 384);

  /////////////////////////phy_procedures_nr_ue_TX///////////////////////
  ///////////

  phy_procedures_nrUE_TX(UE, &UE_proc, gNB_id, 0);

  ///////////
  ////////////////////////////////////////////////////
  tx_offset = slot*frame_parms->samples_per_slot;

  txlev = signal_energy_amp_shift(&UE->common_vars.txdata[0][tx_offset + 5*frame_parms->ofdm_symbol_size + 4*frame_parms->nb_prefix_samples + frame_parms->nb_prefix_samples0],
          frame_parms->ofdm_symbol_size + frame_parms->nb_prefix_samples);

  txlev_float = (double)txlev/(double)AMP; // output of signal_energy is fixed point representation


  
  printf("\n");

  for (SNR = snr0; SNR < snr1; SNR += snr_step) {

    n_errors = 0;
    n_false_positive = 0;

    //AWGN
    sigma_dB = 10*log10(txlev_float)-SNR;
    sigma    = pow(10,sigma_dB/10);

    for (trial = 0; trial < n_trials; trial++) {

      errors_scrambling  = 0;
      errors_bit         = 0;

      for (i=0; i<frame_length_complex_samples; i++) {
        for (ap=0; ap<frame_parms->nb_antennas_rx; ap++) {
          // [hna] doesn't work with noise in case of mod_order = 6
          ((short*) gNB->common_vars.rxdata[ap])[2*i]   = (((int16_t *)UE->common_vars.txdata[ap])[(i<<1)])+ (int16_t)(sqrt(sigma/2)*gaussdouble(0.0,1.0)*(double)AMP); // convert to fixed point
          ((short*) gNB->common_vars.rxdata[ap])[2*i+1] = (((int16_t *)UE->common_vars.txdata[ap])[(i<<1)+1])+ (int16_t)(sqrt(sigma/2)*gaussdouble(0.0,1.0)*(double)AMP);
        }
      }

      
      phy_procedures_gNB_common_RX(gNB, &gNB_proc);

      phy_procedures_gNB_uespec_RX(gNB, &gNB_proc, rel15_ul->ulsch_pdu_rel15.start_symbol, rel15_ul->ulsch_pdu_rel15.start_symbol + rel15_ul->ulsch_pdu_rel15.number_symbols);


      //----------------------------------------------------------
      //---------------------- count errors ----------------------
      //----------------------------------------------------------

      for (i = 0; i < available_bits; i++) {
        
        if(((ulsch_ue[0]->g[i] == 0) && (gNB->pusch_vars[UE_id]->llr[i] <= 0)) || 
           ((ulsch_ue[0]->g[i] == 1) && (gNB->pusch_vars[UE_id]->llr[i] >= 0)))
        {
          if(errors_scrambling == 0) {
          	printf("\n");
            printf("First bit in error in unscrambling = %d\n",i);
          }
          errors_scrambling++;
        }

      }

      for (i = 0; i < TBS; i++) {

        estimated_output_bit[i] = (ulsch_gNB->harq_processes[harq_pid]->b[i/8] & (1 << (i & 7))) >> (i & 7);
        test_input_bit[i]       = (ulsch_ue[0]->harq_processes[harq_pid]->b[i / 8] & (1 << (i & 7))) >> (i & 7); // Further correct for multiple segments

        if (estimated_output_bit[i] != test_input_bit[i]) {
          if(errors_bit == 0)
            printf("First bit in error in decoding = %d\n",i);
          errors_bit++;
        }
        
      }

      ////////////////////////////////////////////////////////////

      if (errors_scrambling > 0) {
        if (n_trials == 1)
          printf("errors_scrambling = %d (trial %d)\n", errors_scrambling, trial);
      }

      if (errors_bit > 0) {
        n_false_positive++;
        if (n_trials == 1)
          printf("errors_bit = %d (trial %d)\n", errors_bit, trial);
      }
      printf("\n");
    } // [hna] for (trial = 0; trial < n_trials; trial++)
    
    printf("*****************************************\n");
    printf("SNR %f, (false positive %f)\n", SNR,
           (float) n_false_positive / (float) n_trials);
    printf("*****************************************\n");
    printf("\n");

    if (errors_bit == 0) {
      printf("PUSCH test OK\n");
      printf("\n");
      break;
    }
    printf("\n");
  } // [hna] for (SNR = snr0; SNR < snr1; SNR += snr_step)


  for (i = 0; i < 2; i++) {

    printf("----------------------\n");
    printf("freeing codeword %d\n", i);
    printf("----------------------\n");

    printf("gNB ulsch[0][%d]\n", i); // [hna] ulsch[0] is for RA

    free_gNB_ulsch(gNB->ulsch[0][i]);

    printf("gNB ulsch[%d][%d]\n",UE_id+1, i);

    free_gNB_ulsch(gNB->ulsch[UE_id+1][i]); // "+1" because first element in ulsch is for RA

    for (sf = 0; sf < 2; sf++) {

      printf("UE  ulsch[%d][0][%d]\n", sf, i);

      if (UE->ulsch[sf][0][i])
        free_nr_ue_ulsch(UE->ulsch[sf][0][i]);
    }

    printf("\n");
  }

  if (output_fd)
    fclose(output_fd);

  if (input_fd)
    fclose(input_fd);

  return (n_errors);
}
