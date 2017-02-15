/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>

#include <embb/base/c/internal/bitset.h>

#include <embb/mtapi/c/mtapi.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_node_t.h>


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_affinity_init(
  MTAPI_OUT mtapi_affinity_t* mask,
  MTAPI_IN mtapi_boolean_t affinity,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

  embb_mtapi_log_trace("mtapi_affinity_init() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (MTAPI_NULL != mask) {
      embb_bitset_clear_all(mask);
      if (affinity) {
        embb_bitset_set_n(mask, node->attributes.num_cores);
      }
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_PARAMETER;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_affinity_set(
  MTAPI_INOUT mtapi_affinity_t* mask,
  MTAPI_IN mtapi_uint_t core_num,
  MTAPI_IN mtapi_boolean_t affinity,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

  embb_mtapi_log_trace("mtapi_affinity_set() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (MTAPI_NULL != mask) {
      if (core_num < node->attributes.num_cores) {
        if (affinity) {
          embb_bitset_set(mask, core_num);
        } else {
          embb_bitset_clear(mask, core_num);
        }
        local_status = MTAPI_SUCCESS;
      } else {
        local_status = MTAPI_ERR_CORE_NUM;
      }
    } else {
      local_status = MTAPI_ERR_PARAMETER;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

mtapi_boolean_t mtapi_affinity_get(
  MTAPI_OUT mtapi_affinity_t* mask,
  MTAPI_IN mtapi_uint_t core_num,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_boolean_t affinity = MTAPI_FALSE;
  embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

  embb_mtapi_log_trace("mtapi_affinity_get() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (MTAPI_NULL != mask) {
      if (core_num < node->attributes.num_cores) {
        affinity =
          embb_bitset_is_set(mask, core_num) ? MTAPI_TRUE : MTAPI_FALSE;
        local_status = MTAPI_SUCCESS;
      } else {
        local_status = MTAPI_ERR_CORE_NUM;
      }
    } else {
      local_status = MTAPI_ERR_PARAMETER;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return affinity;
}
