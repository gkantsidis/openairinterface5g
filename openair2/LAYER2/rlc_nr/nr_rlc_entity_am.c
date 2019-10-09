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

#include "nr_rlc_entity_am.h"

#include <stdlib.h>
#include <string.h>

#include "nr_rlc_pdu.h"

#include "LOG/log.h"

/*************************************************************************/
/* PDU RX functions                                                      */
/*************************************************************************/


/*************************************************************************/
/* TX functions                                                          */
/*************************************************************************/

static int modulus_tx(nr_rlc_entity_am_t *entity, int a)
{
  int r = a - entity->tx_next_ack;
  if (r < 0) r += entity->sn_modulus;
  return r;
}

static int sn_compare_tx(void *_entity, int a, int b)
{
  nr_rlc_entity_am_t *entity = _entity;
  return modulus_tx(entity, a) - modulus_tx(entity, b);
}

static int is_window_stalling(nr_rlc_entity_am_t *entity)
{
  /* we are stalling if tx_next is not:
   *   tx_next_ack <= tx_next < tx_next_ack + window_size
   */
  return !(sn_compare_tx(entity, entity->tx_next_ack, entity->tx_next) <= 0 &&
           sn_compare_tx(entity, entity->tx_next,
                         (entity->tx_next_ack + entity->window_size) %
                           entity->sn_modulus) < 0);
}
static void include_poll(nr_rlc_entity_am_t *entity, char *buffer)
{
  /* set the P bit to 1 */
  buffer[0] |= 0x40;

  entity->pdu_without_poll = 0;
  entity->byte_without_poll = 0;

  /* set POLL_SN to highest SN submitted to lower layer
   * (this is: entity->tx_next - 1) (todo: be sure of this)
   */
  entity->poll_sn = (entity->tx_next - 1 + entity->sn_modulus)
                      % entity->sn_modulus;

  /* start/restart t_poll_retransmit */
  entity->t_poll_retransmit_start = entity->t_current;
}

static int check_poll_after_pdu_assembly(nr_rlc_entity_am_t *entity)
{
  int retransmission_buffer_empty;
  int transmission_buffer_empty;

  /* is transmission buffer empty? */
  if (entity->tx_list == NULL)
    transmission_buffer_empty = 1;
  else
    transmission_buffer_empty = 0;

  /* is retransmission buffer empty? */
  if (entity->retransmit_list == NULL)
    retransmission_buffer_empty = 1;
  else
    retransmission_buffer_empty = 0;

  return (transmission_buffer_empty && retransmission_buffer_empty) ||
         is_window_stalling(entity);
}

static int serialize_sdu(nr_rlc_entity_am_t *entity,
                         nr_rlc_sdu_segment_t *sdu, char *buffer, int bufsize,
                         int p)
{
  nr_rlc_pdu_encoder_t encoder;
  int sn_length;

  /* generate header */
  nr_rlc_pdu_encoder_init(&encoder, buffer, bufsize);

  nr_rlc_pdu_encoder_put_bits(&encoder, 1, 1);             /* D/C: 1 = data */
  nr_rlc_pdu_encoder_put_bits(&encoder, 0, 1);     /* P: reserve, set later */

  nr_rlc_pdu_encoder_put_bits(&encoder, 1-sdu->is_first,1);/* 1st bit of SI */
  nr_rlc_pdu_encoder_put_bits(&encoder, 1-sdu->is_last,1); /* 2nd bit of SI */

  /* let's include the 2 R bits (when SN field length is 18) in sn_length */
  if (entity->sn_field_length == 12)
    sn_length = 12;
  else
    sn_length = 18 + 2;

  nr_rlc_pdu_encoder_put_bits(&encoder, sdu->sdu->sn, sn_length); /* (R+)SN */

  if (!sdu->is_first)
    nr_rlc_pdu_encoder_put_bits(&encoder, sdu->so, 16);               /* SO */

  /* data */
  memcpy(buffer + encoder.byte, sdu->sdu->data + sdu->so, sdu->size);

  if (p)
    include_poll(entity, buffer);

  return 0;
}

/* for a given SDU/SDU segment, computes the corresponding PDU header size */
static int compute_pdu_header_size(nr_rlc_entity_am_t *entity,
                                   nr_rlc_sdu_segment_t *sdu)
{
  int header_size = 2;
  /* one more byte if SN field length is 18 */
  if (entity->sn_field_length == 18)
    header_size++;
  /* two more bytes for SO if SDU segment is not the first */
  if (!sdu->is_first) header_size += 2;
  return header_size;
}

/* resize SDU/SDU segment for the corresponding PDU to fit into 'pdu_size'
 * bytes
 * - modifies SDU/SDU segment to become an SDU segment
 * - returns a new SDU segment covering the remaining data bytes
 */
static nr_rlc_sdu_segment_t *resegment(nr_rlc_sdu_segment_t *sdu,
                                       nr_rlc_entity_am_t *entity,
                                       int pdu_size)
{
  nr_rlc_sdu_segment_t *next;
  int pdu_header_size;
  int over_size;

  pdu_header_size = compute_pdu_header_size(entity, sdu);

  next = calloc(1, sizeof(nr_rlc_sdu_segment_t));
  if (next == NULL) {
    LOG_E(RLC, "%s:%d:%s: out of memory\n", __FILE__, __LINE__,  __FUNCTION__);
    exit(1);
  }
  *next = *sdu;

  over_size = pdu_header_size + sdu->size - pdu_size;

  /* update SDU */
  sdu->size -= over_size;
  sdu->is_last = 0;

  /* create new segment */
  next->size = over_size;
  next->so = sdu->so + sdu->size;
  next->is_first = 0;

  return next;
}

static int generate_status(nr_rlc_entity_am_t *entity, char *buffer, int size)
{
  return 0;
}

static int generate_retx_pdu(nr_rlc_entity_am_t *entity, char *buffer,
                             int size)
{
  int p;

  if (entity->force_poll) {
    p = 1;
    entity->force_poll = 0;
  }

printf("%d\n", p);
  return 0;
}

static int generate_tx_pdu(nr_rlc_entity_am_t *entity, char *buffer, int size)
{
  nr_rlc_sdu_segment_t *sdu;
  int pdu_header_size;
  int pdu_size;
  int p;

  /* sn out of window (that is: we have window stalling)? do nothing */
  if (is_window_stalling(entity))
    return 0;

  if (entity->tx_list == NULL)
    return 0;

  sdu = entity->tx_list;

  pdu_header_size = compute_pdu_header_size(entity, sdu);

  /* not enough room for at least one byte of data? do nothing */
  if (pdu_header_size + 1 > size)
    return 0;

  entity->tx_list = entity->tx_list->next;

  /* assign SN to SDU if not assigned yet */
  if (sdu->sdu->sn == -1) {
    sdu->sdu->sn = entity->tx_next;
    entity->tx_next = (entity->tx_next + 1) % entity->sn_modulus;
  }

  /* segment if necessary */
  pdu_size = pdu_header_size + sdu->size;
  if (pdu_size > size) {
    nr_rlc_sdu_segment_t *next_sdu;
    next_sdu = resegment(sdu, entity, size);
    /* put the second SDU back at the head of the TX list */
    next_sdu->next = entity->tx_list;
    entity->tx_list = next_sdu;
  }

  /* polling actions for a new PDU */
  entity->pdu_without_poll++;
  entity->byte_without_poll += sdu->size;
  if ((entity->poll_pdu != -1 &&
       entity->pdu_without_poll >= entity->poll_pdu) ||
      (entity->poll_byte != -1 &&
       entity->byte_without_poll >= entity->poll_byte))
    p = 1;
  else
    p = check_poll_after_pdu_assembly(entity);

  if (entity->force_poll) {
    p = 1;
    entity->force_poll = 0;
  }

  return serialize_sdu(entity, sdu, buffer, size, p);
}

static int status_to_report(nr_rlc_entity_am_t *entity)
{
  return 0;
}

int nr_rlc_entity_am_generate_pdu(nr_rlc_entity_t *_entity,
                                  char *buffer, int size)
{
  nr_rlc_entity_am_t *entity = (nr_rlc_entity_am_t *)_entity;
  int ret;

  if (status_to_report(entity)) {
    ret = generate_status(entity, buffer, size);
    if (ret != 0)
      return ret;
  }

  if (entity->retransmit_list != NULL) {
    ret = generate_retx_pdu(entity, buffer, size);
    if (ret != 0)
      return ret;
  }

  return generate_tx_pdu(entity, buffer, size);
}

/*************************************************************************/
/* SDU RX functions                                                      */
/*************************************************************************/

void nr_rlc_entity_am_recv_sdu(nr_rlc_entity_t *_entity,
                               char *buffer, int size,
                               int sdu_id)
{
  nr_rlc_entity_am_t *entity = (nr_rlc_entity_am_t *)_entity;
  nr_rlc_sdu_segment_t *sdu;

  if (size > NR_SDU_MAX) {
    LOG_E(RLC, "%s:%d:%s: fatal: SDU size too big (%d bytes)\n",
          __FILE__, __LINE__, __FUNCTION__, size);
    exit(1);
  }

  if (entity->tx_size + size > entity->tx_maxsize) {
    LOG_D(RLC, "%s:%d:%s: warning: SDU rejected, SDU buffer full\n",
          __FILE__, __LINE__, __FUNCTION__);
    return;
  }

  entity->tx_size += size;

  sdu = nr_rlc_new_sdu(buffer, size, sdu_id);

  nr_rlc_sdu_segment_list_add(&entity->tx_list, &entity->tx_end, sdu);
}
