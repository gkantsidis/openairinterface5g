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

static int modulus_rx(nr_rlc_entity_am_t *entity, int a)
{
  /* as per 38.322 7.1, modulus base is rx_next */
  int r = a - entity->rx_next;
  if (r < 0) r += entity->sn_modulus;
  return r;
}

static int sn_in_recv_window(void *_entity, int sn)
{
  nr_rlc_entity_am_t *entity = _entity;
  int mod_sn = modulus_rx(entity, sn);
  /* we simplify rx_next <= sn < rx_next + am_window_size */
  return mod_sn < entity->window_size;
}

static int sn_compare_rx(void *_entity, int a, int b)
{
  nr_rlc_entity_am_t *entity = _entity;
  return modulus_rx(entity, a) - modulus_rx(entity, b);
}

static int segment_already_received(nr_rlc_entity_am_t *entity,
    int sn, int so, int size)
{
  nr_rlc_pdu_t *l = entity->rx_list;
  int covered;

  while (l != NULL && size > 0) {
    if (l->sn == sn) {
      if (l->so <= so && so < l->so + l->size) {
        covered = l->size - (so - l->so);
        size -= covered;
        so += covered;
      } else if (l->so <= so+size-1 && so+size-1 < l->so + l->size) {
        covered = size - (l->so - so);
        size -= covered;
      }
    }
    l = l->next;
  }

  return size <= 0;
}

static void consider_retransmission(nr_rlc_entity_am_t *entity,
    nr_rlc_sdu_segment_t *cur, int old_retx_count)
{
  int update_retx = old_retx_count == cur->sdu->retx_count;

  if (update_retx)
    cur->sdu->retx_count++;

  /* let's report max RETX reached for all retx_count >= max_retx_threshold
   * (specs say to report if retx_count == max_retx_threshold).
   * Upper layers should react (radio link failure), so no big deal actually.
   */
  if (update_retx && cur->sdu->retx_count >= entity->max_retx_threshold) {
    entity->common.max_retx_reached(entity->common.max_retx_reached_data,
                                    (nr_rlc_entity_t *)entity);
  }

  /* let's put in retransmit list even if we are over max_retx_threshold.
   * upper layers should deal with this condition, internally it's better
   * for the RLC code to keep going with this segment (we only remove
   * a segment that was ACKed)
   */
  nr_rlc_sdu_segment_list_add(&entity->retransmit_list,
                              &entity->retransmit_end,
                              cur);
}

static void reception_actions(nr_rlc_entity_am_t *entity, nr_rlc_pdu_t *pdu)
{
  int x = pdu->sn;

  if (sn_compare_rx(entity, x, entity->rx_next_highest) >= 0)
    entity->rx_next_highest = (x + 1) % entity->sn_modulus;

  if (segment_full(entity, x)) {
  }

#if 0
  vr_ms = entity->vr_ms;
  while (rlc_am_segment_full(entity, vr_ms))
    vr_ms = (vr_ms + 1) % 1024;
  entity->vr_ms = vr_ms;

  if (x == entity->vr_r) {
    vr_r = entity->vr_r;
    while (rlc_am_segment_full(entity, vr_r)) {
      /* move segments with sn=vr(r) from rx list to end of reassembly list */
      while (entity->rx_list != NULL && entity->rx_list->sn == vr_r) {
        rlc_rx_pdu_segment_t *e = entity->rx_list;
        entity->rx_list = e->next;
        e->next = NULL;
        if (entity->reassemble.start == NULL) {
          entity->reassemble.start = e;
          /* the list was empty, we need to init decoder */
          entity->reassemble.sn = -1;
          if (!rlc_am_reassemble_next_segment(&entity->reassemble)) {
            /* TODO: proper error recovery (or remove the test, it should not happen) */
            LOG_E(RLC, "%s:%d:%s: fatal\n", __FILE__, __LINE__, __FUNCTION__);
            exit(1);
          }
        } else {
          entity->reassemble.end->next = e;
        }
        entity->reassemble.end = e;
      }

      /* update vr_r */
      vr_r = (vr_r + 1) % 1024;
    }
    entity->vr_r = vr_r;
  }

  rlc_am_reassemble(entity);

  if (entity->t_reordering_start) {
    int vr_x = entity->vr_x;
    if (vr_x < entity->vr_r) vr_x += 1024;
    if (vr_x == entity->vr_r || vr_x > entity->vr_r + 512)
      entity->t_reordering_start = 0;
  }

  if (entity->t_reordering_start == 0) {
    if (sn_compare_rx(entity, entity->vr_h, entity->vr_r) > 0) {
      entity->t_reordering_start = entity->t_current;
      entity->vr_x = entity->vr_h;
    }
  }
#endif
}

void nr_rlc_entity_am_recv_pdu(nr_rlc_entity_t *_entity,
                               char *buffer, int size)
{
#define R(d) do { if (nr_rlc_pdu_decoder_in_error(&d)) goto err; } while (0)
  nr_rlc_entity_am_t *entity = (nr_rlc_entity_am_t *)_entity;
  nr_rlc_pdu_decoder_t decoder;
  nr_rlc_pdu_t *pdu;
  int dc;
  int p = 0;
  int si;
  int sn;
  int so;
  int data_size;
  int is_first;
  int is_last;

  nr_rlc_pdu_decoder_init(&decoder, buffer, size);
  dc = nr_rlc_pdu_decoder_get_bits(&decoder, 1); R(decoder);
  if (dc == 0) goto control;

  /* data PDU */
  p  = nr_rlc_pdu_decoder_get_bits(&decoder, 1); R(decoder);
  si = nr_rlc_pdu_decoder_get_bits(&decoder, 2); R(decoder);

  is_first = (si & 0x2) == 0;
  is_last = (si & 0x1) == 0;

  if (entity->sn_field_length == 18) {
    nr_rlc_pdu_decoder_get_bits(&decoder, 2); R(decoder);
  }

  sn = nr_rlc_pdu_decoder_get_bits(&decoder, entity->sn_field_length);
  R(decoder);

  if (!is_first) {
    so = nr_rlc_pdu_decoder_get_bits(&decoder, 16); R(decoder);
  }

  data_size = size - decoder.byte;

  /* dicard PDU if rx buffer is full */
  if (entity->rx_size + size > entity->rx_maxsize) {
    LOG_D(RLC, "%s:%d:%s: warning: discard PDU, RX buffer full\n",
          __FILE__, __LINE__, __FUNCTION__);
    goto discard;
  }

  if (!sn_in_recv_window(entity, sn)) {
    LOG_D(RLC, "%s:%d:%s: warning: discard PDU, sn out of window (sn %d rx_next %d)\n",
          __FILE__, __LINE__, __FUNCTION__,
           sn, entity->rx_next);
    goto discard;
  }

  /* discard segment if all the bytes of the segment are already there */
  if (segment_already_received(entity, sn, so, data_size)) {
    LOG_D(RLC, "%s:%d:%s: warning: discard PDU, already received\n",
          __FILE__, __LINE__, __FUNCTION__);
    goto discard;
  }

  /* put in pdu reception list */
  entity->rx_size += size;
  pdu = nr_rlc_new_pdu(sn, so, is_first, is_last,
                       buffer + size - data_size, data_size);
  entity->rx_list = nr_rlc_pdu_list_add(sn_compare_rx, entity,
                                        entity->rx_list, pdu);

  /* do reception actions (38.322 5.2.3.2.3) */
  reception_actions(entity, pdu);

  if (p) {
    /* 38.322 5.3.4 says status triggering should be delayed
     * until x < rx_highest_status or x >= rx_next + am_window_size.
     * This is not clear (what is x then? we keep the same?). So let's
     * trigger no matter what.
     */
    int v = (entity->rx_next + entity->window_size) % entity->sn_modulus;
    entity->status_triggered = 1;
    if (!(sn_compare_rx(entity, sn, entity->rx_highest_status) < 0 ||
          sn_compare_rx(entity, sn, v) >= 0)) {
      LOG_D(RLC, "%s:%d:%s: warning: STATUS trigger should be delayed, according to specs\n",
            __FILE__, __LINE__, __FUNCTION__);
    }
  }

  return;

control:

  return;

goto err;
err:
  LOG_W(RLC, "%s:%d:%s: error decoding PDU, discarding\n", __FILE__, __LINE__, __FUNCTION__);
  goto discard;

discard:
  if (p)
    entity->status_triggered = 1;

#undef R
}

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

  /* generate header */
  nr_rlc_pdu_encoder_init(&encoder, buffer, bufsize);

  nr_rlc_pdu_encoder_put_bits(&encoder, 1, 1);             /* D/C: 1 = data */
  nr_rlc_pdu_encoder_put_bits(&encoder, 0, 1);     /* P: reserve, set later */

  nr_rlc_pdu_encoder_put_bits(&encoder, 1-sdu->is_first,1);/* 1st bit of SI */
  nr_rlc_pdu_encoder_put_bits(&encoder, 1-sdu->is_last,1); /* 2nd bit of SI */

  if (entity->sn_field_length == 18)
    nr_rlc_pdu_encoder_put_bits(&encoder, 0, 2);                       /* R */

  nr_rlc_pdu_encoder_put_bits(&encoder, sdu->sdu->sn,
                                        entity->sn_field_length);     /* SN */

  if (!sdu->is_first)
    nr_rlc_pdu_encoder_put_bits(&encoder, sdu->so, 16);               /* SO */

  /* data */
  memcpy(buffer + encoder.byte, sdu->sdu->data + sdu->so, sdu->size);

  if (p)
    include_poll(entity, buffer);

  return encoder.byte + sdu->size;
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

  sdu->sdu->ref_count++;

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
  nr_rlc_sdu_segment_t *sdu;
  int pdu_header_size;
  int pdu_size;
  int p;

  sdu = entity->retransmit_list;

  pdu_header_size = compute_pdu_header_size(entity, sdu);

  /* not enough room for at least one byte of data? do nothing */
  if (pdu_header_size + 1 > size)
    return 0;

  entity->retransmit_list = entity->retransmit_list->next;

  /* segment if necessary */
  pdu_size = pdu_header_size + sdu->size;
  if (pdu_size > size) {
    nr_rlc_sdu_segment_t *next_sdu;
    next_sdu = resegment(sdu, entity, size);
    /* put the second SDU back at the head of the retransmit list */
    next_sdu->next = entity->retransmit_list;
    entity->retransmit_list = next_sdu;
  }

  /* put SDU/SDU segment in the wait list */
  nr_rlc_sdu_segment_list_add(&entity->wait_list, &entity->wait_end, sdu);

  p = check_poll_after_pdu_assembly(entity);

  if (entity->force_poll) {
    p = 1;
    entity->force_poll = 0;
  }

  return serialize_sdu(entity, sdu, buffer, size, p);
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

  /* put SDU/SDU segment in the wait list */
  nr_rlc_sdu_segment_list_add(&entity->wait_list, &entity->wait_end, sdu);

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

nr_rlc_entity_buffer_status_t nr_rlc_entity_am_buffer_status(
    nr_rlc_entity_t *_entity, int maxsize)
{
  nr_rlc_entity_am_t *entity = (nr_rlc_entity_am_t *)_entity;
  nr_rlc_entity_buffer_status_t ret;
  ret.status_size = ret.tx_size = ret.retx_size = 0;
  return ret;
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

/*************************************************************************/
/* time/timers                                                           */
/*************************************************************************/

static void check_t_poll_retransmit(nr_rlc_entity_am_t *entity)
{
  nr_rlc_sdu_segment_t head;
  nr_rlc_sdu_segment_t *cur;
  nr_rlc_sdu_segment_t *prev;
  int sn;
  int old_retx_count;

  /* 38.322 5.3.3.4 */
  /* did t_poll_retransmit expire? */
  if (entity->t_poll_retransmit_start == 0 ||
      entity->t_current <= entity->t_poll_retransmit_start +
                               entity->t_poll_retransmit)
    return;

  /* stop timer */
  entity->t_poll_retransmit_start = 0;

  /* 38.322 5.3.3.4 says:
   *
   *     - include a poll in a RLC data PDU as described in section 5.3.3.2
   *
   * That does not seem to be conditional. So we forcefully will send
   * a poll as soon as we generate a PDU.
   * Hopefully this interpretation is correct. In the worst case we generate
   * more polling than necessary, but it's not a big deal. When
   * 't_poll_retransmit' expires it means we didn't receive a status report,
   * meaning a bad radio link, so things are quite bad at this point and
   * asking again for a poll won't hurt much more.
   */
  entity->force_poll = 1;

  LOG_D(RLC, "%s:%d:%s: warning: t_poll_retransmit expired\n",
        __FILE__, __LINE__, __FUNCTION__);

  /* do we meet conditions of 38.322 5.3.3.4? */
  if (!check_poll_after_pdu_assembly(entity))
    return;

  /* search wait list for SDU with highest SN */
  /* this code may be incorrect: in LTE we had to look for PDU
   * with SN = VT(S) - 1, but for NR the specs say "highest SN among the
   * ones submitted to lower layers" not 'tx_next - 1'. So we should look
   * for the highest SN in the wait list. But that's no big deal. If the
   * program runs this code, then the connection is in a bad state and we
   * can retransmit whatever we want. At some point we will receive a status
   * report and retransmit what we really have to. Actually we could just
   * retransmit the head of wait list (the specs have this 'or').
   * (Actually, maybe this interpretation is not correct and what the code
   * does is correct. The specs are confusing.)
   */
  sn = (entity->tx_next - 1 + entity->sn_modulus) % entity->sn_modulus;

  head.next = entity->wait_list;
  cur = entity->wait_list;
  prev = &head;

  while (cur != NULL) {
    if (cur->sdu->sn == sn)
      break;
    prev = cur;
    cur = cur->next;
  }

  /* SDU with highest SN not found? take the head of wait list */
  if (cur == NULL) {
    cur = entity->wait_list;
    prev = &head;
    sn = cur->sdu->sn;
  }

  /* todo: do we need to for check cur == NULL?
   * It seems that no, the wait list should not be empty here, but not sure.
   */

  old_retx_count = cur->sdu->retx_count;

  /* 38.322 says "SDU", not "SDU segment", so let's retransmit all
   * SDU segments with this SN
   */
  /* todo: maybe we could simply retransmit the current SDU segment,
   * so that we don't have to run through the full wait list.
   */
  while (cur != NULL) {
    if (cur->sdu->sn == sn) {
      prev->next = cur->next;
      /* put in retransmit list */
      consider_retransmission(entity, cur, old_retx_count);
    } else {
      prev = cur;
    }
    cur = prev->next;
  }
  entity->wait_list = head.next;
}

void nr_rlc_entity_am_set_time(nr_rlc_entity_t *_entity, uint64_t now)
{
  nr_rlc_entity_am_t *entity = (nr_rlc_entity_am_t *)_entity;

  entity->t_current = now;

  check_t_poll_retransmit(entity);
}
