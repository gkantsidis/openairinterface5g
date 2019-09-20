/*
 * Copyright 2017 Cisco Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef _FAPI_NR_UE_INTERFACE_H_
#define _FAPI_NR_UE_INTERFACE_H_

#include "stddef.h"
#include "platform_types.h"
#include "fapi_nr_ue_constants.h"
#include "PHY/impl_defs_nr.h"

/*
  typedef unsigned int	   uint32_t;
  typedef unsigned short	   uint16_t;
  typedef unsigned char	   uint8_t;
  typedef signed int		   int32_t;
  typedef signed short	   int16_t;
  typedef signed char		   int8_t;
*/

typedef struct {
  uint8_t identifier_dci_formats          ; // 0  IDENTIFIER_DCI_FORMATS:
  uint8_t carrier_ind                     ; // 1  CARRIER_IND: 0 or 3 bits, as defined in Subclause x.x of [5, TS38.213]
  uint8_t sul_ind_0_1                     ; // 2  SUL_IND_0_1:
  uint8_t slot_format_ind                 ; // 3  SLOT_FORMAT_IND: size of DCI format 2_0 is configurable by higher layers up to 128 bits, according to Subclause 11.1.1 of [5, TS 38.213]
  uint8_t pre_emption_ind                 ; // 4  PRE_EMPTION_IND: size of DCI format 2_1 is configurable by higher layers up to 126 bits, according to Subclause 11.2 of [5, TS 38.213]. Each pre-emption indication is 14 bits
  uint8_t block_number                    ; // 5  BLOCK_NUMBER: starting position of a block is determined by the parameter startingBitOfFormat2_3
  uint8_t close_loop_ind                  ; // 6  CLOSE_LOOP_IND:
  uint8_t bandwidth_part_ind              ; // 7  BANDWIDTH_PART_IND:
  uint8_t short_message_ind               ; // 8  SHORT_MESSAGE_IND:
  uint8_t short_messages                  ; // 9  SHORT_MESSAGES:
  uint16_t freq_dom_resource_assignment_UL; // 10 FREQ_DOM_RESOURCE_ASSIGNMENT_UL: PUSCH hopping with resource allocation type 1 not considered
  //    (NOTE 1) If DCI format 0_0 is monitored in common search space
  //    and if the number of information bits in the DCI format 0_0 prior to padding
  //    is larger than the payload size of the DCI format 1_0 monitored in common search space
  //    the bitwidth of the frequency domain resource allocation field in the DCI format 0_0
  //    is reduced such that the size of DCI format 0_0 equals to the size of the DCI format 1_0
  uint16_t freq_dom_resource_assignment_DL; // 11 FREQ_DOM_RESOURCE_ASSIGNMENT_DL:
  uint8_t time_dom_resource_assignment    ; // 12 TIME_DOM_RESOURCE_ASSIGNMENT: 0, 1, 2, 3, or 4 bits as defined in Subclause 6.1.2.1 of [6, TS 38.214]. The bitwidth for this field is determined as log2(I) bits,
  //    where I the number of entries in the higher layer parameter pusch-AllocationList
  uint8_t vrb_to_prb_mapping              ; // 13 VRB_TO_PRB_MAPPING: 0 bit if only resource allocation type 0
  uint8_t prb_bundling_size_ind           ; // 14 PRB_BUNDLING_SIZE_IND:0 bit if the higher layer parameter PRB_bundling is not configured or is set to 'static', or 1 bit if the higher layer parameter PRB_bundling is set to 'dynamic' according to Subclause 5.1.2.3 of [6, TS 38.214]
  uint8_t rate_matching_ind               ; // 15 RATE_MATCHING_IND: 0, 1, or 2 bits according to higher layer parameter rate-match-PDSCH-resource-set
  uint8_t zp_csi_rs_trigger               ; // 16 ZP_CSI_RS_TRIGGER:
  uint8_t freq_hopping_flag               ; // 17 FREQ_HOPPING_FLAG: 0 bit if only resource allocation type 0
  uint8_t tb1_mcs                         ; // 18 TB1_MCS:
  uint8_t tb1_ndi                         ; // 19 TB1_NDI:
  uint8_t tb1_rv                          ; // 20 TB1_RV:
  uint8_t tb2_mcs                         ; // 21 TB2_MCS:
  uint8_t tb2_ndi                         ; // 22 TB2_NDI:
  uint8_t tb2_rv                          ; // 23 TB2_RV:
  uint8_t mcs                             ; // 24 MCS:
  uint8_t ndi                             ; // 25 NDI:
  uint8_t rv                              ; // 26 RV:
  uint8_t harq_process_number             ; // 27 HARQ_PROCESS_NUMBER:
  uint8_t dai                             ; // 28 DAI: For format1_1: 4 if more than one serving cell are configured in the DL and the higher layer parameter HARQ-ACK-codebook=dynamic, where the 2 MSB bits are the counter DAI and the 2 LSB bits are the total DAI
  //    2 if one serving cell is configured in the DL and the higher layer parameter HARQ-ACK-codebook=dynamic, where the 2 bits are the counter DAI
  //    0 otherwise
  uint8_t first_dai                       ; // 29 FIRST_DAI: (1 or 2 bits) 1 bit for semi-static HARQ-ACK
  uint8_t second_dai                      ; // 30 SECOND_DAI: (0 or 2 bits) 2 bits for dynamic HARQ-ACK codebook with two HARQ-ACK sub-codebooks
  uint8_t tb_scaling                      ; // 31 TB_SCALING:
  uint8_t tpc_pusch                       ; // 32 TPC_PUSCH:
  uint8_t tpc_pucch                       ; // 33 TPC_PUCCH:
  uint8_t pucch_resource_ind              ; // 34 PUCCH_RESOURCE_IND:
  uint8_t pdsch_to_harq_feedback_time_ind ; // 35 PDSCH_TO_HARQ_FEEDBACK_TIME_IND:
  uint8_t srs_resource_ind                ; // 36 SRS_RESOURCE_IND:
  uint8_t precod_nbr_layers               ; // 37 PRECOD_NBR_LAYERS:
  uint8_t antenna_ports                   ; // 38 ANTENNA_PORTS:
  uint8_t tci                             ; // 39 TCI: 0 bit if higher layer parameter tci-PresentInDCI is not enabled; otherwise 3 bits
  uint8_t srs_request                     ; // 40 SRS_REQUEST:
  uint8_t tpc_cmd                         ; // 41 TPC_CMD:
  uint8_t csi_request                     ; // 42 CSI_REQUEST:
  uint8_t cbgti                           ; // 43 CBGTI: 0, 2, 4, 6, or 8 bits determined by higher layer parameter maxCodeBlockGroupsPerTransportBlock for the PDSCH
  uint8_t cbgfi                           ; // 44 CBGFI: 0 or 1 bit determined by higher layer parameter codeBlockGroupFlushIndicator
  uint8_t ptrs_dmrs                       ; // 45 PTRS_DMRS:
  uint8_t beta_offset_ind                 ; // 46 BETA_OFFSET_IND:
  uint8_t dmrs_seq_ini                    ; // 47 DMRS_SEQ_INI: 1 bit if the cell has two ULs and the number of bits for DCI format 1_0 before padding
  //    is larger than the number of bits for DCI format 0_0 before padding; 0 bit otherwise
  uint8_t ul_sch_ind                      ; // 48 UL_SCH_IND:  value of "1" indicates UL-SCH shall be transmitted on the PUSCH and a value of "0" indicates UL-SCH shall not be transmitted on the PUSCH
  uint16_t padding_nr_dci                 ; // 49 PADDING_NR_DCI: (Note 2) If DCI format 0_0 is monitored in common search space
  //    and if the number of information bits in the DCI format 0_0 prior to padding
  //    is less than the payload size of the DCI format 1_0 monitored in common search space
  //    zeros shall be appended to the DCI format 0_0
  //    until the payload size equals that of the DCI format 1_0
  uint8_t sul_ind_0_0                     ; // 50 SUL_IND_0_0:
  uint8_t ra_preamble_index               ; // 51 RA_PREAMBLE_INDEX:
  uint8_t sul_ind_1_0                     ; // 52 SUL_IND_1_0:
  uint8_t ss_pbch_index                   ; // 53 SS_PBCH_INDEX
  uint8_t prach_mask_index                ; // 54 PRACH_MASK_INDEX
  uint8_t reserved_nr_dci                 ; // 55 RESERVED_NR_DCI
} fapi_nr_dci_pdu_rel15_t;



typedef struct {
  uint8_t uci_format;
  uint8_t uci_channel;
  uint8_t harq_ack_bits;
  uint32_t harq_ack;
  uint8_t csi_bits;
  uint32_t csi;
  uint8_t sr_bits;
  uint32_t sr;
} fapi_nr_uci_pdu_rel15_t;

    

typedef struct {
  /// frequency_domain_resource;
  //uint32_t rb_start;
  //uint32_t rb_end;
  uint64_t frequency_domain_resource;
  uint16_t rb_offset;

  uint8_t duration;
  uint8_t cce_reg_mapping_type;                   //  interleaved or noninterleaved
  uint8_t cce_reg_interleaved_reg_bundle_size;    //  valid if CCE to REG mapping type is interleaved type
  uint8_t cce_reg_interleaved_interleaver_size;   //  valid if CCE to REG mapping type is interleaved type
  uint8_t cce_reg_interleaved_shift_index;        //  valid if CCE to REG mapping type is interleaved type
  uint8_t precoder_granularity;
  uint16_t pdcch_dmrs_scrambling_id;

  uint8_t tci_state_pdcch;
  uint8_t tci_present_in_dci;
} fapi_nr_coreset_t;

//
// Top level FAPI messages
//



//
// P7
//

typedef struct {
  uint16_t rnti;
  uint8_t dci_format;
  // n_CCE index of first CCE for PDCCH reception
  int n_CCE;
  // N_CCE is L, or number of CCEs for DCI
  int N_CCE;
  fapi_nr_dci_pdu_rel15_t dci;
} fapi_nr_dci_indication_pdu_t;


///
typedef struct {
  uint32_t sfn_slot;
  uint16_t number_of_dcis;
  fapi_nr_dci_indication_pdu_t dci_list[10];
} fapi_nr_dci_indication_t;


typedef struct {
  uint32_t pdu_length;
  uint8_t* pdu;
} fapi_nr_pdsch_pdu_t;

typedef struct {
  uint8_t* pdu;   //  3bytes
  uint8_t additional_bits;
  uint8_t ssb_index;
  uint8_t ssb_length;
  uint16_t cell_id;

} fapi_nr_mib_pdu_t;

typedef struct {
  uint32_t pdu_length;
  uint8_t* pdu;
  uint32_t sibs_mask;
} fapi_nr_sib_pdu_t;

typedef struct {
  uint8_t pdu_type;
  union {
    fapi_nr_pdsch_pdu_t pdsch_pdu;
    fapi_nr_mib_pdu_t mib_pdu;
    fapi_nr_sib_pdu_t sib_pdu;
  };
} fapi_nr_rx_indication_body_t;

///
#define FAPI_RX_IND_MAX_PDU 100
typedef struct {
  uint32_t sfn_slot;
  uint16_t number_pdus;
  fapi_nr_rx_indication_body_t rx_indication_body[FAPI_RX_IND_MAX_PDU];
} fapi_nr_rx_indication_t;

typedef struct {
  uint8_t ul_cqi;
  uint16_t timing_advance;
  uint16_t rnti;
} fapi_nr_tx_config_t;

typedef struct {
  uint16_t pdu_length;
  uint16_t pdu_index;
  uint8_t* pdu;
} fapi_nr_tx_request_body_t;

///
typedef struct {
  uint32_t sfn_slot;
  fapi_nr_tx_config_t tx_config;
  uint16_t number_of_pdus;
  fapi_nr_tx_request_body_t *tx_request_body;
} fapi_nr_tx_request_t;

typedef struct {
  uint8_t preamble_index;
  uint8_t prach_configuration_index;
  uint16_t preamble_length;
  uint8_t power_ramping_step;
  uint16_t preamble_received_target_power;
  uint8_t msg1_fdm;
  uint8_t msg1_frequency_start;
  uint8_t zero_correlation_zone_config;
  uint8_t subcarrier_spacing;
  uint8_t restrictedset_config;
  uint16_t root_sequence_index;
  uint16_t rsrp_threshold_ssb;
  uint16_t rsrp_threshold_sul;
  uint16_t prach_freq_offset;
} fapi_nr_ul_config_prach_pdu;

typedef struct {

        pucch_format_nr_t      format;              /* format   0    1    2    3    4    */
        uint8_t                initialCyclicShift;  /*          x    x                   */
        uint8_t                nrofSymbols;         /*          x    x    x    x    x    */
        uint8_t                startingSymbolIndex; /*          x    x    x    x    x    */
        uint8_t                timeDomainOCC;       /*               x                   */
        uint8_t                nrofPRBs;            /*                    x    x         */
        uint16_t               startingPRB;         /*                                     maxNrofPhysicalResourceBlocks  = 275 */
        uint8_t                occ_length;          /*                              x    */
        uint8_t                occ_Index;           /*                              x    */

        feature_status_t       intraSlotFrequencyHopping;
        uint16_t               secondHopPRB;

        /*
         -- Enabling inter-slot frequency hopping when PUCCH Format 1, 3 or 4 is repeated over multiple slots.
         -- The field is not applicable for format 2.
         */
        feature_status_t       interslotFrequencyHopping;
        /*
            -- Enabling 2 DMRS symbols per hop of a PUCCH Format 3 or 4 if both hops are more than X symbols when FH is enabled (X=4).
            -- Enabling 4 DMRS sybmols for a PUCCH Format 3 or 4 with more than 2X+1 symbols when FH is disabled (X=4).
            -- Corresponds to L1 parameter 'PUCCH-F3-F4-additional-DMRS' (see 38.213, section 9.2.1)
            -- The field is not applicable for format 1 and 2.
        */
        enable_feature_t       additionalDMRS;
        /*
            -- Max coding rate to determine how to feedback UCI on PUCCH for format 2, 3 or 4
            -- Corresponds to L1 parameter 'PUCCH-F2-maximum-coderate', 'PUCCH-F3-maximum-coderate' and 'PUCCH-F4-maximum-coderate'
            -- (see 38.213, section 9.2.5)
            -- The field is not applicable for format 1.
         */
        PUCCH_MaxCodeRate_t    maxCodeRate;
        /*
            -- Number of slots with the same PUCCH F1, F3 or F4. When the field is absent the UE applies the value n1.
            -- Corresponds to L1 parameter 'PUCCH-F1-number-of-slots', 'PUCCH-F3-number-of-slots' and 'PUCCH-F4-number-of-slots'
            -- (see 38.213, section 9.2.6)
            -- The field is not applicable for format 2.
         */
        uint8_t                nrofSlots;
        /*
            -- Enabling pi/2 BPSK for UCI symbols instead of QPSK for PUCCH.
            -- Corresponds to L1 parameter 'PUCCH-PF3-PF4-pi/2PBSK' (see 38.213, section 9.2.5)
            -- The field is not applicable for format 1 and 2.
         */
        feature_status_t       pi2PBSK;
        /*
            -- Enabling simultaneous transmission of CSI and HARQ-ACK feedback with or without SR with PUCCH Format 2, 3 or 4
            -- Corresponds to L1 parameter 'PUCCH-F2-Simultaneous-HARQ-ACK-CSI', 'PUCCH-F3-Simultaneous-HARQ-ACK-CSI' and
            -- 'PUCCH-F4-Simultaneous-HARQ-ACK-CSI' (see 38.213, section 9.2.5)
            -- When the field is absent the UE applies the value OFF
            -- The field is not applicable for format 1.
         */
        enable_feature_t       simultaneousHARQ_ACK_CSI;
        /*
              -- Configuration of group- and sequence hopping for all the PUCCH formats 0, 1, 3 and 4. "neither" implies neither group
              -- or sequence hopping is enabled. "enable" enables group hopping and disables sequence hopping. "disable"” disables group
              -- hopping and enables sequence hopping. Corresponds to L1 parameter 'PUCCH-GroupHopping' (see 38.211, section 6.4.1.3)
              pucch-GroupHopping            ENUMERATED { neither, enable, disable },
         */
        pucch_GroupHopping_t   pucch_GroupHopping;
        /*
              -- Cell-Specific scrambling ID for group hoppping and sequence hopping if enabled.
              -- Corresponds to L1 parameter 'HoppingID' (see 38.211, section 6.3.2.2)
              hoppingId               BIT STRING (SIZE (10))                              OPTIONAL,   -- Need R
         */
        uint16_t               hoppingId;
        /*
              -- Power control parameter P0 for PUCCH transmissions. Value in dBm. Only even values (step size 2) allowed.
              -- Corresponds to L1 parameter 'p0-nominal-pucch' (see 38.213, section 7.2)
              p0-nominal                INTEGER (-202..24)                                OPTIONAL,   -- Need R
         */
        int8_t                 p0_nominal;

        int8_t                 deltaF_PUCCH_f[NUMBER_PUCCH_FORMAT_NR];
        uint8_t                p0_PUCCH_Id;     /* INTEGER (1..8)     */
        int8_t                 p0_PUCCH_Value;
        // pathlossReferenceRSs        SEQUENCE (SIZE (1..maxNrofPUCCH-PathlossReferenceRSs)) OF PUCCH-PathlossReferenceRS OPTIONAL, -- Need M
        int8_t                 twoPUCCH_PC_AdjustmentStates;

    } fapi_nr_ul_config_uci_pdu;

    typedef enum {pusch_freq_hopping_disabled = 0 , pusch_freq_hopping_enabled = 1}pusch_freq_hopping_t;
    typedef struct{
    uint8_t aperiodicSRS_ResourceTrigger;
    } fapi_nr_ul_srs_config_t;
    typedef struct {
        uint8_t bandwidth_part_ind;
        uint16_t number_rbs;
        uint16_t start_rb;
        uint8_t frame_offset;
        uint16_t number_symbols;
        uint16_t start_symbol;
        pusch_freq_hopping_t pusch_freq_hopping;
        uint8_t mcs;
        uint8_t ndi;
        uint8_t rv;
        uint8_t harq_process_nbr;
        int8_t accumulated_delta_PUSCH;
        int8_t absolute_delta_PUSCH;
        uint8_t n_layers;
        uint8_t tpmi;
        uint8_t n_dmrs_cdm_groups;
        uint8_t dmrs_ports[4];
        uint8_t n_front_load_symb;
        fapi_nr_ul_srs_config_t srs_config;
        uint8_t csi_reportTriggerSize;
        uint8_t maxCodeBlockGroupsPerTransportBlock;
        uint8_t ptrs_dmrs_association_port;
        uint8_t beta_offset_ind;
    } fapi_nr_ul_config_ulsch_pdu_rel15_t;

typedef struct {
  uint16_t rnti;
  fapi_nr_ul_config_ulsch_pdu_rel15_t ulsch_pdu_rel15;
} fapi_nr_ul_config_ulsch_pdu;

typedef struct {

} fapi_nr_ul_config_srs_pdu;

// nFAPI enums
typedef enum {
  FAPI_NR_DL_CONFIG_DCI_DL_PDU_TYPE = 0,
  FAPI_NR_DL_CONFIG_BCH_PDU_TYPE,
  FAPI_NR_DL_CONFIG_DLSCH_PDU_TYPE,
  FAPI_NR_DL_CONFIG_PCH_PDU_TYPE,
} fapi_nr_dl_config_pdu_type_e;

// nFAPI enums
typedef enum {
  FAPI_NR_UL_CONFIG_PRACH_PDU_TYPE = 0,
  FAPI_NR_UL_CONFIG_ULSCH_PDU_TYPE,
  FAPI_NR_UL_CONFIG_UCI_PDU_TYPE,
  FAPI_NR_UL_CONFIG_SRS_PDU_TYPE,
} fapi_nr_ul_config_pdu_type_e;

typedef struct {
  uint8_t pdu_type;
  union {
    fapi_nr_ul_config_prach_pdu prach_config_pdu;
    fapi_nr_ul_config_uci_pdu uci_config_pdu;
    fapi_nr_ul_config_ulsch_pdu ulsch_config_pdu;
    fapi_nr_ul_config_srs_pdu srs_config_pdu;
  };
} fapi_nr_ul_config_request_pdu_t;

typedef struct {
  uint32_t sfn_slot;
  uint8_t number_pdus;
  fapi_nr_ul_config_request_pdu_t ul_config_list[FAPI_NR_UL_CONFIG_LIST_NUM];
} fapi_nr_ul_config_request_t;


typedef struct {
  uint16_t rnti;
  
  fapi_nr_coreset_t coreset;
  uint16_t N_RB_BWP;
  uint32_t duration;
  uint8_t number_of_candidates[5];    //  aggregation level 1, 2, 4, 8, 16 
  uint16_t monitoring_symbols_within_slot;
  //  DCI foramt-specific
  uint8_t format_2_0_number_of_candidates[5];    //  aggregation level 1, 2, 4, 8, 16
  uint8_t format_2_3_monitorying_periodicity;
  uint8_t format_2_3_number_of_candidates;
} fapi_nr_dl_config_dci_dl_pdu_rel15_t;

typedef struct {
  fapi_nr_dl_config_dci_dl_pdu_rel15_t dci_config_rel15;
} fapi_nr_dl_config_dci_pdu;
typedef struct{
  uint8_t aperiodicSRS_ResourceTrigger;
} fapi_nr_dl_srs_config_t;

typedef enum{vrb_to_prb_mapping_non_interleaved = 0, vrb_to_prb_mapping_interleaved = 1} vrb_to_prb_mapping_t;
//typedef fapi_nr_dci_pdu_rel15_t fapi_nr_dl_config_dlsch_pdu_rel15_t;
typedef struct {
  uint8_t bandwidth_part_ind;
  uint16_t number_rbs;
  uint16_t start_rb;
  uint8_t frame_offset;
  uint16_t number_symbols;
  uint16_t start_symbol;
  uint8_t prb_bundling_size_ind;
  uint8_t rate_matching_ind;
  uint8_t zp_csi_rs_trigger;
  uint8_t mcs;
  uint8_t ndi;
  uint8_t rv;
  uint8_t tb2_mcs;
  uint8_t tb2_ndi;
  uint8_t tb2_rv;
  uint8_t harq_process_nbr;
  vrb_to_prb_mapping_t vrb_to_prb_mapping;
  uint8_t dai;
  double scaling_factor_S;
  int8_t accumulated_delta_PUCCH;
  uint8_t pucch_resource_id;
  uint8_t pdsch_to_harq_feedback_time_ind;
  uint8_t n_dmrs_cdm_groups;
  uint8_t dmrs_ports[10];
  uint8_t n_front_load_symb;
  uint8_t tci_state;
  fapi_nr_dl_srs_config_t srs_config;
  uint8_t cbgti;
  uint8_t codeBlockGroupFlushIndicator;
  //  to be check the fields needed to L1 with NR_DL_UE_HARQ_t and NR_UE_DLSCH_t
} fapi_nr_dl_config_dlsch_pdu_rel15_t;

typedef struct {

  uint8_t subcarrier_spacing_common;
  uint8_t ssb_subcarrier_offset;
  uint8_t dmrs_type_a_position;
  uint8_t pdcch_config_sib1;
  uint8_t cell_barred;
  uint8_t intra_frequency_reselection;

  uint16_t system_frame_number;
  uint8_t ssb_index;
  uint8_t half_frame_bit;
} fapi_nr_dl_config_bch_pdu;

typedef struct {
  uint16_t rnti;
  fapi_nr_dl_config_dlsch_pdu_rel15_t dlsch_config_rel15;
} fapi_nr_dl_config_dlsch_pdu;

typedef struct {
  uint8_t pdu_type;
  union {
    fapi_nr_dl_config_bch_pdu bch_config_pdu;
    fapi_nr_dl_config_dci_pdu dci_config_pdu;
    fapi_nr_dl_config_dlsch_pdu dlsch_config_pdu;
  };
} fapi_nr_dl_config_request_pdu_t;

typedef struct {
  uint32_t sfn_slot;
  uint8_t number_pdus;
  fapi_nr_dl_config_request_pdu_t dl_config_list[FAPI_NR_DL_CONFIG_LIST_NUM];
} fapi_nr_dl_config_request_t;


//
// P5
//

    
typedef struct {
  uint8_t  numerology_index_mu;
  uint8_t duplex_mode;
  uint8_t  dl_cyclic_prefix_type;
  uint8_t  ul_cyclic_prefix_type;
} fapi_nr_subframe_config_t;

typedef struct {
  uint16_t  dl_carrier_bandwidth;
  uint16_t  ul_carrier_bandwidth;
  uint16_t  dl_absolutefrequencypointA;
  uint16_t  ul_absolutefrequencypointA;
  uint16_t  dl_offsettocarrier;
  uint16_t  ul_offsettocarrier;
  uint16_t  dl_subcarrierspacing;
  uint16_t  ul_subcarrierspacing;
  uint16_t  dl_specificcarrier_k0;
  uint16_t  ul_specificcarrier_k0;
  uint16_t  NIA_subcarrierspacing;
} fapi_nr_rf_config_t;

typedef struct {
  uint16_t  physical_cell_id;
  uint8_t   half_frame_index;
  uint16_t  ssb_subcarrier_offset;
  uint16_t  ssb_sib1_position_in_burst; // in sib1
  uint64_t  ssb_scg_position_in_burst;  // in servingcellconfigcommon 
  uint8_t  ssb_periodicity;
  uint16_t  ss_pbch_block_power;
  uint16_t  n_ssb_crb;
} fapi_nr_sch_config_t;

typedef struct {
  uint16_t  dl_bandwidth;
  uint16_t  ul_bandwidth;
  uint16_t  dl_offset;
  uint16_t  ul_offset;
  uint8_t   dl_subcarrierSpacing;
  uint8_t   ul_subcarrierSpacing;
} fapi_nr_initialBWP_config_t;

#define FAPI_NR_PDSCH_CONFIG_MAXALLOCATIONS 16
#define FAPI_NR_PUSCH_CONFIG_MAXALLOCATIONS 16

typedef struct {
  uint16_t  dmrs_TypeA_Position;
  uint16_t  num_PDSCHTimeDomainResourceAllocations;
  uint16_t  PDSCHTimeDomainResourceAllocation_k0[FAPI_NR_PDSCH_CONFIG_MAXALLOCATIONS];         
  uint16_t  PDSCHTimeDomainResourceAllocation_mappingType[FAPI_NR_PDSCH_CONFIG_MAXALLOCATIONS]; 
  uint16_t  PDSCHTimeDomainResourceAllocation_startSymbolAndLength[FAPI_NR_PDSCH_CONFIG_MAXALLOCATIONS];
} fapi_nr_pdsch_config_t;

typedef struct {
  uint16_t  prach_RootSequenceIndex;                                        ///// L1 parameter 'PRACHRootSequenceIndex'
  uint16_t  prach_msg1_SubcarrierSpacing;                                   ///// L1 parameter 'prach-Msg1SubcarrierSpacing'
  uint16_t  restrictedSetConfig;
  uint16_t  msg3_transformPrecoding;                                        ///// L1 parameter 'msg3-tp'
  uint16_t  ssb_perRACH_OccasionAndCB_PreamblesPerSSB;
  uint16_t  ra_ContentionResolutionTimer;
  uint16_t  rsrp_ThresholdSSB;
  /////////////////--------------------NR RACH-ConfigGeneric--------------------/////////////////
  uint16_t  prach_ConfigurationIndex;                                       ///// L1 parameter 'PRACHConfigurationIndex'
  uint16_t  prach_msg1_FDM;                                                 ///// L1 parameter 'prach-FDM'
  uint16_t  prach_msg1_FrequencyStart;                                      ///// L1 parameter 'prach-frequency-start'
  uint16_t  zeroCorrelationZoneConfig;
  uint16_t  preambleReceivedTargetPower;
  uint16_t  preambleTransMax;
  uint16_t  powerRampingStep;
  uint16_t  ra_ResponseWindow;
} fapi_nr_rach_config_t;

typedef struct {
  uint16_t  groupHoppingEnabledTransformPrecoding;                          ///// L1 parameter 'Group-hopping-enabled-Transform-precoding'
  uint16_t  msg3_DeltaPreamble;                                             ///// L1 parameter 'Delta-preamble-msg3' 
  uint16_t  p0_NominalWithGrant;                                            ///// L1 parameter 'p0-nominal-pusch-withgrant'
  uint16_t  dmrs_TypeA_Position;
  uint16_t  num_PUSCHTimeDomainResourceAllocations;
  uint16_t  PUSCHTimeDomainResourceAllocation_k2[FAPI_NR_PUSCH_CONFIG_MAXALLOCATIONS];                                ///// L1 parameter 'K2' 
  uint16_t  PUSCHTimeDomainResourceAllocation_mappingType[FAPI_NR_PUSCH_CONFIG_MAXALLOCATIONS];                       ///// L1 parameter 'Mapping-type'
  uint16_t  PUSCHTimeDomainResourceAllocation_startSymbolAndLength[FAPI_NR_PUSCH_CONFIG_MAXALLOCATIONS];
} fapi_nr_pusch_config_t;

typedef struct {
  uint8_t pucch_resource_common;
  uint16_t  pucch_GroupHopping;                                             ///// L1 parameter 'PUCCH-GroupHopping' 
  uint8_t hopping_id;
  uint16_t  p0_nominal;                                                     ///// L1 parameter 'p0-nominal-pucch'
} fapi_nr_pucch_config_t;

typedef struct {
  uint8_t  controlResourceSetZero;
  uint8_t  searchSpaceZero;
  
  //  fapi_nr_SearchSpace_t           sib1searchSpace;
  //  fapi_nr_SearchSpace_t           sibssearchSpace; 
  //  fapi_nr_SearchSpace_t           ra_SearchSpace;
} fapi_nr_pdcch_config_t;

typedef struct {
//NR TDD-UL-DL-ConfigCommon                ///// L1 parameter 'UL-DL-configuration-common'
  uint16_t  referenceSubcarrierSpacing;                                     ///// L1 parameter 'reference-SCS'
  uint16_t  dl_ul_periodicity;                                  ///// L1 parameter 'DL-UL-transmission-periodicity'
  uint16_t  nrofDownlinkSlots;                                              ///// L1 parameter 'number-of-DL-slots'
  uint16_t  nrofDownlinkSymbols;                                            ///// L1 parameter 'number-of-DL-symbols-common'
  uint16_t  nrofUplinkSlots;                                                ///// L1 parameter 'number-of-UL-slots'
  uint16_t  nrofUplinkSymbols;                                              ///// L1 parameter 'number-of-UL-symbols-common'
  uint16_t  Pattern2Present;
  uint16_t  Pattern2_dl_ul_periodicity;                                  ///// L1 parameter 'DL-UL-transmission-periodicity'
  uint16_t  Pattern2_nrofDownlinkSlots;                                              ///// L1 parameter 'number-of-DL-slots'
  uint16_t  Pattern2_nrofDownlinkSymbols;                                            ///// L1 parameter 'number-of-DL-symbols-common'
  uint16_t  Pattern2_nrofUplinkSlots;                                                ///// L1 parameter 'number-of-UL-slots'
  uint16_t  Pattern2_nrofUplinkSymbols;                                              ///// L1 parameter 'number-of-UL-symbols-common'
} fapi_nr_tdd_ul_dl_config_t;

#define FAPI_MAX_NUM_RF_BANDS 16
typedef struct {
        uint16_t number_rf_bands;
        uint16_t rf_band[FAPI_MAX_NUM_RF_BANDS];
} fapi_rf_bands_t;

typedef struct
{
  fapi_rf_bands_t rf_bands;

  uint32_t nrarfcn;

  // nfapi_nmm_frequency_bands_t nmm_gsm_frequency_bands;
  // nfapi_nmm_frequency_bands_t nmm_umts_frequency_bands;
  // nfapi_nmm_frequency_bands_t nmm_lte_frequency_bands;
  // nfapi_uint8_tlv_t nmm_uplink_rssi_supported;

} fapi_nr_fapi_t;

typedef struct {
  fapi_nr_fapi_t                           fapi_config;
  fapi_nr_subframe_config_t                subframe_config;
  fapi_nr_rf_config_t                      rf_config;
  fapi_nr_sch_config_t                     sch_config;
  fapi_nr_initialBWP_config_t              initialBWP_config;
  fapi_nr_pdsch_config_t                   pdsch_config;
  fapi_nr_rach_config_t                    rach_config;
  fapi_nr_pusch_config_t                   pusch_config;
  fapi_nr_pucch_config_t                   pucch_config;
  fapi_nr_pdcch_config_t                   pdcch_config;
  fapi_nr_tdd_ul_dl_config_t               tdd_ul_dl_config;
  //  fapi_nr_ratematchpattern_t               ratematchpattern;
  //  fapi_nr_ratematchpattern_lte_crs_t       ratematchpattern_lte_crs;
} fapi_nr_config_request_t;

typedef struct {
  fapi_nr_coreset_t coreset;

  uint8_t monitoring_slot_peridicity;
  uint8_t monitoring_slot_offset;
  uint16_t duration;
  uint16_t monitoring_symbols_within_slot;
  uint8_t number_of_candidates[5];            //  aggregation level 1, 2, 4, 8, 16

  uint8_t dci_2_0_number_of_candidates[5];    //  aggregation level 1, 2, 4, 8, 16
  uint8_t dci_2_3_monitorying_periodicity;
  uint8_t dci_2_3_number_of_candidates;
        
} fapi_nr_search_space_t;

typedef struct {
  fapi_nr_search_space_t search_space_sib1;
  fapi_nr_search_space_t search_space_others_sib;
  fapi_nr_search_space_t search_space_paging;
  //fapi_nr_coreset_t      coreset_ra;         //  common coreset
  fapi_nr_search_space_t search_space_ra;    
} fapi_nr_pdcch_config_common_t;

typedef struct {
  uint8_t k0;
  uint8_t mapping_type;
  uint8_t symbol_starting;
  uint8_t symbol_length;
} fapi_nr_pdsch_time_domain_resource_allocation_t;

typedef struct {
  fapi_nr_pdsch_time_domain_resource_allocation_t allocation_list[FAPI_NR_MAX_NUM_DL_ALLOCATIONS];
} fapi_nr_pdsch_config_common_t;

typedef struct {
  uint8_t prach_configuration_index;
  uint8_t msg1_fdm;
  uint8_t msg1_frequency_start;
  uint8_t zero_correlation_zone_config;
  uint8_t preamble_received_target_power;
  uint8_t preamble_transmission_max;
  uint8_t power_ramping_step;
  uint8_t ra_window_size;

  uint8_t total_number_of_preamble;
  uint8_t ssb_occasion_per_rach;
  uint8_t cb_preamble_per_ssb;

  uint8_t group_a_msg3_size;
  uint8_t group_a_number_of_preamble;
  uint8_t group_b_power_offset;
  uint8_t contention_resolution_timer;
  uint8_t rsrp_threshold_ssb;
  uint8_t rsrp_threshold_ssb_sul;
  uint8_t prach_length;   //  l839, l139
  uint8_t prach_root_sequence_index;  //  0 - 837 for l839, 0 - 137 for l139
  uint8_t msg1_subcarrier_spacing;
  uint8_t restrictedset_config;
  uint8_t msg3_transform_precoding;
} fapi_nr_rach_config_common_t;

typedef struct {
  uint8_t k2;
  uint8_t mapping_type;
  uint8_t symbol_starting;
  uint8_t symbol_length;
} fapi_nr_pusch_time_domain_resource_allocation_t;
      
typedef struct {
  uint8_t group_hopping_enabled_transform_precoding;
  fapi_nr_pusch_time_domain_resource_allocation_t allocation_list[FAPI_NR_MAX_NUM_UL_ALLOCATIONS];
  uint8_t msg3_delta_preamble;
  uint8_t p0_nominal_with_grant;
} fapi_nr_pusch_config_common_t;

typedef struct {
  uint8_t pucch_resource_common;
  uint8_t pucch_group_hopping;
  uint8_t hopping_id;
  uint8_t p0_nominal;
} fapi_nr_pucch_config_common_t;


typedef struct {
        
  fapi_nr_pdcch_config_common_t pdcch_config_common;
  fapi_nr_pdsch_config_common_t pdsch_config_common;
        
} fapi_nr_dl_bwp_common_config_t;



typedef struct {
  uint16_t int_rnti;
  uint8_t time_frequency_set;
  uint8_t dci_payload_size;
  uint8_t serving_cell_id[FAPI_NR_MAX_NUM_SERVING_CELLS];    //  interrupt configuration per serving cell
  uint8_t position_in_dci[FAPI_NR_MAX_NUM_SERVING_CELLS];    //  interrupt configuration per serving cell
} fapi_nr_downlink_preemption_t;

typedef struct {
  uint8_t tpc_index;
  uint8_t tpc_index_sul;
  uint8_t target_cell;
} fapi_nr_pusch_tpc_command_config_t;

typedef struct {
  uint8_t tpc_index_pcell;
  uint8_t tpc_index_pucch_scell;
} fapi_nr_pucch_tpc_command_config_t;

typedef struct {
  uint8_t starting_bit_of_format_2_3;
  uint8_t feild_type_format_2_3;
} fapi_nr_srs_tpc_command_config_t;

typedef struct {
  fapi_nr_downlink_preemption_t downlink_preemption;
  fapi_nr_pusch_tpc_command_config_t tpc_pusch;
  fapi_nr_pucch_tpc_command_config_t tpc_pucch;
  fapi_nr_srs_tpc_command_config_t tpc_srs;
} fapi_nr_pdcch_config_dedicated_t;

typedef struct {
  uint8_t dmrs_type;
  uint8_t dmrs_addition_position;
  uint8_t max_length;
  uint16_t scrambling_id0;
  uint16_t scrambling_id1;
  uint8_t ptrs_frequency_density[2];      //  phase tracking rs
  uint8_t ptrs_time_density[3];           //  phase tracking rs
  uint8_t ptrs_epre_ratio;                //  phase tracking rs
  uint8_t ptrs_resource_element_offset;   //  phase tracking rs
} fapi_nr_dmrs_downlink_config_t;

typedef struct {
  uint8_t bwp_or_cell_level;
  uint8_t pattern_type;
  uint32_t resource_blocks[9];        //  bitmaps type 275 bits
  uint8_t slot_type;                  //  bitmaps type one/two slot(s)
  uint32_t symbols_in_resouece_block; //  bitmaps type 14/28 bits
  uint8_t periodic;                   //  bitmaps type 
  uint32_t pattern[2];                //  bitmaps type 2/4/5/8/10/20/40 bits

  fapi_nr_coreset_t coreset;         //  coreset

  uint8_t subcarrier_spacing;
  uint8_t mode;
} fapi_nr_rate_matching_pattern_group_t;

typedef struct {
  //  resource mapping
  uint8_t row;    //  row1/row2/row4/other
  uint16_t frequency_domain_allocation; //    4/12/3/6 bits
  uint8_t number_of_ports;
  uint8_t first_ofdm_symbol_in_time_domain;
  uint8_t first_ofdm_symbol_in_time_domain2;
  uint8_t cdm_type;
  uint8_t density;            //  .5/1/3
  uint8_t density_dot5_type;  //  even/odd PRBs
        
  uint8_t frequency_band_starting_rb;     //  freqBand
  uint8_t frequency_band_number_of_rb;    //  freqBand

  //  periodicityAndOffset
  uint8_t periodicity;    //  slot4/5/8/10/16/20/32/40/64/80/160/320/640
  uint32_t offset;        //  0..639 bits
} fapi_nr_zp_csi_rs_resource_t;

typedef struct {
  uint16_t data_scrambling_id_pdsch;
  fapi_nr_dmrs_downlink_config_t dmrs_dl_for_pdsch_mapping_type_a;
  fapi_nr_dmrs_downlink_config_t dmrs_dl_for_pdsch_mapping_type_b; 
  uint8_t vrb_to_prb_interleaver;
  uint8_t resource_allocation;
  fapi_nr_pdsch_time_domain_resource_allocation_t allocation_list[FAPI_NR_MAX_NUM_DL_ALLOCATIONS];
  uint8_t pdsch_aggregation_factor;
  fapi_nr_rate_matching_pattern_group_t rate_matching_pattern_group1;
  fapi_nr_rate_matching_pattern_group_t rate_matching_pattern_group2;
  uint8_t rbg_size;
  uint8_t mcs_table;
  uint8_t max_num_of_code_word_scheduled_by_dci;
  uint8_t bundle_size;        //  prb_bundling static
  uint8_t bundle_size_set1;   //  prb_bundling dynamic 
  uint8_t bundle_size_set2;   //  prb_bundling dynamic
  fapi_nr_zp_csi_rs_resource_t periodically_zp_csi_rs_resource_set[FAPI_NR_MAX_NUM_ZP_CSI_RS_RESOURCE_PER_SET];
} fapi_nr_pdsch_config_dedicated_t;

typedef struct {
  uint16_t starting_prb;
  uint8_t intra_slot_frequency_hopping;
  uint16_t second_hop_prb;
  uint8_t format;                 //  pucch format 0..4
  uint8_t initial_cyclic_shift;
  uint8_t number_of_symbols;
  uint8_t starting_symbol_index;
  uint8_t time_domain_occ;
  uint8_t number_of_prbs;
  uint8_t occ_length;
  uint8_t occ_index;
} fapi_nr_pucch_resource_t;

typedef struct {
  uint8_t periodicity;
  uint8_t number_of_harq_process;
  fapi_nr_pucch_resource_t n1_pucch_an;
} fapi_nr_sps_config_t;

typedef struct {
  uint8_t beam_failure_instance_max_count;
  uint8_t beam_failure_detection_timer;
} fapi_nr_radio_link_monitoring_config_t;

typedef struct {
  fapi_nr_pdcch_config_dedicated_t pdcch_config_dedicated;
  fapi_nr_pdsch_config_dedicated_t pdsch_config_dedicated;
  fapi_nr_sps_config_t sps_config;
  fapi_nr_radio_link_monitoring_config_t radio_link_monitoring_config;
} fapi_nr_dl_bwp_dedicated_config_t;

typedef struct {
  fapi_nr_rach_config_common_t  rach_config_common;
  fapi_nr_pusch_config_common_t pusch_config_common;
  fapi_nr_pucch_config_common_t pucch_config_common;

} fapi_nr_ul_bwp_common_config_t;
        
typedef struct {
  uint8_t inter_slot_frequency_hopping;
  uint8_t additional_dmrs;
  uint8_t max_code_rate;
  uint8_t number_of_slots;
  uint8_t pi2bpsk;
  uint8_t simultaneous_harq_ack_csi;
} fapi_nr_pucch_format_config_t;

typedef struct {
  fapi_nr_pucch_format_config_t format1;
  fapi_nr_pucch_format_config_t format2;
  fapi_nr_pucch_format_config_t format3;
  fapi_nr_pucch_format_config_t format4;
  fapi_nr_pucch_resource_t multi_csi_pucch_resources[2];
  uint8_t dl_data_to_ul_ack[8];
  //  pucch power control
  uint8_t deltaF_pucch_f0;
  uint8_t deltaF_pucch_f1;
  uint8_t deltaF_pucch_f2;
  uint8_t deltaF_pucch_f3;
  uint8_t deltaF_pucch_f4;
  uint8_t two_pucch_pc_adjusment_states;
} fapi_nr_pucch_config_dedicated_t;

typedef struct {
  uint8_t dmrs_type;
  uint8_t dmrs_addition_position;
  uint8_t ptrs_uplink_config; // to indicate if PTRS Uplink is configured of not
  uint8_t ptrs_type;  //cp-OFDM, dft-S-OFDM
  uint16_t ptrs_frequency_density[2];
  uint8_t ptrs_time_density[3];
  uint8_t ptrs_max_number_of_ports;
  uint8_t ptrs_resource_element_offset;
  uint8_t ptrs_power;
  uint16_t ptrs_sample_density[5];
  uint8_t ptrs_time_density_transform_precoding;

  uint8_t max_length;
  uint16_t scrambling_id0;
  uint16_t scrambling_id1;
  uint8_t npusch_identity;
  uint8_t disable_sequence_group_hopping;
  uint8_t sequence_hopping_enable;
} fapi_nr_dmrs_uplink_config_t;

typedef struct {
  uint8_t tpc_accmulation;
  uint8_t msg3_alpha;
  uint8_t p0_nominal_with_grant;
  uint8_t two_pusch_pc_adjustments_states;
  uint8_t delta_mcs;
} fapi_nr_pusch_power_control_t;

typedef enum {tx_config_codebook = 1, tx_config_nonCodebook = 2} tx_config_t;
typedef enum {transform_precoder_disabled = 0, transform_precoder_enabled = 1} transform_precoder_t;
typedef enum {
  codebook_subset_fullyAndPartialAndNonCoherent = 1,
  codebook_subset_partialAndNonCoherent = 2,
  codebook_subset_nonCoherent = 3} codebook_subset_t;
typedef struct {
  uint16_t data_scrambling_identity;
  tx_config_t tx_config;
  fapi_nr_dmrs_uplink_config_t dmrs_ul_for_pusch_mapping_type_a;
  fapi_nr_dmrs_uplink_config_t dmrs_ul_for_pusch_mapping_type_b;
  fapi_nr_pusch_power_control_t pusch_power_control;
  uint8_t frequency_hopping;
  uint16_t frequency_hopping_offset_lists[4];
  uint8_t resource_allocation;
  fapi_nr_pusch_time_domain_resource_allocation_t allocation_list[FAPI_NR_MAX_NUM_UL_ALLOCATIONS];
  uint8_t pusch_aggregation_factor;
  uint8_t mcs_table;
  uint8_t mcs_table_transform_precoder;
  transform_precoder_t transform_precoder;
  codebook_subset_t codebook_subset;
  uint8_t max_rank;
  uint8_t rbg_size;

  //uci-OnPUSCH
  uint8_t uci_on_pusch_type;  //dynamic, semi-static
  uint8_t beta_offset_ack_index1[4];
  uint8_t beta_offset_ack_index2[4];
  uint8_t beta_offset_ack_index3[4];
  uint8_t beta_offset_csi_part1_index1[4];
  uint8_t beta_offset_csi_part1_index2[4];
  uint8_t beta_offset_csi_part2_index1[4];
  uint8_t beta_offset_csi_part2_index2[4];

  uint8_t tp_pi2BPSK;
} fapi_nr_pusch_config_dedicated_t;

typedef struct {
  uint8_t frequency_hopping;
  fapi_nr_dmrs_uplink_config_t cg_dmrs_configuration;
  uint8_t mcs_table;
  uint8_t mcs_table_transform_precoder;

  //uci-OnPUSCH
  uint8_t uci_on_pusch_type;  //dynamic, semi-static
  uint8_t beta_offset_ack_index1[4];
  uint8_t beta_offset_ack_index2[4];
  uint8_t beta_offset_ack_index3[4];
  uint8_t beta_offset_csi_part1_index1[4];
  uint8_t beta_offset_csi_part1_index2[4];
  uint8_t beta_offset_csi_part2_index1[4];
  uint8_t beta_offset_csi_part2_index2[4];

  uint8_t resource_allocation;
  //  rgb-Size structure missing in spec.
  uint8_t power_control_loop_to_use;
  //  p0-PUSCH-Alpha
  uint8_t p0;
  uint8_t alpha;

  uint8_t transform_precoder;
  uint8_t number_of_harq_process;
  uint8_t rep_k;
  uint8_t rep_k_rv;
  uint8_t periodicity;
  uint8_t configured_grant_timer;
  //  rrc-ConfiguredUplinkGrant
  uint16_t time_domain_offset;
  uint8_t time_domain_allocation;
  uint32_t frequency_domain_allocation;
  uint8_t antenna_ports;
  uint8_t dmrs_seq_initialization;
  uint8_t precoding_and_number_of_layers;
  uint8_t srs_resource_indicator;
  uint8_t mcs_and_tbs;
  uint8_t frequency_hopping_offset;
  uint8_t path_loss_reference_index;

} fapi_nr_configured_grant_config_t;

typedef struct {
  uint8_t qcl_type1_serving_cell_index;
  uint8_t qcl_type1_bwp_id;
  uint8_t qcl_type1_rs_type;  //  csi-rs or ssb
  uint8_t qcl_type1_nzp_csi_rs_resource_id;
  uint8_t qcl_type1_ssb_index;
  uint8_t qcl_type1_type;
        
  uint8_t qcl_type2_serving_cell_index;
  uint8_t qcl_type2_bwp_id;
  uint8_t qcl_type2_rs_type;  //  csi-rs or ssb
  uint8_t qcl_type2_nzp_csi_rs_resource_id;
  uint8_t qcl_type2_ssb_index;
  uint8_t qcl_type2_type;

} fapi_nr_tci_state_t;

typedef struct {
  uint8_t root_sequence_index;
  //  rach genertic
  uint8_t prach_configuration_index;
  uint8_t msg1_fdm;
  uint8_t msg1_frequency_start;
  uint8_t zero_correlation_zone_config;
  uint8_t preamble_received_target_power;
  uint8_t preamble_transmission_max;
  uint8_t power_ramping_step;
  uint8_t ra_window_size;

  uint8_t rsrp_threshold_ssb;
  //  PRACH-ResourceDedicatedBFR
  uint8_t bfr_ssb_index[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint8_t bfr_ssb_ra_preamble_index[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  // NZP-CSI-RS-Resource
  uint8_t bfr_csi_rs_nzp_resource_mapping[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint8_t bfr_csi_rs_power_control_offset[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint8_t bfr_csi_rs_power_control_offset_ss[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint16_t bfr_csi_rs_scrambling_id[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint8_t bfr_csi_rs_resource_periodicity[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint16_t bfr_csi_rs_resource_offset[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  fapi_nr_tci_state_t qcl_infomation_periodic_csi_rs[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];

  uint8_t bfr_csirs_ra_occasions[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS];
  uint8_t bfr_csirs_ra_preamble_index[FAPI_NR_MAX_NUM_CANDIDATE_BEAMS][FAPI_NR_MAX_RA_OCCASION_PER_CSIRS];

  uint8_t ssb_per_rach_occasion;
  uint8_t ra_ssb_occasion_mask_index;
  fapi_nr_search_space_t recovery_search_space;
  //  RA-Prioritization
  uint8_t power_ramping_step_high_priority;
  uint8_t scaling_factor_bi;
  uint8_t beam_failure_recovery_timer;
} fapi_nr_beam_failure_recovery_config_t;

typedef struct {
  fapi_nr_pucch_config_dedicated_t pucch_config_dedicated;
  fapi_nr_pusch_config_dedicated_t pusch_config_dedicated;
  fapi_nr_configured_grant_config_t configured_grant_config;
  //  SRS-Config
  uint8_t srs_tpc_accumulation;
  fapi_nr_beam_failure_recovery_config_t beam_failure_recovery_config;
        
} fapi_nr_ul_bwp_dedicated_config_t;

#define FAPI_NR_CONFIG_REQUEST_MASK_PBCH                0x01
#define FAPI_NR_CONFIG_REQUEST_MASK_DL_BWP_COMMON       0x02
#define FAPI_NR_CONFIG_REQUEST_MASK_UL_BWP_COMMON       0x04
#define FAPI_NR_CONFIG_REQUEST_MASK_DL_BWP_DEDICATED    0x08
#define FAPI_NR_CONFIG_REQUEST_MASK_UL_BWP_DEDICATED    0x10

#endif
