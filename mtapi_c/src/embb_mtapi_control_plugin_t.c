/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#include <embb/base/c/atomic.h>

#include <embb_mtapi_control_plugin_t.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>


/* ---- POOL STORAGE ------------------------------------------------------- */

#define MTAPI_MAX_CONTROL_PLUGINS 32
static embb_mtapi_control_plugin_t control_plugin_pool[MTAPI_MAX_CONTROL_PLUGINS];
static embb_atomic_int control_plugin_count = { 0 };


/* ---- CLASS MEMBERS ------------------------------------------------------ */

void embb_mtapi_ext_control_plugin_initialize(
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_IN mtapi_node_t node_id,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_SUCCESS;

  for (int ii = 0; ii < embb_atomic_load_int(&control_plugin_count); ii++) {
    mtapi_status_t plugin_status = MTAPI_ERR_UNKNOWN;
    control_plugin_pool[ii].initialize_function(domain_id, node_id, &plugin_status);
    if (plugin_status != MTAPI_SUCCESS) {
      local_status = plugin_status;
      control_plugin_pool[ii].initialized = MTAPI_FALSE;
    } else {
      control_plugin_pool[ii].initialized = MTAPI_TRUE;
    }
  }

  mtapi_status_set(status, local_status);
}

void embb_mtapi_ext_control_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_SUCCESS;

  for (int ii = 0; ii < embb_atomic_load_int(&control_plugin_count); ii++) {
    mtapi_status_t plugin_status = MTAPI_ERR_UNKNOWN;
    if (control_plugin_pool[ii].initialized) {
      control_plugin_pool[ii].finalize_function(&plugin_status);
      if (plugin_status != MTAPI_SUCCESS) {
        local_status = plugin_status;
      } else {
        control_plugin_pool[ii].initialized = MTAPI_FALSE;
      }
    }
  }

  mtapi_status_set(status, local_status);
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_ext_register_control_plugin(
  MTAPI_IN mtapi_ext_control_plugin_initialize_function_t initialize_function,
  MTAPI_IN mtapi_ext_control_plugin_finalize_function_t finalize_function,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  int index = embb_atomic_fetch_and_add_int(&control_plugin_count, 1);
  if (index < MTAPI_MAX_CONTROL_PLUGINS) {
    control_plugin_pool[index].initialize_function = initialize_function;
    control_plugin_pool[index].finalize_function = finalize_function;
    local_status = MTAPI_SUCCESS;
  } else {
    embb_atomic_store_int(&control_plugin_count, MTAPI_MAX_CONTROL_PLUGINS);
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}
