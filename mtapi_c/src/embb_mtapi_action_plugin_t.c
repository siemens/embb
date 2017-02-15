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

#include <embb/mtapi/c/mtapi_ext.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_job_t.h>
#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>

mtapi_action_hndl_t mtapi_ext_plugin_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN mtapi_ext_plugin_task_start_function_t task_start_function,
  MTAPI_IN mtapi_ext_plugin_task_cancel_function_t task_cancel_function,
  MTAPI_IN mtapi_ext_plugin_action_finalize_function_t
    action_finalize_function,
  MTAPI_IN void* plugin_data,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_IN mtapi_action_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_action_hndl_t action_handle = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };

  embb_mtapi_log_trace("mtapi_ext_plugin_action_create() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    /* check if job is valid */
    if (embb_mtapi_job_is_id_valid(node, job_id)) {
      embb_mtapi_job_t* job = embb_mtapi_job_get_storage_for_id(node, job_id);
      embb_mtapi_action_t* new_action =
        embb_mtapi_action_pool_allocate(node->action_pool);
      if (MTAPI_NULL != new_action) {
        new_action->domain_id = node->domain_id;
        new_action->node_id = node->node_id;
        new_action->job_id = job_id;
        new_action->node_local_data = node_local_data;
        new_action->node_local_data_size = node_local_data_size;
        new_action->enabled = MTAPI_TRUE;
        new_action->is_plugin_action = MTAPI_TRUE;
        embb_atomic_init_int(&new_action->num_tasks, 0);

        new_action->plugin_task_start_function = task_start_function;
        new_action->plugin_task_cancel_function = task_cancel_function;
        new_action->plugin_action_finalize_function = action_finalize_function;
        new_action->plugin_data = (void*)plugin_data;

        /* set defaults if no attributes were given */
        if (MTAPI_NULL != attributes) {
          new_action->attributes = *attributes;
          local_status = MTAPI_SUCCESS;
        } else {
          /* use the default */
          mtapi_actionattr_init(&new_action->attributes, &local_status);
        }

        /* check if affinity is sane */
        if (0 == new_action->attributes.affinity) {
          local_status = MTAPI_ERR_PARAMETER;
        }

        if (MTAPI_SUCCESS == local_status) {
          action_handle = new_action->handle;
          embb_mtapi_job_add_action(job, new_action);
        } else {
          embb_mtapi_action_finalize(new_action);
          embb_mtapi_action_pool_deallocate(node->action_pool, new_action);
        }
      } else {
        /* no more space left in action pool */
        local_status = MTAPI_ERR_ACTION_LIMIT;
      }
    } else {
      local_status = MTAPI_ERR_JOB_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return action_handle;
}
