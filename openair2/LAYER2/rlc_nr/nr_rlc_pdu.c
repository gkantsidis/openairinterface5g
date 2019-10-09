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

#include "nr_rlc_pdu.h"

#include <stdlib.h>

#include "LOG/log.h"

void nr_rlc_pdu_encoder_init(nr_rlc_pdu_encoder_t *encoder, 
                             char *buffer, int size)
{
  encoder->byte = 0;
  encoder->bit = 0;
  encoder->buffer = buffer;
  encoder->size = size;
}

static void put_bit(nr_rlc_pdu_encoder_t *encoder, int bit)
{
  if (encoder->byte == encoder->size) {
    LOG_E(RLC, "%s:%d:%s: fatal, buffer full\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  encoder->buffer[encoder->byte] <<= 1;
  if (bit)
    encoder->buffer[encoder->byte] |= 1;

  encoder->bit++;
  if (encoder->bit == 8) {
    encoder->bit = 0;
    encoder->byte++;
  }
}

void nr_rlc_pdu_encoder_put_bits(nr_rlc_pdu_encoder_t *encoder,
                                 int value, int count)
{
  int i;
  int x;

  if (count > 31) {
    LOG_E(RLC, "%s:%d:%s: fatal\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  x = 1 << (count - 1);
  for (i = 0; i < count; i++, x >>= 1)
    put_bit(encoder, value & x);
}
