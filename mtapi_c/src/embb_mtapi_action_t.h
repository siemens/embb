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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_ACTION_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_ACTION_T_H_

#include <embb/mtapi/c/mtapi_ext.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_pool_template.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Action class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_action_struct {
  mtapi_action_hndl_t handle;

  mtapi_domain_t domain_id;
  mtapi_node_t node_id;
  mtapi_job_id_t job_id;
  mtapi_action_function_t action_function;
  const void* node_local_data;
  mtapi_size_t node_local_data_size;
  mtapi_action_attributes_t attributes;
  mtapi_boolean_t enabled;

  mtapi_boolean_t is_plugin_action;
  void* plugin_data;
  mtapi_ext_plugin_task_start_function_t plugin_task_start_function;
  mtapi_ext_plugin_task_cancel_function_t plugin_task_cancel_function;
  mtapi_ext_plugin_action_finalize_function_t plugin_action_finalize_function;

  embb_atomic_int num_tasks;
};

#include <embb_mtapi_action_t_fwd.h>

/**
 * Default constructor.
 * \memberof embb_mtapi_action_struct
 */
void embb_mtapi_action_initialize(embb_mtapi_action_t* that);

/**
 * Destructor.
 * \memberof embb_mtapi_action_struct
 */
void embb_mtapi_action_finalize(embb_mtapi_action_t* that);


/* ---- POOL DECLARATION --------------------------------------------------- */

embb_mtapi_pool(action)


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_ACTION_T_H_
