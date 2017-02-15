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

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/c/mtapi_ext.h>

#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>

#include <mtapi_status_t.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/internal/unused.h>

#define PLUGIN_DOMAIN_ID 1
#define PLUGIN_NODE_ID 1
#define PLUGIN_JOB_ID 1

#include <stdlib.h>
#include <stdio.h>
#include <embb/base/c/log.h>
#define MTAPI_CHECK_STATUS(status) \
if (MTAPI_SUCCESS != status) { \
  printf("...error %d\n\n", status); \
  exit(status); \
}

// snippet_begin:mtapi_c_plugin_task_schedule
void plugin_task_schedule(embb_mtapi_task_t* local_task) {
  // here the task might be dispatched to some hardware or separate thread

  // mark the task as running
  embb_mtapi_task_set_state(local_task, MTAPI_TASK_RUNNING);

  // nothing to do to execute the no-op task

  // just mark the task as done
  embb_mtapi_task_set_state(local_task, MTAPI_TASK_COMPLETED);
}
// snippet_end

// snippet_begin:mtapi_c_plugin_task_start_cb
void plugin_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  // do we have a node?
  if (embb_mtapi_node_is_initialized()) {
    // get the node instance
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    // is this a valid task?
    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      // get the tasks storage
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      // dispatch the task
      plugin_task_schedule(local_task);

      local_status = MTAPI_SUCCESS;
    }
    else {
      local_status = MTAPI_ERR_TASK_INVALID;
    }
  }
  else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
// snippet_end

// snippet_begin:mtapi_c_plugin_task_cancel_cb
void plugin_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(task);
  // nothing to cancel in this simple example
  mtapi_status_set(status, MTAPI_SUCCESS);
}
// snippet_end

// snippet_begin:mtapi_c_plugin_action_finalize_cb
void plugin_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(action);
  // nothing to do for tearing down the plugin action
  mtapi_status_set(status, MTAPI_SUCCESS);
}
// snippet_end

void RunMTAPI_C_Plugin() {
  mtapi_status_t status;
  mtapi_job_hndl_t job;
  mtapi_action_hndl_t action;
  mtapi_task_hndl_t task;

  mtapi_initialize(
    PLUGIN_DOMAIN_ID,
    PLUGIN_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES,
    MTAPI_NULL,
    &status);
  MTAPI_CHECK_STATUS(status);

  // snippet_begin:mtapi_c_plugin_action_create
  action = mtapi_ext_plugin_action_create(
    PLUGIN_JOB_ID,
    plugin_task_start,
    plugin_task_cancel,
    plugin_action_finalize,
    MTAPI_NULL,
    MTAPI_NULL,
    0,
    MTAPI_DEFAULT_ACTION_ATTRIBUTES,
    &status);
  // snippet_end
  MTAPI_CHECK_STATUS(status);

  // snippet_begin:mtapi_c_plugin_get_job
  job = mtapi_job_get(
    PLUGIN_JOB_ID,
    PLUGIN_DOMAIN_ID,
    &status);
  // snippet_end
  MTAPI_CHECK_STATUS(status);

  // snippet_begin:mtapi_c_plugin_task_do_start
  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,
    job,
    MTAPI_NULL, 0,
    MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES,
    MTAPI_GROUP_NONE,
    &status);
  // snippet_end
  MTAPI_CHECK_STATUS(status);

  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);
}
