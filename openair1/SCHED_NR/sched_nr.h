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

/*
  \author Guy De Souza
  \company EURECOM
  \email desouza@eurecom.fr
*/

#ifndef __openair_SCHED_NR_DEFS_H__
#define __openair_SCHED_NR_DEFS_H__

#include "PHY/defs_gNB.h"
#include "PHY_INTERFACE/phy_interface.h"
#include "SCHED/sched_eNB.h"
#include "PHY/NR_TRANSPORT/nr_dci.h"
#include "phy_frame_config_nr.h"


void nr_set_ssb_first_subcarrier(nfapi_nr_config_request_t *cfg, NR_DL_FRAME_PARMS *fp);
void phy_procedures_gNB_TX(PHY_VARS_gNB *gNB, int frame_tx, int slot_tx, int do_meas);
void phy_procedures_gNB_common_RX(PHY_VARS_gNB *gNB, gNB_L1_rxtx_proc_t *proc);
void phy_procedures_gNB_uespec_RX(PHY_VARS_gNB *gNB, gNB_L1_rxtx_proc_t *proc, uint8_t symbol_start, uint8_t symbol_end);
void nr_common_signal_procedures (PHY_VARS_gNB *gNB,int frame, int slot);
void nr_feptx_ofdm(RU_t *ru,int frame_tx,int tti_tx);
void nr_feptx_ofdm_2thread(RU_t *ru,int frame_tx,int tti_tx);
void nr_feptx0(RU_t *ru,int tti_tx,int first_symbol, int num_symbols);
void nr_init_feptx_thread(RU_t *ru);
void fep_full(RU_t *ru,int slot);
void feptx_prec(RU_t *ru,int frame_tx,int tti_tx);
int nr_phy_init_RU(RU_t *ru);

void nr_configure_css_dci_initial(nfapi_nr_dl_config_pdcch_parameters_rel15_t* pdcch_params,
				  nr_scs_e scs_common,
				  nr_scs_e pdcch_scs,
				  nr_frequency_range_e freq_range,
				  uint8_t rmsi_pdcch_config,
				  uint8_t ssb_idx,
                  uint8_t k_ssb,
                  uint16_t sfn_ssb,
                  uint8_t n_ssb,
				  uint16_t nb_slots_per_frame,
				  uint16_t N_RB);

#endif
