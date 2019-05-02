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

/*! \file flexran_agent_s1ap.c
 * \brief FlexRAN agent Control Module S1AP
 * \author Navid Nikaein
 * \date 2019
 * \version 0.1
 */

#include "flexran_agent_s1ap.h"


/*Array containing the Agent-S1AP interfaces*/
AGENT_S1AP_xface *agent_s1ap_xface[NUM_MAX_ENB];

void flexran_agent_fill_s1ap_cell_config(mid_t mod_id,
					 Protocol__FlexS1apConfig **s1ap_config) {
  
  Protocol__FlexS1apMme **mme_conf = NULL;
  Protocol__FlexS1apUe **ue_conf = NULL;
  int i = 0 ;

  if (!*s1ap_config){
    *s1ap_config = malloc(sizeof(Protocol__FlexS1apConfig));
    if (!*s1ap_config) return;
    protocol__flex_s1ap_config__init(*s1ap_config);
    LOG_D(FLEXRAN_AGENT, "flexran_agent_fill_s1ap_cell_config %d\n", mod_id);

    // S1AP status 
    (*s1ap_config)->has_pending=1;
    (*s1ap_config)->pending= flexran_get_s1ap_mme_pending(mod_id);

    (*s1ap_config)->has_connected=1;
    (*s1ap_config)->connected=flexran_get_s1ap_mme_connected(mod_id);
    
    (*s1ap_config)->enb_s1_ip=flexran_get_s1ap_enb_s1_ip(mod_id);
    if ((*s1ap_config)->enb_s1_ip == NULL) (*s1ap_config)->enb_s1_ip="";

    (*s1ap_config)->enb_name=flexran_get_s1ap_enb_name(mod_id);
    
    (*s1ap_config)->n_mme=flexran_get_s1ap_nb_mme(mod_id) ;
    (*s1ap_config)->n_ue=flexran_get_s1ap_nb_ue(mod_id);

    // MME conf
    if (((*s1ap_config)->n_mme > 0) && ((*s1ap_config)->connected)) {
      mme_conf = malloc(sizeof(Protocol__FlexS1apMme *) * (*s1ap_config)->n_mme);
      if(mme_conf == NULL) goto error1;
      for(i = 0; i < (*s1ap_config)->n_mme; i++){
	mme_conf[i] = malloc(sizeof(Protocol__FlexS1apMme));
	if (!mme_conf[i]) goto error1;
	protocol__flex_s1ap_mme__init(mme_conf[i]);
	
	if (flexran_get_s1ap_mme_conf(mod_id, i, mme_conf[i]) < 0 ){
	  (*s1ap_config)->n_mme =0;
	  mme_conf=NULL;
	  break;
	}
      }
    } 
  
    //UE conf
    if (((*s1ap_config)->n_ue>0) && ((*s1ap_config)->connected)){
      ue_conf = malloc(sizeof(Protocol__FlexS1apUe *) * (*s1ap_config)->n_ue);
      if(ue_conf == NULL) goto error2;
      for(i = 0; i < (*s1ap_config)->n_ue; i++){
	ue_conf[i] = malloc(sizeof(Protocol__FlexS1apUe));
	if (!ue_conf[i]) goto error2;
	protocol__flex_s1ap_ue__init(ue_conf[i]);

	if (flexran_get_s1ap_ue_conf(mod_id, i, ue_conf[i]) < 0 ){
	  (*s1ap_config)->n_ue=0;
	  ue_conf=NULL;
	  break;
	}
      }
    }
  }
  (*s1ap_config)->mme=mme_conf;
  (*s1ap_config)->ue=ue_conf;
  return; 
  
 error1:
  LOG_E(FLEXRAN_AGENT, "flexran_agent_fill_s1ap_cell_config: MME malloc failed %d n_mme %d (%p)\n", mod_id,(*s1ap_config)->n_mme, s1ap_config);
 error2:
  LOG_E(FLEXRAN_AGENT, "flexran_agent_fill_s1ap_cell_config: UE malloc failed %d n_ue %d (%p)\n", mod_id,(*s1ap_config)->n_ue, s1ap_config);
}


int flexran_agent_register_s1ap_xface(mid_t mod_id) {

  if (agent_s1ap_xface[mod_id]) {
    LOG_E(FLEXRAN_AGENT, "S1AP agent CM for eNB %d is already registered\n", mod_id);
    return -1;
  }
  
  AGENT_S1AP_xface *xface = malloc(sizeof(AGENT_S1AP_xface));
  if (!xface) {
     LOG_E(FLEXRAN_AGENT, "could not allocate memory for S1AP agent xface %d\n", mod_id);
    return -1;
  }
 
  // not implmeneted yet
  xface->flexran_s1ap_notify_release_request=NULL;

  agent_s1ap_xface[mod_id] = xface;

  return 0;
}

int flexran_agent_unregister_s1ap_xface(mid_t mod_id) {

  if (!agent_s1ap_xface[mod_id]) {
    LOG_E(FLEXRAN_AGENT, "S1AP agent for eNB %d is not registered\n", mod_id);
    return -1;
  }
  // set the 
  agent_s1ap_xface[mod_id]->flexran_s1ap_notify_release_request=NULL;
    
  free(agent_s1ap_xface[mod_id]);
  agent_s1ap_xface[mod_id] = NULL;
 
  return 0;
}

AGENT_S1AP_xface *flexran_agent_get_s1ap_xface(mid_t mod_id)
{
  return agent_s1ap_xface[mod_id];
}
