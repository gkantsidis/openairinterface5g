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
  gnb_config.c
  -------------------
  AUTHOR  : Lionel GAUTHIER, navid nikaein, Laurent Winckel, WEI-TAI CHEN
  COMPANY : EURECOM, NTUST
  EMAIL   : Lionel.Gauthier@eurecom.fr, navid.nikaein@eurecom.fr, kroempa@gmail.com
*/

#include <string.h>
#include <inttypes.h>

#include "common/utils/LOG/log.h"
#include "common/utils/LOG/log_extern.h"
#include "assertions.h"
#include "gnb_config.h"
#include "UTIL/OTG/otg.h"
#include "UTIL/OTG/otg_externs.h"
#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
# if defined(ENABLE_USE_MME)
#   include "s1ap_eNB.h"
#   include "sctp_eNB_task.h"
# endif
#endif
#include "sctp_default_values.h"
// #include "SystemInformationBlockType2.h"
// #include "LAYER2/MAC/extern.h"
// #include "LAYER2/MAC/proto.h"
#include "PHY/phy_extern.h"
#include "PHY/INIT/phy_init.h"
#include "targets/ARCH/ETHERNET/USERSPACE/LIB/ethernet_lib.h"
#include "nfapi_vnf.h"
#include "nfapi_pnf.h"

#include "L1_paramdef.h"
#include "MACRLC_paramdef.h"
#include "common/config/config_userapi.h"
//#include "RRC_config_tools.h"
#include "gnb_paramdef.h"
#include "LAYER2/NR_MAC_gNB/mac_proto.h"

#include "NR_asn_constant.h"
#include "NR_SCS-SpecificCarrier.h"
#include "NR_TDD-UL-DL-ConfigCommon.h"
#include "NR_FrequencyInfoUL.h"
#include "NR_RACH-ConfigGeneric.h"
#include "NR_RACH-ConfigCommon.h"
#include "NR_PUSCH-TimeDomainResourceAllocation.h"
#include "NR_PUSCH-ConfigCommon.h"
#include "NR_PUCCH-ConfigCommon.h"
#include "NR_PDSCH-TimeDomainResourceAllocation.h"
#include "NR_PDSCH-ConfigCommon.h"
#include "NR_RateMatchPattern.h"
#include "NR_RateMatchPatternLTE-CRS.h"
#include "NR_SearchSpace.h"
#include "NR_ControlResourceSet.h"
#include "NR_EUTRA-MBSFN-SubframeConfig.h"

extern uint16_t sf_ahead;
extern void set_parallel_conf(char *parallel_conf);
extern void set_worker_conf(char *worker_conf);
extern PARALLEL_CONF_t get_thread_parallel_conf(void);
extern WORKER_CONF_t   get_thread_worker_conf(void);
extern int config_check_band_frequencies(int ind, int16_t band, uint32_t downlink_frequency,
                                         int32_t uplink_frequency_offset, uint32_t  frame_type);

void prepare_scc(NR_ServingCellConfigCommon_t *scc) {

  NR_FreqBandIndicatorNR_t                        *dl_frequencyBandList,*ul_frequencyBandList;
  struct NR_SCS_SpecificCarrier                   *dl_scs_SpecificCarrierList,*ul_scs_SpecificCarrierList;
  struct NR_PDSCH_TimeDomainResourceAllocation    *bwp_dl_timedomainresourceallocation;
  struct NR_PUSCH_TimeDomainResourceAllocation    *pusch_configcommontimedomainresourceallocation;
  //  struct NR_RateMatchPattern                      *ratematchpattern;
  //  NR_RateMatchPatternId_t                         *ratematchpatternid;
  //  NR_TCI_StateId_t                                *TCI_StateId;
  //  struct NR_ControlResourceSet                    *bwp_dl_controlresourceset;
  //  NR_SearchSpace_t                                *bwp_dl_searchspace;

  scc->physCellId                                = CALLOC(1,sizeof(NR_PhysCellId_t));
  scc->downlinkConfigCommon                      = CALLOC(1,sizeof(struct NR_DownlinkConfigCommon));
  scc->downlinkConfigCommon->frequencyInfoDL     = CALLOC(1,sizeof(struct NR_FrequencyInfoDL));
  scc->downlinkConfigCommon->initialDownlinkBWP  = CALLOC(1,sizeof(struct NR_BWP_DownlinkCommon));
  scc->uplinkConfigCommon                        = CALLOC(1,sizeof(struct NR_UplinkConfigCommon));
  scc->uplinkConfigCommon->frequencyInfoUL       = CALLOC(1,sizeof(struct NR_FrequencyInfoUL));
  scc->uplinkConfigCommon->initialUplinkBWP      = CALLOC(1,sizeof(struct NR_BWP_UplinkCommon));
  //scc->supplementaryUplinkConfig       = CALLOC(1,sizeof(struct NR_UplinkConfigCommon));  
  scc->ssb_PositionsInBurst                      = CALLOC(1,sizeof(struct NR_ServingCellConfigCommon__ssb_PositionsInBurst));
  scc->ssb_periodicityServingCell                = CALLOC(1,sizeof(long));
  //  scc->rateMatchPatternToAddModList              = CALLOC(1,sizeof(struct NR_ServingCellConfigCommon__rateMatchPatternToAddModList));
  //  scc->rateMatchPatternToReleaseList             = CALLOC(1,sizeof(struct NR_ServingCellConfigCommon__rateMatchPatternToReleaseList));
  scc->ssbSubcarrierSpacing                      = CALLOC(1,sizeof(NR_SubcarrierSpacing_t));
  scc->tdd_UL_DL_ConfigurationCommon             = CALLOC(1,sizeof(struct NR_TDD_UL_DL_ConfigCommon));
  scc->tdd_UL_DL_ConfigurationCommon->pattern2   = CALLOC(1,sizeof(struct NR_TDD_UL_DL_Pattern));
  
  scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencySSB     = CALLOC(1,sizeof(NR_ARFCN_ValueNR_t));
  
  dl_frequencyBandList              = CALLOC(1,sizeof(NR_FreqBandIndicatorNR_t));
  dl_scs_SpecificCarrierList        = CALLOC(1,sizeof(struct NR_SCS_SpecificCarrier));

  ASN_SEQUENCE_ADD(&scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list,dl_frequencyBandList);  
  ASN_SEQUENCE_ADD(&scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list,dl_scs_SpecificCarrierList);		   		   
  //  scc->downlinkConfigCommon->initialDownlinkBWP->genericParameters.cyclicPrefix    = CALLOC(1,sizeof(long));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon                = CALLOC(1,sizeof(struct NR_SetupRelease_PDCCH_ConfigCommon));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->present=NR_SetupRelease_PDCCH_ConfigCommon_PR_setup; 
  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup  = CALLOC(1,sizeof(struct NR_PDCCH_ConfigCommon));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->controlResourceSetZero    = CALLOC(1,sizeof(long));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->searchSpaceZero           = CALLOC(1,sizeof(long));
  //  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->commonControlResourceSet  = CALLOC(1,sizeof(struct NR_ControlResourceSet));

  //  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->commonSearchSpaceList     = CALLOC(1,sizeof(struct NR_PDCCH_ConfigCommon__commonSearchSpaceList));
  //  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->searchSpaceSIB1                    = CALLOC(1,sizeof(NR_SearchSpaceId_t));
  //  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->searchSpaceOtherSystemInformation  = CALLOC(1,sizeof(NR_SearchSpaceId_t));
  //  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->pagingSearchSpace                  = CALLOC(1,sizeof(NR_SearchSpaceId_t));
  //  scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->ra_SearchSpace                     = CALLOC(1,sizeof(NR_SearchSpaceId_t));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon                 = CALLOC(1,sizeof(struct NR_SetupRelease_PDSCH_ConfigCommon));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->present        = NR_SetupRelease_PDSCH_ConfigCommon_PR_setup;
  scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup   = CALLOC(1,sizeof(struct NR_PDSCH_ConfigCommon));
  scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList = CALLOC(1,sizeof(struct NR_PDSCH_TimeDomainResourceAllocationList));
  //
  for (int i=0;i<NR_maxNrofDL_Allocations;i++) {
    bwp_dl_timedomainresourceallocation = CALLOC(1,sizeof(NR_PDSCH_TimeDomainResourceAllocation_t));
    bwp_dl_timedomainresourceallocation->k0  = CALLOC(1,sizeof(long));
    *bwp_dl_timedomainresourceallocation->k0=0;
    ASN_SEQUENCE_ADD(&scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list,
		     bwp_dl_timedomainresourceallocation);
  }

  ul_frequencyBandList              = CALLOC(1,sizeof(NR_FreqBandIndicatorNR_t));
  scc->uplinkConfigCommon->frequencyInfoUL->frequencyBandList          = CALLOC(1,sizeof(struct NR_MultiFrequencyBandListNR));
  ASN_SEQUENCE_ADD(&scc->uplinkConfigCommon->frequencyInfoUL->frequencyBandList->list,ul_frequencyBandList);

  scc->uplinkConfigCommon->frequencyInfoUL->absoluteFrequencyPointA    = CALLOC(1,sizeof(NR_ARFCN_ValueNR_t));
  //  scc->uplinkConfigCommon->frequencyInfoUL->additionalSpectrumEmission = CALLOC(1,sizeof(NR_AdditionalSpectrumEmission_t));
  scc->uplinkConfigCommon->frequencyInfoUL->p_Max                      = CALLOC(1,sizeof(NR_P_Max_t));
  //  scc->uplinkConfigCommon->frequencyInfoUL->frequencyShift7p5khz       = CALLOC(1,sizeof(long));
  
  //  scc->uplinkConfigCommon->initialUplinkBWP->genericParameters.cyclicPrefix    = CALLOC(1,sizeof(long));
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon                 = CALLOC(1,sizeof(NR_SetupRelease_RACH_ConfigCommon_t));
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->present = NR_SetupRelease_RACH_ConfigCommon_PR_setup;
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup   = CALLOC(1,sizeof(struct NR_RACH_ConfigCommon));
  //  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->totalNumberOfRA_Preambles                  = CALLOC(1,sizeof(long));
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->ssb_perRACH_OccasionAndCB_PreamblesPerSSB  = CALLOC(1,sizeof(struct NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB));
  //  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->groupBconfigured                           = CALLOC(1,sizeof(struct NR_RACH_ConfigCommon__groupBconfigured));
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rsrp_ThresholdSSB            = CALLOC(1,sizeof(NR_RSRP_Range_t));
  //  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rsrp_ThresholdSSB_SUL        = CALLOC(1,sizeof(NR_RSRP_Range_t));
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg1_SubcarrierSpacing       = CALLOC(1,sizeof(NR_SubcarrierSpacing_t));
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg3_transformPrecoder       = CALLOC(1,sizeof(long));
  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon                 = CALLOC(1,sizeof(NR_SetupRelease_PUSCH_ConfigCommon_t)); 
  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->present        = NR_SetupRelease_PUSCH_ConfigCommon_PR_setup;
  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup   = CALLOC(1,sizeof(struct NR_PUSCH_ConfigCommon));

  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->groupHoppingEnabledTransformPrecoding = CALLOC(1,sizeof(long));
  
  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList  = CALLOC(1,sizeof(struct NR_PUSCH_TimeDomainResourceAllocationList));
  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->msg3_DeltaPreamble              = CALLOC(1,sizeof(long));
  scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->p0_NominalWithGrant             = CALLOC(1,sizeof(long));
  
  scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon                                                = CALLOC(1,sizeof(struct NR_SetupRelease_PUCCH_ConfigCommon)); 
  scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon->present= NR_SetupRelease_PUCCH_ConfigCommon_PR_setup;
  scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon->choice.setup                                  = CALLOC(1,sizeof(struct NR_PUCCH_ConfigCommon));
  scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon->choice.setup->p0_nominal                      = CALLOC(1,sizeof(long));
  scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon->choice.setup->pucch_ResourceCommon            = CALLOC(1,sizeof(long));
  scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon->choice.setup->hoppingId                       = CALLOC(1,sizeof(long));

  //  scc->ssb_PositionsInBurst->choice.shortBitmap.buf  = MALLOC(1);
  //  scc->ssb_PositionsInBurst->choice.mediumBitmap.buf = MALLOC(1);
  //  scc->ssb_PositionsInBurst->choice.longBitmap.buf   = MALLOC(8);
  

  ul_scs_SpecificCarrierList  = CALLOC(1,sizeof(struct NR_SCS_SpecificCarrier));
  ASN_SEQUENCE_ADD(&scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list,ul_scs_SpecificCarrierList);

  for (int i=0;i<NR_maxNrofUL_Allocations;i++) {
    pusch_configcommontimedomainresourceallocation      = CALLOC(1,sizeof(struct NR_PUSCH_TimeDomainResourceAllocation));
    pusch_configcommontimedomainresourceallocation->k2  = CALLOC(1,sizeof(long));
    *pusch_configcommontimedomainresourceallocation->k2=0;
    ASN_SEQUENCE_ADD(&scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list,pusch_configcommontimedomainresourceallocation); 
  }
  //ratematchpattern                              = CALLOC(1,sizeof(struct NR_RateMatchPattern));
  //ratematchpattern->patternType.choice.bitmaps  = CALLOC(1,sizeof(struct NR_RateMatchPattern__patternType__bitmaps));
  //ratematchpattern->patternType.choice.bitmaps->resourceBlocks.buf = MALLOC(35);
  //ratematchpattern->patternType.choice.bitmaps->symbolsInResourceBlock.choice.oneSlot.buf   = MALLOC(2);
  //ratematchpattern->patternType.choice.bitmaps->symbolsInResourceBlock.choice.twoSlots.buf  = MALLOC(4);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern                       = CALLOC(1,sizeof(struct NR_RateMatchPattern__patternType__bitmaps__periodicityAndPattern));
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n2.buf        = MALLOC(1);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n4.buf        = MALLOC(1);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n5.buf        = MALLOC(1);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n8.buf        = MALLOC(1);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n10.buf       = MALLOC(2);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n20.buf       = MALLOC(3);
  //ratematchpattern->patternType.choice.bitmaps->periodicityAndPattern->choice.n40.buf       = MALLOC(5);
  //ratematchpattern->subcarrierSpacing           = CALLOC(1,sizeof(NR_SubcarrierSpacing_t));
  //ratematchpatternid                            = CALLOC(1,sizeof(NR_RateMatchPatternId_t));
  
}


void fix_scc(NR_ServingCellConfigCommon_t *scc,int ssbmap) {


  int ssbmaplen = (int)scc->ssb_PositionsInBurst->present;
  AssertFatal(ssbmaplen==NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_shortBitmap || ssbmaplen==NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_mediumBitmap || ssbmaplen==NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_longBitmap, "illegal ssbmaplen %d\n",ssbmaplen);
  if(ssbmaplen==NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_shortBitmap){
    scc->ssb_PositionsInBurst->choice.shortBitmap.size = 1;
    scc->ssb_PositionsInBurst->choice.shortBitmap.bits_unused = 4;
    scc->ssb_PositionsInBurst->choice.shortBitmap.buf = CALLOC(1,1);
    scc->ssb_PositionsInBurst->choice.shortBitmap.buf[0] = ssbmap;
  }else if(ssbmaplen==NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_mediumBitmap){
	  scc->ssb_PositionsInBurst->choice.mediumBitmap.size = 1;
	  scc->ssb_PositionsInBurst->choice.mediumBitmap.bits_unused = 0;
    scc->ssb_PositionsInBurst->choice.mediumBitmap.buf = CALLOC(1,1);
    scc->ssb_PositionsInBurst->choice.mediumBitmap.buf[0] = ssbmap;
  }else {
    scc->ssb_PositionsInBurst->choice.longBitmap.size = 8;
    scc->ssb_PositionsInBurst->choice.longBitmap.bits_unused = 0;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf = CALLOC(1,8);
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[0] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[1] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[2] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[3] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[4] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[5] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[6] = 0xff;
    scc->ssb_PositionsInBurst->choice.longBitmap.buf[7] = 0xff;
  }

  // fix UL absolute frequency
  if ((int)*scc->uplinkConfigCommon->frequencyInfoUL->absoluteFrequencyPointA==-1) {
     free(scc->uplinkConfigCommon->frequencyInfoUL->absoluteFrequencyPointA);
     scc->uplinkConfigCommon->frequencyInfoUL->absoluteFrequencyPointA = NULL;
  }

  // fix libconfig (asn1c uses long) 
  scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rach_ConfigGeneric.preambleReceivedTargetPower-=((long)1<<32); 
  *scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->p0_NominalWithGrant-=((long)1<<32);
  *scc->uplinkConfigCommon->initialUplinkBWP->pucch_ConfigCommon->choice.setup->p0_nominal-=((long)1<<32);

  // fix DL and UL Allocation lists
  
  for (int i=scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.count-1;i>=0;i--) {
    printf("Checking element %d : %d\n",i,*scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.array[i]->k0);
    if (*scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.array[i]->k0>32) {
      printf("removing pdsch_TimeDomainAllocationList element %d\n",i);
      free(scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.array[i]->k0);
      asn_sequence_del(&scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list,i,1); 
      printf("List size now %d\n",scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.count);
    }
  }

  for (int i=scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list.count-1;i>=0;i--) {
    if (*scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list.array[i]->k2>32) {
      free(scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list.array[i]->k2);
      asn_sequence_del(&scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list,i,1); 
    }
  }
  if (scc->tdd_UL_DL_ConfigurationCommon->pattern2->dl_UL_TransmissionPeriodicity > 320 ) {
    free(scc->tdd_UL_DL_ConfigurationCommon->pattern2);
    scc->tdd_UL_DL_ConfigurationCommon->pattern2=NULL;
  }

  if ((int)*scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg1_SubcarrierSpacing == -1)
    free(scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg1_SubcarrierSpacing);
    scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg1_SubcarrierSpacing=NULL;
}

void RCconfig_nr_flexran()
{
  uint16_t  i;
  uint16_t  num_gnbs;
  char      aprefix[MAX_OPTNAME_SIZE*2 + 8];
  /* this will possibly truncate the cell id (RRC assumes int32_t).
   * Both Nid_cell and gnb_id are signed in RRC case, but we use unsigned for
   * the bitshifting to work properly */
  int32_t   Nid_cell = 0;
  uint16_t  Nid_cell_tr = 0;
  uint32_t  gnb_id = 0;


  /* get number of gNBs */
  paramdef_t GNBSParams[] = GNBSPARAMS_DESC;
  config_get(GNBSParams, sizeof(GNBSParams)/sizeof(paramdef_t), NULL);
  num_gnbs = GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt;

  /* for gNB ID */
  paramdef_t GNBParams[]  = GNBPARAMS_DESC;
  paramlist_def_t GNBParamList = {GNB_CONFIG_STRING_GNB_LIST, NULL, 0};

  paramdef_t flexranParams[] = FLEXRANPARAMS_DESC;
  config_get(flexranParams, sizeof(flexranParams)/sizeof(paramdef_t), CONFIG_STRING_NETWORK_CONTROLLER_CONFIG);

  if (!RC.flexran) {
    RC.flexran = calloc(num_gnbs, sizeof(flexran_agent_info_t*));
    AssertFatal(RC.flexran,
                "can't ALLOCATE %zu Bytes for %d flexran agent info with size %zu\n",
                num_gnbs * sizeof(flexran_agent_info_t*),
                num_gnbs, sizeof(flexran_agent_info_t*));
  }

  for (i = 0; i < num_gnbs; i++) {
    RC.flexran[i] = calloc(1, sizeof(flexran_agent_info_t));
    AssertFatal(RC.flexran[i],
                "can't ALLOCATE %zu Bytes for flexran agent info (iteration %d/%d)\n",
                sizeof(flexran_agent_info_t), i + 1, num_gnbs);
    /* if config says "yes", enable Agent, in all other cases it's like "no" */
    RC.flexran[i]->enabled          = strcasecmp(*(flexranParams[FLEXRAN_ENABLED].strptr), "yes") == 0;
    /* if not enabled, simply skip the rest, it is not needed anyway */
    if (!RC.flexran[i]->enabled)
      continue;
    RC.flexran[i]->interface_name   = strdup(*(flexranParams[FLEXRAN_INTERFACE_NAME_IDX].strptr));
    //inet_ntop(AF_INET, &(enb_properties->properties[mod_id]->flexran_agent_ipv4_address), in_ip, INET_ADDRSTRLEN);
    RC.flexran[i]->remote_ipv4_addr = strdup(*(flexranParams[FLEXRAN_IPV4_ADDRESS_IDX].strptr));
    RC.flexran[i]->remote_port      = *(flexranParams[FLEXRAN_PORT_IDX].uptr);
    RC.flexran[i]->cache_name       = strdup(*(flexranParams[FLEXRAN_CACHE_IDX].strptr));
    RC.flexran[i]->node_ctrl_state  = strcasecmp(*(flexranParams[FLEXRAN_AWAIT_RECONF_IDX].strptr), "yes") == 0 ? ENB_WAIT : ENB_NORMAL_OPERATION;

    config_getlist(&GNBParamList, GNBParams, sizeof(GNBParams)/sizeof(paramdef_t),NULL);
    /* gNB ID from configuration, as read in by RCconfig_RRC() */
    if (!GNBParamList.paramarray[i][GNB_GNB_ID_IDX].uptr) {
      // Calculate a default gNB ID
# if defined(ENABLE_USE_MME)
      gnb_id = i + (s1ap_generate_eNB_id () & 0xFFFF8);
# else
      gnb_id = i;
# endif
    } else {
        gnb_id = *(GNBParamList.paramarray[i][GNB_GNB_ID_IDX].uptr);
    }

    /* cell ID */
    sprintf(aprefix, "%s.[%i]", GNB_CONFIG_STRING_GNB_LIST, i);

    RC.flexran[i]->mod_id   = i;
    RC.flexran[i]->agent_id = (((uint64_t)i) << 48) | (((uint64_t)gnb_id) << 16) | ((uint64_t)Nid_cell_tr);

    /*
     * Assume for the moment the monolithic case, i.e. agent can provide information for all layers
     * Consider using uint16_t flexran_get_capabilities_mask(mid_t mod_id),
     *                    with RC.rrc[mod_id]->node_type = ngran_gNB
     */
    RC.flexran[i]->capability_mask = (1 << PROTOCOL__FLEX_BS_CAPABILITY__LOPHY)
    		                       | (1 << PROTOCOL__FLEX_BS_CAPABILITY__HIPHY)
								   | (1 << PROTOCOL__FLEX_BS_CAPABILITY__LOMAC)
								   | (1 << PROTOCOL__FLEX_BS_CAPABILITY__HIMAC)
								   | (1 << PROTOCOL__FLEX_BS_CAPABILITY__RLC)
								   | (1 << PROTOCOL__FLEX_BS_CAPABILITY__PDCP)
								   | (1 << PROTOCOL__FLEX_BS_CAPABILITY__SDAP)
								   | (1 << PROTOCOL__FLEX_BS_CAPABILITY__RRC);

  }
}

void RCconfig_NR_L1(void) {
  int               i,j;
  paramdef_t L1_Params[] = L1PARAMS_DESC;
  paramlist_def_t L1_ParamList = {CONFIG_STRING_L1_LIST,NULL,0};


  if (RC.gNB == NULL) {
    RC.gNB                       = (PHY_VARS_gNB **)malloc((1+NUMBER_OF_gNB_MAX)*sizeof(PHY_VARS_gNB*));
    LOG_I(NR_PHY,"RC.gNB = %p\n",RC.gNB);
    memset(RC.gNB,0,(1+NUMBER_OF_gNB_MAX)*sizeof(PHY_VARS_gNB*));
  }

  config_getlist( &L1_ParamList,L1_Params,sizeof(L1_Params)/sizeof(paramdef_t), NULL);

  if (L1_ParamList.numelt > 0) {

    for (j = 0; j < RC.nb_nr_L1_inst; j++) {

      if (RC.gNB[j] == NULL) {
        RC.gNB[j]                       = (PHY_VARS_gNB *)malloc(sizeof(PHY_VARS_gNB));
        LOG_I(NR_PHY,"RC.gNB[%d] = %p\n",j,RC.gNB[j]);
        memset(RC.gNB[j],0,sizeof(PHY_VARS_gNB));
	RC.gNB[j]->Mod_id  = j;
      }

      if(strcmp(*(L1_ParamList.paramarray[j][L1_TRANSPORT_N_PREFERENCE_IDX].strptr), "local_mac") == 0) {
        //sf_ahead = 2; // Need 4 subframe gap between RX and TX
      }else if (strcmp(*(L1_ParamList.paramarray[j][L1_TRANSPORT_N_PREFERENCE_IDX].strptr), "nfapi") == 0) {
        RC.gNB[j]->eth_params_n.local_if_name            = strdup(*(L1_ParamList.paramarray[j][L1_LOCAL_N_IF_NAME_IDX].strptr));
        RC.gNB[j]->eth_params_n.my_addr                  = strdup(*(L1_ParamList.paramarray[j][L1_LOCAL_N_ADDRESS_IDX].strptr));
        RC.gNB[j]->eth_params_n.remote_addr              = strdup(*(L1_ParamList.paramarray[j][L1_REMOTE_N_ADDRESS_IDX].strptr));
        RC.gNB[j]->eth_params_n.my_portc                 = *(L1_ParamList.paramarray[j][L1_LOCAL_N_PORTC_IDX].iptr);
        RC.gNB[j]->eth_params_n.remote_portc             = *(L1_ParamList.paramarray[j][L1_REMOTE_N_PORTC_IDX].iptr);
        RC.gNB[j]->eth_params_n.my_portd                 = *(L1_ParamList.paramarray[j][L1_LOCAL_N_PORTD_IDX].iptr);
        RC.gNB[j]->eth_params_n.remote_portd             = *(L1_ParamList.paramarray[j][L1_REMOTE_N_PORTD_IDX].iptr);
        RC.gNB[j]->eth_params_n.transp_preference        = ETH_UDP_MODE;

        //sf_ahead = 2; // Cannot cope with 4 subframes betweem RX and TX - set it to 2

        RC.nb_nr_macrlc_inst = 1;  // This is used by mac_top_init_gNB()

        // This is used by init_gNB_afterRU()
        RC.nb_nr_CC = (int *)malloc((1+RC.nb_nr_inst)*sizeof(int));
        RC.nb_nr_CC[0]=1;

        RC.nb_nr_inst =1; // DJP - feptx_prec uses num_gNB but phy_init_RU uses nb_nr_inst

        LOG_I(PHY,"%s() NFAPI PNF mode - RC.nb_nr_inst=1 this is because phy_init_RU() uses that to index and not RC.num_gNB - why the 2 similar variables?\n", __FUNCTION__);
        LOG_I(PHY,"%s() NFAPI PNF mode - RC.nb_nr_CC[0]=%d for init_gNB_afterRU()\n", __FUNCTION__, RC.nb_nr_CC[0]);
        LOG_I(PHY,"%s() NFAPI PNF mode - RC.nb_nr_macrlc_inst:%d because used by mac_top_init_gNB()\n", __FUNCTION__, RC.nb_nr_macrlc_inst);

        mac_top_init_gNB();

        configure_nfapi_pnf(RC.gNB[j]->eth_params_n.remote_addr, RC.gNB[j]->eth_params_n.remote_portc, RC.gNB[j]->eth_params_n.my_addr, RC.gNB[j]->eth_params_n.my_portd, RC.gNB[j]->eth_params_n     .remote_portd);
      }else { // other midhaul
      } 
    }// for (j = 0; j < RC.nb_nr_L1_inst; j++)
    printf("Initializing northbound interface for L1\n");
    l1_north_init_gNB();
  }else{
    LOG_I(PHY,"No " CONFIG_STRING_L1_LIST " configuration found");    

    // DJP need to create some structures for VNF

    j = 0;


    if (RC.gNB[j] == NULL) {
      RC.gNB[j] = (PHY_VARS_gNB *)malloc(sizeof(PHY_VARS_gNB));
      memset((void*)RC.gNB[j],0,sizeof(PHY_VARS_gNB));
      LOG_I(PHY,"RC.gNB[%d] = %p\n",j,RC.gNB[j]);
      RC.gNB[j]->Mod_id  = j;
    }   
  }
}

void RCconfig_nr_macrlc() {
  int               j;

  paramdef_t MacRLC_Params[] = MACRLCPARAMS_DESC;
  paramlist_def_t MacRLC_ParamList = {CONFIG_STRING_MACRLC_LIST,NULL,0};

  config_getlist( &MacRLC_ParamList,MacRLC_Params,sizeof(MacRLC_Params)/sizeof(paramdef_t), NULL);    
  
  if ( MacRLC_ParamList.numelt > 0) {

    RC.nb_nr_macrlc_inst=MacRLC_ParamList.numelt; 
    mac_top_init_gNB();   
    RC.nb_nr_mac_CC = (int*)malloc(RC.nb_nr_macrlc_inst*sizeof(int));

    for (j=0;j<RC.nb_nr_macrlc_inst;j++) {
      RC.nb_nr_mac_CC[j] = *(MacRLC_ParamList.paramarray[j][MACRLC_CC_IDX].iptr);
      //RC.nrmac[j]->phy_test = *(MacRLC_ParamList.paramarray[j][MACRLC_PHY_TEST_IDX].iptr);
      //printf("PHY_TEST = %d,%d\n", RC.nrmac[j]->phy_test, j);

      if (strcmp(*(MacRLC_ParamList.paramarray[j][MACRLC_TRANSPORT_N_PREFERENCE_IDX].strptr), "local_RRC") == 0) {
  // check number of instances is same as RRC/PDCP
  
      }else if (strcmp(*(MacRLC_ParamList.paramarray[j][MACRLC_TRANSPORT_N_PREFERENCE_IDX].strptr), "cudu") == 0) {
        RC.nrmac[j]->eth_params_n.local_if_name            = strdup(*(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_N_IF_NAME_IDX].strptr));
        RC.nrmac[j]->eth_params_n.my_addr                  = strdup(*(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_N_ADDRESS_IDX].strptr));
        RC.nrmac[j]->eth_params_n.remote_addr              = strdup(*(MacRLC_ParamList.paramarray[j][MACRLC_REMOTE_N_ADDRESS_IDX].strptr));
        RC.nrmac[j]->eth_params_n.my_portc                 = *(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_N_PORTC_IDX].iptr);
        RC.nrmac[j]->eth_params_n.remote_portc             = *(MacRLC_ParamList.paramarray[j][MACRLC_REMOTE_N_PORTC_IDX].iptr);
        RC.nrmac[j]->eth_params_n.my_portd                 = *(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_N_PORTD_IDX].iptr);
        RC.nrmac[j]->eth_params_n.remote_portd             = *(MacRLC_ParamList.paramarray[j][MACRLC_REMOTE_N_PORTD_IDX].iptr);;
        RC.nrmac[j]->eth_params_n.transp_preference        = ETH_UDP_MODE;
      }else { // other midhaul
        AssertFatal(1==0,"MACRLC %d: %s unknown northbound midhaul\n",j, *(MacRLC_ParamList.paramarray[j][MACRLC_TRANSPORT_N_PREFERENCE_IDX].strptr));
      } 

      if (strcmp(*(MacRLC_ParamList.paramarray[j][MACRLC_TRANSPORT_S_PREFERENCE_IDX].strptr), "local_L1") == 0) {

      }else if (strcmp(*(MacRLC_ParamList.paramarray[j][MACRLC_TRANSPORT_S_PREFERENCE_IDX].strptr), "nfapi") == 0) {
        RC.nrmac[j]->eth_params_s.local_if_name            = strdup(*(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_S_IF_NAME_IDX].strptr));
        RC.nrmac[j]->eth_params_s.my_addr                  = strdup(*(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_S_ADDRESS_IDX].strptr));
        RC.nrmac[j]->eth_params_s.remote_addr              = strdup(*(MacRLC_ParamList.paramarray[j][MACRLC_REMOTE_S_ADDRESS_IDX].strptr));
        RC.nrmac[j]->eth_params_s.my_portc                 = *(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_S_PORTC_IDX].iptr);
        RC.nrmac[j]->eth_params_s.remote_portc             = *(MacRLC_ParamList.paramarray[j][MACRLC_REMOTE_S_PORTC_IDX].iptr);
        RC.nrmac[j]->eth_params_s.my_portd                 = *(MacRLC_ParamList.paramarray[j][MACRLC_LOCAL_S_PORTD_IDX].iptr);
        RC.nrmac[j]->eth_params_s.remote_portd             = *(MacRLC_ParamList.paramarray[j][MACRLC_REMOTE_S_PORTD_IDX].iptr);
        RC.nrmac[j]->eth_params_s.transp_preference        = ETH_UDP_MODE;

        //sf_ahead = 2; // Cannot cope with 4 subframes betweem RX and TX - set it to 2

        printf("**************** vnf_port:%d\n", RC.mac[j]->eth_params_s.my_portc);
        configure_nfapi_vnf(RC.nrmac[j]->eth_params_s.my_addr, RC.nrmac[j]->eth_params_s.my_portc);
        printf("**************** RETURNED FROM configure_nfapi_vnf() vnf_port:%d\n", RC.nrmac[j]->eth_params_s.my_portc);
      }else { // other midhaul
        AssertFatal(1==0,"MACRLC %d: %s unknown southbound midhaul\n",j,*(MacRLC_ParamList.paramarray[j][MACRLC_TRANSPORT_S_PREFERENCE_IDX].strptr));
      } 
    }//  for (j=0;j<RC.nb_nr_macrlc_inst;j++)
  }else {// MacRLC_ParamList.numelt > 0
    AssertFatal (0,"No " CONFIG_STRING_MACRLC_LIST " configuration found");     
  }

}

void RCconfig_NRRRC(MessageDef *msg_p, uint32_t i, gNB_RRC_INST *rrc) {

  int                    num_gnbs                                                      = 0;
  char aprefix[MAX_OPTNAME_SIZE*2 + 8];
  int32_t                gnb_id                                                        = 0;
  int k;
  paramdef_t GNBSParams[] = GNBSPARAMS_DESC;
  ////////// Identification parameters
  paramdef_t GNBParams[]  = GNBPARAMS_DESC;
  paramlist_def_t GNBParamList = {GNB_CONFIG_STRING_GNB_LIST,NULL,0};

  NR_ServingCellConfigCommon_t *scc = calloc(1,sizeof(NR_ServingCellConfigCommon_t));
  int ssb_SubcarrierOffset = 0;
  int ssb_bitmap=0xff;
  memset((void*)scc,0,sizeof(NR_ServingCellConfigCommon_t));
  prepare_scc(scc);
  paramdef_t SCCsParams[] = SCCPARAMS_DESC(scc);
  paramlist_def_t SCCsParamList = {GNB_CONFIG_STRING_SERVINGCELLCONFIGCOMMON, NULL, 0};
  paramdef_t SSBsParams[] = SSBPARAMS_DESC;
  paramlist_def_t SSBsParamList = {GNB_CONFIG_STRING_SSBSUBCARRIEROFFSET, NULL, 0};
   ////////// Physical parameters


  /* get global parameters, defined outside any section in the config file */
 
  config_get( GNBSParams,sizeof(GNBSParams)/sizeof(paramdef_t),NULL); 
  num_gnbs = GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt;
  AssertFatal (i<num_gnbs,"Failed to parse config file no %ith element in %s \n",i, GNB_CONFIG_STRING_ACTIVE_GNBS);

  if (num_gnbs>0) {



    // Output a list of all gNBs. ////////// Identification parameters
    config_getlist( &GNBParamList,GNBParams,sizeof(GNBParams)/sizeof(paramdef_t),NULL); 
    
    if (GNBParamList.paramarray[i][GNB_GNB_ID_IDX].uptr == NULL) {
      // Calculate a default gNB ID
# if defined(ENABLE_USE_MME)
      uint32_t hash;
      hash = s1ap_generate_eNB_id ();
      gnb_id = i + (hash & 0xFFFF8);
# else
      gnb_id = i;
# endif
    } else {
      gnb_id = *(GNBParamList.paramarray[i][GNB_GNB_ID_IDX].uptr);
    }

    sprintf(aprefix, "%s.[%i]", GNB_CONFIG_STRING_GNB_LIST, 0);
    config_getlist(&SSBsParamList, NULL, 0, aprefix);
    if (SSBsParamList.numelt > 0) config_get(SSBsParams,sizeof(SSBsParams)/sizeof(paramdef_t),aprefix);


    config_getlist(&SCCsParamList, NULL, 0, aprefix);
    if (SCCsParamList.numelt > 0) {    
      sprintf(aprefix, "%s.[%i].%s.[%i]", GNB_CONFIG_STRING_GNB_LIST,0,GNB_CONFIG_STRING_SERVINGCELLCONFIGCOMMON, 0);
      config_get( SCCsParams,sizeof(SCCsParams)/sizeof(paramdef_t),aprefix);  
      LOG_I(RRC,"Read in ServingCellConfigCommon (PhysCellId %d, ABSFREQSSB %d, DLBand %d, ABSFREQPOINTA %d, DLBW %d,RACH_TargetReceivedPower %d\n",
	    (int)*scc->physCellId,
	    (int)*scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencySSB,
	    (int)*scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0],
	    (int)scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencyPointA,
	    (int)scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->carrierBandwidth,
	    (int)scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rach_ConfigGeneric.preambleReceivedTargetPower);
      fix_scc(scc,ssb_bitmap);

    }
    printf("NRRRC %d: Southbound Transport %s\n",i,*(GNBParamList.paramarray[i][GNB_TRANSPORT_S_PREFERENCE_IDX].strptr));
    
    if (strcmp(*(GNBParamList.paramarray[i][GNB_TRANSPORT_S_PREFERENCE_IDX].strptr), "local_mac") == 0) {
      
    } else if (strcmp(*(GNBParamList.paramarray[i][GNB_TRANSPORT_S_PREFERENCE_IDX].strptr), "cudu") == 0) {
      rrc->eth_params_s.local_if_name            = strdup(*(GNBParamList.paramarray[i][GNB_LOCAL_S_IF_NAME_IDX].strptr));
      rrc->eth_params_s.my_addr                  = strdup(*(GNBParamList.paramarray[i][GNB_LOCAL_S_ADDRESS_IDX].strptr));
      rrc->eth_params_s.remote_addr              = strdup(*(GNBParamList.paramarray[i][GNB_REMOTE_S_ADDRESS_IDX].strptr));
      rrc->eth_params_s.my_portc                 = *(GNBParamList.paramarray[i][GNB_LOCAL_S_PORTC_IDX].uptr);
      rrc->eth_params_s.remote_portc             = *(GNBParamList.paramarray[i][GNB_REMOTE_S_PORTC_IDX].uptr);
      rrc->eth_params_s.my_portd                 = *(GNBParamList.paramarray[i][GNB_LOCAL_S_PORTD_IDX].uptr);
      rrc->eth_params_s.remote_portd             = *(GNBParamList.paramarray[i][GNB_REMOTE_S_PORTD_IDX].uptr);
      rrc->eth_params_s.transp_preference        = ETH_UDP_MODE;
    } else { // other midhaul
    }       
    
    // search if in active list
    
    for (k=0; k <num_gnbs ; k++) {
      if (strcmp(GNBSParams[GNB_ACTIVE_GNBS_IDX].strlistptr[k], *(GNBParamList.paramarray[i][GNB_GNB_NAME_IDX].strptr) )== 0) {
	
        char gnbpath[MAX_OPTNAME_SIZE + 8];
        sprintf(gnbpath,"%s.[%i]",GNB_CONFIG_STRING_GNB_LIST,k);

	
        paramdef_t PLMNParams[] = GNBPLMNPARAMS_DESC;

        paramlist_def_t PLMNParamList = {GNB_CONFIG_STRING_PLMN_LIST, NULL, 0};
        /* map parameter checking array instances to parameter definition array instances */
        checkedparam_t config_check_PLMNParams [] = PLMNPARAMS_CHECK;

        for (int I = 0; I < sizeof(PLMNParams) / sizeof(paramdef_t); ++I)
          PLMNParams[I].chkPptr = &(config_check_PLMNParams[I]);

        NRRRC_CONFIGURATION_REQ (msg_p).cell_identity     = gnb_id;
        NRRRC_CONFIGURATION_REQ (msg_p).tac               = *GNBParamList.paramarray[i][GNB_TRACKING_AREA_CODE_IDX].uptr;
        AssertFatal(!GNBParamList.paramarray[i][GNB_MOBILE_COUNTRY_CODE_IDX_OLD].strptr
                    && !GNBParamList.paramarray[i][GNB_MOBILE_NETWORK_CODE_IDX_OLD].strptr,
                    "It seems that you use an old configuration file. Please change the existing\n"
                    "    tracking_area_code  =  \"1\";\n"
                    "    mobile_country_code =  \"208\";\n"
                    "    mobile_network_code =  \"93\";\n"
                    "to\n"
                    "    tracking_area_code  =  1; // no string!!\n"
                    "    plmn_list = ( { mcc = 208; mnc = 93; mnc_length = 2; } )\n");
        config_getlist(&PLMNParamList, PLMNParams, sizeof(PLMNParams)/sizeof(paramdef_t), gnbpath);

        if (PLMNParamList.numelt < 1 || PLMNParamList.numelt > 6)
          AssertFatal(0, "The number of PLMN IDs must be in [1,6], but is %d\n",
                      PLMNParamList.numelt);

        RRC_CONFIGURATION_REQ(msg_p).num_plmn = PLMNParamList.numelt;

        for (int l = 0; l < PLMNParamList.numelt; ++l) {
	
	  NRRRC_CONFIGURATION_REQ (msg_p).mcc[l]               = *PLMNParamList.paramarray[l][GNB_MOBILE_COUNTRY_CODE_IDX].uptr;
	  NRRRC_CONFIGURATION_REQ (msg_p).mnc[l]               = *PLMNParamList.paramarray[l][GNB_MOBILE_NETWORK_CODE_IDX].uptr;
	  NRRRC_CONFIGURATION_REQ (msg_p).mnc_digit_length[l]  = *PLMNParamList.paramarray[l][GNB_MNC_DIGIT_LENGTH].u8ptr;
	  AssertFatal((NRRRC_CONFIGURATION_REQ (msg_p).mnc_digit_length[l] == 2) ||
		      (NRRRC_CONFIGURATION_REQ (msg_p).mnc_digit_length[l] == 3),"BAD MNC DIGIT LENGTH %d",
		      NRRRC_CONFIGURATION_REQ (msg_p).mnc_digit_length[l]);
	}
	
        // Parse optional physical parameters
        sprintf(gnbpath,"%s.[%i]",GNB_CONFIG_STRING_GNB_LIST,k),


	NRRRC_CONFIGURATION_REQ (msg_p).ssb_SubcarrierOffset = ssb_SubcarrierOffset;
	NRRRC_CONFIGURATION_REQ (msg_p).scc = scc;	   
	  
      }//


    }//End for (k=0; k <num_gnbs ; k++)


  }//End if (num_gnbs>0)


}//End RCconfig_NRRRC function

int RCconfig_nr_gtpu(void ) {

  int               num_gnbs                      = 0;



  char*             gnb_interface_name_for_S1U    = NULL;
  char*             gnb_ipv4_address_for_S1U      = NULL;
  uint32_t          gnb_port_for_S1U              = 0;
  char             *address                       = NULL;
  char             *cidr                          = NULL;
  char gtpupath[MAX_OPTNAME_SIZE*2 + 8];
    

  paramdef_t GNBSParams[] = GNBSPARAMS_DESC;
  
  paramdef_t GTPUParams[]  = GNBGTPUPARAMS_DESC;
  LOG_I(GTPU,"Configuring GTPu\n");

/* get number of active eNodeBs */
  config_get( GNBSParams,sizeof(GNBSParams)/sizeof(paramdef_t),NULL); 
  num_gnbs = GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt;
  AssertFatal (num_gnbs >0,
           "Failed to parse config file no active gNodeBs in %s \n", GNB_CONFIG_STRING_ACTIVE_GNBS);


  sprintf(gtpupath,"%s.[%i].%s",GNB_CONFIG_STRING_GNB_LIST,0,GNB_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);
  config_get( GTPUParams,sizeof(GTPUParams)/sizeof(paramdef_t),gtpupath);    



    cidr = gnb_ipv4_address_for_S1U;
    address = strtok(cidr, "/");
    
    if (address) {
      MessageDef *message;
      AssertFatal((message = itti_alloc_new_message(TASK_GNB_APP, GTPV1U_ENB_S1_REQ))!=NULL,"");
      IPV4_STR_ADDR_TO_INT_NWBO ( address, RC.gtpv1u_data_g->enb_ip_address_for_S1u_S12_S4_up, "BAD IP ADDRESS FORMAT FOR eNB S1_U !\n" );
      LOG_I(GTPU,"Configuring GTPu address : %s -> %x\n",address,RC.gtpv1u_data_g->enb_ip_address_for_S1u_S12_S4_up);
      GTPV1U_ENB_S1_REQ(message).enb_port_for_S1u_S12_S4_up = gnb_port_for_S1U;
      itti_send_msg_to_task (TASK_GTPV1_U, 0, message); // data model is wrong: gtpu doesn't have enb_id (or module_id)
    } else
    LOG_E(GTPU,"invalid address for S1U\n");


return 0;
}

int RCconfig_NR_S1(MessageDef *msg_p, uint32_t i) {

  int               j,k = 0;
  int               gnb_id;
  int32_t           my_int;
  const char*       active_gnb[MAX_GNB];
  char             *address                       = NULL;
  char             *cidr                          = NULL;

  // for no gcc warnings 

  (void)  my_int;

  memset((char*)active_gnb,0,MAX_GNB* sizeof(char*));

  paramdef_t GNBSParams[] = GNBSPARAMS_DESC;
  paramdef_t GNBParams[]  = GNBPARAMS_DESC;
  paramlist_def_t GNBParamList = {GNB_CONFIG_STRING_GNB_LIST,NULL,0};

  /* get global parameters, defined outside any section in the config file */
  config_get( GNBSParams,sizeof(GNBSParams)/sizeof(paramdef_t),NULL); 

  /*
#if defined(ENABLE_ITTI) && defined(ENABLE_USE_MME)
    if (strcasecmp( *(GNBSParams[GNB_ASN1_VERBOSITY_IDX].strptr), GNB_CONFIG_STRING_ASN1_VERBOSITY_NONE) == 0) {
      asn_debug      = 0;
      asn1_xer_print = 0;
    } else if (strcasecmp( *(GNBSParams[GNB_ASN1_VERBOSITY_IDX].strptr), GNB_CONFIG_STRING_ASN1_VERBOSITY_INFO) == 0) {
      asn_debug      = 1;
      asn1_xer_print = 1;
    } else if (strcasecmp(*(GNBSParams[GNB_ASN1_VERBOSITY_IDX].strptr) , GNB_CONFIG_STRING_ASN1_VERBOSITY_ANNOYING) == 0) {
      asn_debug      = 1;
      asn1_xer_print = 2;
    } else {
      asn_debug      = 0;
      asn1_xer_print = 0;
    }
#endif
  */
  
    AssertFatal (i<GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt,
     "Failed to parse config file %s, %uth attribute %s \n",
     RC.config_file_name, i, GNB_CONFIG_STRING_ACTIVE_GNBS);
    
  
  if (GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt>0) {
    // Output a list of all gNBs.
       config_getlist( &GNBParamList,GNBParams,sizeof(GNBParams)/sizeof(paramdef_t),NULL); 
    
    
      
      
    
    if (GNBParamList.numelt > 0) {
      for (k = 0; k < GNBParamList.numelt; k++) {
	if (GNBParamList.paramarray[k][GNB_GNB_ID_IDX].uptr == NULL) {
	  // Calculate a default gNB ID
	  if (EPC_MODE_ENABLED) {
	    uint32_t hash;
	  
	  hash = s1ap_generate_eNB_id ();
	  gnb_id = k + (hash & 0xFFFF8);
	  } else {
	    gnb_id = k;
	  }
	} else {
          gnb_id = *(GNBParamList.paramarray[k][GNB_GNB_ID_IDX].uptr);
        }
  
  
	// search if in active list
	for (j=0; j < GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt; j++) {
	  if (strcmp(GNBSParams[GNB_ACTIVE_GNBS_IDX].strlistptr[j], *(GNBParamList.paramarray[k][GNB_GNB_NAME_IDX].strptr)) == 0) {
            paramdef_t PLMNParams[] = GNBPLMNPARAMS_DESC;
            paramlist_def_t PLMNParamList = {GNB_CONFIG_STRING_PLMN_LIST, NULL, 0};
            /* map parameter checking array instances to parameter definition array instances */
            checkedparam_t config_check_PLMNParams [] = PLMNPARAMS_CHECK;

            for (int I = 0; I < sizeof(PLMNParams) / sizeof(paramdef_t); ++I)
              PLMNParams[I].chkPptr = &(config_check_PLMNParams[I]);

	    paramdef_t S1Params[]  = GNBS1PARAMS_DESC;
	    paramlist_def_t S1ParamList = {GNB_CONFIG_STRING_MME_IP_ADDRESS,NULL,0};
	    
	    paramdef_t SCTPParams[]  = GNBSCTPPARAMS_DESC;
	    paramdef_t NETParams[]  =  GNBNETPARAMS_DESC;
	    char aprefix[MAX_OPTNAME_SIZE*2 + 8];
	    
	    S1AP_REGISTER_ENB_REQ (msg_p).eNB_id = gnb_id;
      
	    if (strcmp(*(GNBParamList.paramarray[k][GNB_CELL_TYPE_IDX].strptr), "CELL_MACRO_GNB") == 0) {
	      S1AP_REGISTER_ENB_REQ (msg_p).cell_type = CELL_MACRO_ENB;
	    } else  if (strcmp(*(GNBParamList.paramarray[k][GNB_CELL_TYPE_IDX].strptr), "CELL_HOME_GNB") == 0) {
	      S1AP_REGISTER_ENB_REQ (msg_p).cell_type = CELL_HOME_ENB;
	    } else {
	      AssertFatal (0,
			   "Failed to parse gNB configuration file %s, gnb %d unknown value \"%s\" for cell_type choice: CELL_MACRO_GNB or CELL_HOME_GNB !\n",
			   RC.config_file_name, i, *(GNBParamList.paramarray[k][GNB_CELL_TYPE_IDX].strptr));
	    }
      
	    S1AP_REGISTER_ENB_REQ (msg_p).eNB_name         = strdup(*(GNBParamList.paramarray[k][GNB_GNB_NAME_IDX].strptr));
	    S1AP_REGISTER_ENB_REQ (msg_p).tac              = *GNBParamList.paramarray[k][GNB_TRACKING_AREA_CODE_IDX].uptr;
            AssertFatal(!GNBParamList.paramarray[k][GNB_MOBILE_COUNTRY_CODE_IDX_OLD].strptr
                        && !GNBParamList.paramarray[k][GNB_MOBILE_NETWORK_CODE_IDX_OLD].strptr,
                        "It seems that you use an old configuration file. Please change the existing\n"
                        "    tracking_area_code  =  \"1\";\n"
                        "    mobile_country_code =  \"208\";\n"
                        "    mobile_network_code =  \"93\";\n"
                        "to\n"
                        "    tracking_area_code  =  1; // no string!!\n"
                        "    plmn_list = ( { mcc = 208; mnc = 93; mnc_length = 2; } )\n");
            config_getlist(&PLMNParamList, PLMNParams, sizeof(PLMNParams)/sizeof(paramdef_t), aprefix);

            if (PLMNParamList.numelt < 1 || PLMNParamList.numelt > 6)
              AssertFatal(0, "The number of PLMN IDs must be in [1,6], but is %d\n",
                          PLMNParamList.numelt);

            S1AP_REGISTER_ENB_REQ(msg_p).num_plmn = PLMNParamList.numelt;

            for (int l = 0; l < PLMNParamList.numelt; ++l) {
	    
	      S1AP_REGISTER_ENB_REQ (msg_p).mcc[l]              = *PLMNParamList.paramarray[l][GNB_MOBILE_COUNTRY_CODE_IDX].uptr;
	      S1AP_REGISTER_ENB_REQ (msg_p).mnc[l]              = *PLMNParamList.paramarray[l][GNB_MOBILE_NETWORK_CODE_IDX].uptr;
	      S1AP_REGISTER_ENB_REQ (msg_p).mnc_digit_length[l] = *PLMNParamList.paramarray[l][GNB_MNC_DIGIT_LENGTH].u8ptr;
	      S1AP_REGISTER_ENB_REQ (msg_p).default_drx      = 0;
	      AssertFatal((S1AP_REGISTER_ENB_REQ (msg_p).mnc_digit_length[l] == 2) ||
			  (S1AP_REGISTER_ENB_REQ (msg_p).mnc_digit_length[l] == 3),
			  "BAD MNC DIGIT LENGTH %d",
			  S1AP_REGISTER_ENB_REQ (msg_p).mnc_digit_length[l]);
	    }
	    sprintf(aprefix,"%s.[%i]",GNB_CONFIG_STRING_GNB_LIST,k);
	    config_getlist( &S1ParamList,S1Params,sizeof(S1Params)/sizeof(paramdef_t),aprefix); 
	    
	    S1AP_REGISTER_ENB_REQ (msg_p).nb_mme = 0;
	    
	    for (int l = 0; l < S1ParamList.numelt; l++) {
	      
	      S1AP_REGISTER_ENB_REQ (msg_p).nb_mme += 1;
	      
	      strcpy(S1AP_REGISTER_ENB_REQ (msg_p).mme_ip_address[l].ipv4_address,*(S1ParamList.paramarray[l][GNB_MME_IPV4_ADDRESS_IDX].strptr));
	      strcpy(S1AP_REGISTER_ENB_REQ (msg_p).mme_ip_address[l].ipv6_address,*(S1ParamList.paramarray[l][GNB_MME_IPV6_ADDRESS_IDX].strptr));

	      if (strcmp(*(S1ParamList.paramarray[l][GNB_MME_IP_ADDRESS_ACTIVE_IDX].strptr), "yes") == 0) {
	      } 
	      if (strcmp(*(S1ParamList.paramarray[l][GNB_MME_IP_ADDRESS_PREFERENCE_IDX].strptr), "ipv4") == 0) {
		S1AP_REGISTER_ENB_REQ (msg_p).mme_ip_address[j].ipv4 = 1;
	      } else if (strcmp(*(S1ParamList.paramarray[l][GNB_MME_IP_ADDRESS_PREFERENCE_IDX].strptr), "ipv6") == 0) {
		S1AP_REGISTER_ENB_REQ (msg_p).mme_ip_address[j].ipv6 = 1;
	      } else if (strcmp(*(S1ParamList.paramarray[l][GNB_MME_IP_ADDRESS_PREFERENCE_IDX].strptr), "no") == 0) {
		S1AP_REGISTER_ENB_REQ (msg_p).mme_ip_address[j].ipv4 = 1;
		S1AP_REGISTER_ENB_REQ (msg_p).mme_ip_address[j].ipv6 = 1;
	      }

              if (S1ParamList.paramarray[l][GNB_MME_BROADCAST_PLMN_INDEX].iptr)
                S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l] = S1ParamList.paramarray[l][GNB_MME_BROADCAST_PLMN_INDEX].numelt;
              else
                S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l] = 0;

              AssertFatal(S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l] <= S1AP_REGISTER_ENB_REQ(msg_p).num_plmn,
                          "List of broadcast PLMN to be sent to MME can not be longer than actual "
                          "PLMN list (max %d, but is %d)\n",
                          S1AP_REGISTER_ENB_REQ(msg_p).num_plmn,
                          S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l]);

              for (int el = 0; el < S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l]; ++el) {
                /* UINTARRAY gets mapped to int, see config_libconfig.c:223 */
                S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_index[l][el] = S1ParamList.paramarray[l][GNB_MME_BROADCAST_PLMN_INDEX].iptr[el];
                AssertFatal(S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_index[l][el] >= 0
                            && S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_index[l][el] < S1AP_REGISTER_ENB_REQ(msg_p).num_plmn,
                            "index for MME's MCC/MNC (%d) is an invalid index for the registered PLMN IDs (%d)\n",
                            S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_index[l][el],
                            S1AP_REGISTER_ENB_REQ(msg_p).num_plmn);
              }

              /* if no broadcasst_plmn array is defined, fill default values */
              if (S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l] == 0) {
                S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_num[l] = S1AP_REGISTER_ENB_REQ(msg_p).num_plmn;

                for (int el = 0; el < S1AP_REGISTER_ENB_REQ(msg_p).num_plmn; ++el)
                  S1AP_REGISTER_ENB_REQ(msg_p).broadcast_plmn_index[l][el] = el;
              }
	      
	    }

    
	    // SCTP SETTING
	    S1AP_REGISTER_ENB_REQ (msg_p).sctp_out_streams = SCTP_OUT_STREAMS;
	    S1AP_REGISTER_ENB_REQ (msg_p).sctp_in_streams  = SCTP_IN_STREAMS;
	    if (EPC_MODE_ENABLED) {
	      sprintf(aprefix,"%s.[%i].%s",GNB_CONFIG_STRING_GNB_LIST,k,GNB_CONFIG_STRING_SCTP_CONFIG);
	      config_get( SCTPParams,sizeof(SCTPParams)/sizeof(paramdef_t),aprefix); 
	      S1AP_REGISTER_ENB_REQ (msg_p).sctp_in_streams = (uint16_t)*(SCTPParams[GNB_SCTP_INSTREAMS_IDX].uptr);
	      S1AP_REGISTER_ENB_REQ (msg_p).sctp_out_streams = (uint16_t)*(SCTPParams[GNB_SCTP_OUTSTREAMS_IDX].uptr);
	    }

            sprintf(aprefix,"%s.[%i].%s",GNB_CONFIG_STRING_GNB_LIST,k,GNB_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);
	    // NETWORK_INTERFACES
            config_get( NETParams,sizeof(NETParams)/sizeof(paramdef_t),aprefix); 
	    
	    //    S1AP_REGISTER_ENB_REQ (msg_p).enb_interface_name_for_S1U = strdup(enb_interface_name_for_S1U);
	    cidr = *(NETParams[GNB_IPV4_ADDRESS_FOR_S1_MME_IDX].strptr);
	    address = strtok(cidr, "/");
	    
	    S1AP_REGISTER_ENB_REQ (msg_p).enb_ip_address.ipv6 = 0;
	    S1AP_REGISTER_ENB_REQ (msg_p).enb_ip_address.ipv4 = 1;
	    
	    strcpy(S1AP_REGISTER_ENB_REQ (msg_p).enb_ip_address.ipv4_address, address);
	    
	    break;
	  }
	}
      }
    }
  }
  return 0;
}

int RCconfig_nr_parallel(void) {
  char *parallel_conf = NULL;
  char *worker_conf   = NULL;
  extern char *parallel_config;
  extern char *worker_config;
  paramdef_t ThreadParams[]  = THREAD_CONF_DESC;
  paramlist_def_t THREADParamList = {THREAD_CONFIG_STRING_THREAD_STRUCT,NULL,0};
  config_getlist( &THREADParamList,NULL,0,NULL);

  if(THREADParamList.numelt>0) {
    config_getlist( &THREADParamList,ThreadParams,sizeof(ThreadParams)/sizeof(paramdef_t),NULL);
    parallel_conf = strdup(*(THREADParamList.paramarray[0][THREAD_PARALLEL_IDX].strptr));
  } else {
    parallel_conf = strdup("PARALLEL_RU_L1_TRX_SPLIT");
  }

  if(THREADParamList.numelt>0) {
    config_getlist( &THREADParamList,ThreadParams,sizeof(ThreadParams)/sizeof(paramdef_t),NULL);
    worker_conf   = strdup(*(THREADParamList.paramarray[0][THREAD_WORKER_IDX].strptr));
  } else {
    worker_conf   = strdup("WORKER_ENABLE");
  }

  if(parallel_config == NULL) set_parallel_conf(parallel_conf);
  if(worker_config == NULL)   set_worker_conf(worker_conf);

  return 0;
}

void NRRCConfig(void) {

  paramlist_def_t MACRLCParamList = {CONFIG_STRING_MACRLC_LIST,NULL,0};
  paramlist_def_t L1ParamList     = {CONFIG_STRING_L1_LIST,NULL,0};
  paramlist_def_t RUParamList     = {CONFIG_STRING_RU_LIST,NULL,0};
  paramdef_t GNBSParams[]         = GNBSPARAMS_DESC;
  
  char aprefix[MAX_OPTNAME_SIZE*2 + 8];  
  


/* get global parameters, defined outside any section in the config file */
 
  printf("Getting GNBSParams\n");
 
  config_get( GNBSParams,sizeof(GNBSParams)/sizeof(paramdef_t),NULL); 
  RC.nb_nr_inst = GNBSParams[GNB_ACTIVE_GNBS_IDX].numelt;
 

	// Get num MACRLC instances
  config_getlist( &MACRLCParamList,NULL,0, NULL);
  RC.nb_macrlc_inst  = MACRLCParamList.numelt;
  // Get num L1 instances
  config_getlist( &L1ParamList,NULL,0, NULL);
  RC.nb_nr_L1_inst = L1ParamList.numelt;
  
  // Get num RU instances
  config_getlist( &RUParamList,NULL,0, NULL);  
  RC.nb_RU     = RUParamList.numelt; 
  
  RCconfig_nr_parallel();
    

}
