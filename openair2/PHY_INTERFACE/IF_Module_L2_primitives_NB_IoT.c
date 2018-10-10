#include "IF_Module_L2_primitives_NB_IoT.h"
#include "LAYER2/MAC/proto_NB_IoT.h"
#include "LAYER2/MAC/extern_NB_IoT.h"


// Sched_INFO as a input for the scheduler
void UL_indication_NB_IoT(UL_IND_NB_IoT_t *UL_INFO)
{
    int i=0;
    uint32_t abs_subframe;
    Sched_Rsp_NB_IoT_t *SCHED_info = &mac_inst->Sched_INFO;;
    //UE_TEMPLATE_NB_IoT *UE_info;

    /* Disable uplink RX function for now

      //If there is a preamble, do the initiate RA procedure
      if(UL_INFO->NRACH.number_of_initial_scs_detected>0)
        {
          for(i=0;i<UL_INFO->NRACH.number_of_initial_scs_detected;i++)
            {
              // initiate_ra here, some useful inforamtion : 
              //(UL_INFO->NRACH.nrach_pdu_list+i)->nrach_indication_rel13.initial_sc
              //(UL_INFO->NRACH.nrach_pdu_list+i)->nrach_indication_rel13.timing_advance
              init_RA_NB_IoT(mac_inst,
                             (UL_INFO->NRACH.nrach_pdu_list+i)->nrach_indication_rel13.initial_sc,
                             (UL_INFO->NRACH.nrach_pdu_list+i)->nrach_indication_rel13.nrach_ce_level,
                             UL_INFO->frame,
                             //timing_offset = Timing_advance * 16
                             (UL_INFO->NRACH.nrach_pdu_list+i)->nrach_indication_rel13.timing_advance*16
                             );


            }
        }

        // crc indication if there is error for this round UL transmission

        if(UL_INFO->crc_ind.number_of_crcs>0)
        {
          for(i=0;i<UL_INFO->crc_ind.number_of_crcs;i++)
          {
            if((UL_INFO->crc_ind.crc_pdu_list+i)->crc_indication_rel8.crc_flag == 0)
            {
              //unsuccessfully received this UE PDU
              //UE_info = get_ue_from_rnti(mac_inst,((UL_INFO->crc_ind.crc_pdu_list)+i)->rx_ue_information.rnti);
              //UE_info->HARQ_round++;
            }
          }
        }

        //If there is a Uplink SDU which needs to send to MAC

        if(UL_INFO->RX_NPUSCH.number_of_pdus>0)
          {
            for(i=0;i<UL_INFO->RX_NPUSCH.number_of_pdus;i++)
              {
                //For MSG3, Normal Uplink Data, NAK
                rx_sdu_NB_IoT(UL_INFO->module_id,
                              UL_INFO->CC_id,
                              UL_INFO->frame,
                              UL_INFO->subframe,
                              (UL_INFO->RX_NPUSCH.rx_pdu_list+i)->rx_ue_information.rnti,
                              (UL_INFO->RX_NPUSCH.rx_pdu_list+i)->data,
                              (UL_INFO->RX_NPUSCH.rx_pdu_list+i)->rx_indication_rel8.length
                             );

              }

          }

          */
    //LOG_I(MAC,"IF L2 frame: %d ,subframe: %d \n",UL_INFO->frame,UL_INFO->subframe);
    //abs_subframe = UL_INFO->frame*10240+UL_INFO->frame*10+UL_INFO->subframe +4;
    abs_subframe = UL_INFO->frame*10+UL_INFO->subframe +4;

    //LOG_I(MAC,"Enter scheduler in subframe %d\n",abs_subframe);
    //scheduler here
    //Schedule subframe should be next four subframe, means that UL_INFO->frame*10+UL_INFO->subframe + 4
    
    eNB_dlsch_ulsch_scheduler_NB_IoT(mac_inst,abs_subframe);
    mac_inst->if_inst_NB_IoT->schedule_response(&mac_inst->Sched_INFO);

    /*
    free(SCHED_info->TX_req->tx_request_body.tx_pdu_list);
    free(SCHED_info->HI_DCI0_req->hi_dci0_request_body.hi_dci0_pdu_list);
    free(SCHED_info->DL_req->dl_config_request_body.dl_config_pdu_list);
    free(SCHED_info->UL_req->ul_config_request_body.ul_config_pdu_list);
    
    free(SCHED_info->TX_req);
    free(SCHED_info->HI_DCI0_req);
    free(SCHED_info->DL_req);
    free(SCHED_info->UL_req);
    */
}