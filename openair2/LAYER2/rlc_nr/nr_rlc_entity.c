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

#include "nr_rlc_entity.h"

#include <stdlib.h>

#include "nr_rlc_entity_am.h"

#include "LOG/log.h"

nr_rlc_entity_t *new_nr_rlc_entity_am(
    int rx_maxsize,
    int tx_maxsize,
    void (*deliver_sdu)(void *deliver_sdu_data, struct nr_rlc_entity_t *entity,
                      char *buf, int size),
    void *deliver_sdu_data,
    void (*sdu_successful_delivery)(void *sdu_successful_delivery_data,
                                    struct nr_rlc_entity_t *entity,
                                    int sdu_id),
    void *sdu_successful_delivery_data,
    void (*max_retx_reached)(void *max_retx_reached_data,
                             struct nr_rlc_entity_t *entity),
    void *max_retx_reached_data,
    int t_poll_retransmit,
    int t_reassembly,
    int t_status_prohibit,
    int poll_pdu,
    int poll_byte,
    int max_retx_threshold,
    int sn_field_length)
{
  nr_rlc_entity_am_t *ret;

  ret = calloc(1, sizeof(nr_rlc_entity_am_t));
  if (ret == NULL) {
    LOG_E(RLC, "%s:%d:%s: out of memory\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  ret->tx_maxsize = tx_maxsize;
  ret->rx_maxsize = rx_maxsize;

  ret->t_poll_retransmit  = t_poll_retransmit;
  ret->t_reassembly       = t_reassembly;
  ret->t_status_prohibit  = t_status_prohibit;
  ret->poll_pdu           = poll_pdu;
  ret->poll_byte          = poll_byte;
  ret->max_retx_threshold = max_retx_threshold;
  ret->sn_field_length    = sn_field_length;

  if (!(sn_field_length == 12 || sn_field_length == 18)) {
    LOG_E(RLC, "%s:%d:%s: wrong SN field_lenght (%d), must be 12 or 18\n",
          __FILE__, __LINE__, __FUNCTION__, sn_field_length);
    exit(1);
  }
  ret->sn_modulus = 1 << ret->sn_field_length;
  ret->window_size = ret->sn_modulus / 2;

  ret->common.recv_sdu = nr_rlc_entity_am_recv_sdu;

  return (nr_rlc_entity_t *)ret;
}

nr_rlc_entity_t *new_nr_rlc_entity_um(
    int rx_maxsize,
    int tx_maxsize,
    void (*deliver_sdu)(void *deliver_sdu_data, struct nr_rlc_entity_t *entity,
                      char *buf, int size),
    void *deliver_sdu_data,
    int t_reordering,
    int sn_field_length)
{
  return 0;
}
