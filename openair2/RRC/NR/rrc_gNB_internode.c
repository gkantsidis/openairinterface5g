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

/*! \file rrc_gNB_internode.c
 * \brief rrc internode procedures for gNB
 * \author Raymond Knopp
 * \date 2019
 * \version 1.0
 * \company Eurecom
 * \email: raymond.knopp@eurecom.fr
 */
#ifndef RRC_GNB_INTERNODE_C
#define RRC_GNB_INTERNODE_C

#include "nr_rrc_defs.h"
#include "NR_RRCReconfiguration.h"
#include "NR_UE-NR-Capability.h"
#include "NR_CG-ConfigInfo.h"
#include "NR_UE-CapabilityRAT-ContainerList.h"
#include "NR_CG-Config.h"

int parse_CG_ConfigInfo(gNB_RRC_INST *rrc, NR_CG_ConfigInfo_t *CG_ConfigInfo) {

  if (CG_ConfigInfo->criticalExtensions.present == NR_CG_ConfigInfo__criticalExtensions_PR_c1) {
    if (CG_ConfigInfo->criticalExtensions.choice.c1) {
      if (CG_ConfigInfo->criticalExtensions.choice.c1->present == NR_CG_ConfigInfo__criticalExtensions__c1_PR_cg_ConfigInfo) {
	NR_CG_ConfigInfo_IEs_t *cg_ConfigInfo = CG_ConfigInfo->criticalExtensions.choice.c1->choice.cg_ConfigInfo;
	if (cg_ConfigInfo->ue_CapabilityInfo) {
	  // Decode UE-CapabilityRAT-ContainerList
	  NR_UE_CapabilityRAT_ContainerList_t **UE_CapabilityRAT_ContainerList=NULL;
	  asn_dec_rval_t dec_rval = uper_decode(NULL,
						&asn_DEF_NR_UE_NR_Capability,
						(void**)UE_CapabilityRAT_ContainerList,
						cg_ConfigInfo->ue_CapabilityInfo->buf,
						cg_ConfigInfo->ue_CapabilityInfo->size, 0, 0);
          if ((dec_rval.code != RC_OK) && (dec_rval.consumed == 0)) {
             LOG_E(RRC, "[InterNode] Failed to decode NR_UE_NR_Capability (%zu bits)\n",
                   dec_rval.consumed );
             return(-1);
          }
	  rrc_parse_ue_capabilities(rrc,*UE_CapabilityRAT_ContainerList);
	}
	if (cg_ConfigInfo->candidateCellInfoListMN) {
	  LOG_E(RRC,"Can't handle candidateCellInfoListMN yet\n");
	  return(-1);
	}
      }
      else {
	LOG_E(RRC,"c1 extension is not cg_ConfigInfo, returning\n");
	return(-1); 
      }
    }
    else {
      LOG_E(RRC,"c1 extension not found, returning\n");
      return(-1); 
    }
  } else {
    LOG_E(RRC,"Ignoring unknown CG_ConfigInfo extensions\n");
    return(-1);
  }
  return(0);  
}


int generate_CG_Config(gNB_RRC_INST *rrc, 
		       NR_CG_Config_t *cg_Config,
		       NR_RRCReconfiguration_t *reconfig,
		       NR_RadioBearerConfig_t *rbconfig) {


  cg_Config->criticalExtensions.present = NR_CG_Config__criticalExtensions_PR_c1;
  cg_Config->criticalExtensions.choice.c1 = calloc(1,sizeof(*cg_Config->criticalExtensions.choice.c1));
  cg_Config->criticalExtensions.choice.c1->present = NR_CG_Config__criticalExtensions__c1_PR_cg_Config;
  cg_Config->criticalExtensions.choice.c1->choice.cg_Config = calloc(1,sizeof(NR_CG_Config_IEs_t));
  char buffer[1024];
  asn_enc_rval_t enc_rval = uper_encode_to_buffer(&asn_DEF_NR_RRCReconfiguration, NULL, (void *)reconfig, buffer, 1024);
  AssertFatal (enc_rval.encoded > 0, "ASN1 message encoding failed (%s, %jd)!\n",
               enc_rval.failed_type->name, enc_rval.encoded);
  cg_Config->criticalExtensions.choice.c1->choice.cg_Config->scg_CellGroupConfig = calloc(1,sizeof(OCTET_STRING_t));
  OCTET_STRING_fromBuf(cg_Config->criticalExtensions.choice.c1->choice.cg_Config->scg_CellGroupConfig,
		       (const char *)buffer,
		       (enc_rval.encoded+7)>>3); 

  LOG_I(RRC,"Dumping NR_RRCReconfiguration message (%d bytes)\n",(enc_rval.encoded+7)>>3);
  for (int i=0;i<(enc_rval.encoded+7)>>3;i++) {
    printf("%2x ",((uint8_t *)buffer)[i]);
  }
  printf("\n");
  enc_rval = uper_encode_to_buffer(&asn_DEF_NR_RadioBearerConfig, NULL, (void *)rbconfig, buffer, 1024);
  AssertFatal (enc_rval.encoded > 0, "ASN1 message encoding failed (%s, %jd)!\n",
               enc_rval.failed_type->name, enc_rval.encoded);
  cg_Config->criticalExtensions.choice.c1->choice.cg_Config->scg_RB_Config = calloc(1,sizeof(OCTET_STRING_t));
  OCTET_STRING_fromBuf(cg_Config->criticalExtensions.choice.c1->choice.cg_Config->scg_RB_Config,
		       (const char *)buffer,
		       (enc_rval.encoded+7)>>3);  
  LOG_I(RRC,"Dumping scg_RB_Config message (%d bytes)\n",(enc_rval.encoded+7)>>3);
  for (int i=0;i<(enc_rval.encoded+7)>>3;i++) {
    printf("%2x ",((uint8_t*)buffer)[i]);
  }
  printf("\n");

  return(0);
}

#endif
