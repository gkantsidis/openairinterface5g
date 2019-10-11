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

#ifndef _NR_RLC_ENTITY_UM_H_
#define _NR_RLC_ENTITY_UM_H_

#include "nr_rlc_entity.h"
#include "nr_rlc_sdu.h"

typedef struct {
  nr_rlc_entity_t common;

  /* set to the latest know time by the user of the module. Unit: ms */
  uint64_t t_current;

} nr_rlc_entity_um_t;

#endif /* _NR_RLC_ENTITY_UM_H_ */
