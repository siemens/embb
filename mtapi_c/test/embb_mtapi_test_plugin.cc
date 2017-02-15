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

#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_plugin.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>

#include <mtapi_status_t.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/internal/unused.h>

#define PLUGIN_JOB_ID 2

embb_thread_t plugin_thread;
embb_atomic_int plugin_running;
mtapi_task_hndl_t plugin_task;
embb_atomic_int plugin_task_available;

int plugin_thread_function(void * args) {
  EMBB_UNUSED(args);
  while (embb_atomic_load_int(&plugin_running)) {
    /* wait for incoming task */
    while (embb_atomic_load_int(&plugin_running) &&
      !embb_atomic_load_int(&plugin_task_available))
      embb_thread_yield();

    if (embb_atomic_load_int(&plugin_running)) {
      if (embb_mtapi_node_is_initialized()) {
        embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

        if (embb_mtapi_task_pool_is_handle_valid(
          node->task_pool, plugin_task)) {
          embb_mtapi_task_t * local_task =
            embb_mtapi_task_pool_get_storage_for_handle(
            node->task_pool, plugin_task);
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_COMPLETED);
        }
      }
      embb_atomic_store_int(&plugin_task_available, 0);
    }
  }

  return 0;
}

void plugin_initialize(
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_IN mtapi_node_t node_id,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(domain_id);
  EMBB_UNUSED(node_id);

  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int err;

  plugin_task.id = 0;
  plugin_task.tag = 0;
  embb_atomic_init_int(&plugin_running, 1);
  embb_atomic_init_int(&plugin_task_available, 0);

  err = embb_thread_create(&plugin_thread, NULL, plugin_thread_function, NULL);
  if (EMBB_SUCCESS == err) {
    local_status = MTAPI_SUCCESS;
  }

  mtapi_status_set(status, local_status);
}

void plugin_finalize(
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int result = 0;
  int err;

  embb_atomic_store_int(&plugin_running, 0);
  err = embb_thread_join(&plugin_thread, &result);
  if (EMBB_SUCCESS == err) {
    local_status = MTAPI_SUCCESS;
  }

  embb_atomic_destroy_int(&plugin_task_available);
  embb_atomic_destroy_int(&plugin_running);

  mtapi_status_set(status, local_status);
}

void plugin_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);
      embb_mtapi_task_set_state(local_task, MTAPI_TASK_RUNNING);

      plugin_task = task;
      embb_atomic_store_int(&plugin_task_available, 1);
    }
  }

  local_status = MTAPI_SUCCESS;

  mtapi_status_set(status, local_status);
}

void plugin_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(task);
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  mtapi_status_set(status, local_status);
}

void plugin_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(action);
  mtapi_status_set(status, MTAPI_SUCCESS);
}

PluginTest::PluginTest() {
  CreateUnit("mtapi plugin test").
    Add(&PluginTest::TestBasic, this);
}

void PluginTest::TestBasic() {
  embb_mtapi_log_info("running plugin test...\n");

  mtapi_node_attributes_t node_attr;
  mtapi_info_t info;
  mtapi_status_t status;
  mtapi_job_hndl_t job;
  mtapi_action_hndl_t action;
  mtapi_task_hndl_t task;

  status = MTAPI_ERR_UNKNOWN;
  mtapi_nodeattr_init(&node_attr, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_TYPE,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_NODE_TYPE_SMP),
    MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_initialize(
    THIS_DOMAIN_ID,
    THIS_NODE_ID,
    &node_attr,
    &info,
    &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  plugin_initialize(THIS_DOMAIN_ID, THIS_NODE_ID, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
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
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  job = mtapi_job_get(
    PLUGIN_JOB_ID,
    THIS_DOMAIN_ID,
    &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,
    job,
    MTAPI_NULL, 0,
    MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES,
    MTAPI_GROUP_NONE,
    &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  plugin_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);

  embb_mtapi_log_info("...done\n\n");
}
