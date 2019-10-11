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

#include "nr_rlc_sdu.h"

#include <stdlib.h>
#include <string.h>

#include "LOG/log.h"

nr_rlc_sdu_segment_t *nr_rlc_new_sdu(
    char *buffer, int size,
    int upper_layer_id)
{
  nr_rlc_sdu_t *sdu         = calloc(1, sizeof(nr_rlc_sdu_t));
  nr_rlc_sdu_segment_t *ret = calloc(1, sizeof(nr_rlc_sdu_segment_t));
  if (sdu == NULL || ret == NULL)
    goto oom;

  sdu->sn             = -1;                 /* set later */
  sdu->upper_layer_id = upper_layer_id;
  sdu->data           = malloc(size);
  if (sdu->data == NULL)
    goto oom;
  memcpy(sdu->data, buffer, size);
  sdu->size           = size;
  sdu->retx_count     = -1;

  ret->sdu      = sdu;
  ret->size     = size;
  ret->so       = 0;
  ret->is_first = 1;
  ret->is_last  = 1;

  return ret;

oom:
  LOG_E(RLC, "%s:%d:%s: out of memory\n", __FILE__, __LINE__,  __FUNCTION__);
  exit(1);
}

void nr_rlc_free_sdu_segment(nr_rlc_sdu_segment_t *sdu)
{
  sdu->sdu->ref_count--;
  if (sdu->sdu->ref_count == 0) {
    free(sdu->sdu->data);
    free(sdu->sdu);
  }
  free(sdu);
}

void nr_rlc_sdu_segment_list_add(nr_rlc_sdu_segment_t **list,
                                 nr_rlc_sdu_segment_t **end,
                                 nr_rlc_sdu_segment_t *sdu)
{
  if (*list == NULL) {
    *list = sdu;
    *end = sdu;
    return;
  }

  (*end)->next = sdu;
  *end = sdu;
}
