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

#include <stdlib.h>

#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_queue.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/internal/unused.h>

#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23
#define QUEUE_TEST_ID 17

static void testQueueAction(
  const void* args,
  mtapi_size_t /*arg_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* task_context) {
  int workload_id = *reinterpret_cast<const int*>(args);
  int ii;
  mtapi_uint_t core_num = mtapi_context_corenum_get(task_context, MTAPI_NULL);
  srand(core_num);
  for (ii = 1000; ii < rand()%1000000; ii ++) {
  }
  embb_mtapi_log_info("testQueueAction %d called from worker %d...\n",
    workload_id, core_num);
  EMBB_UNUSED_IN_RELEASE(workload_id);
}

static void testDoSomethingElse() {
}

QueueTest::QueueTest() {
  CreateUnit("mtapi queue test").Add(&QueueTest::TestBasic, this);
}

void QueueTest::TestBasic() {
  mtapi_info_t info;
  mtapi_status_t status;
  mtapi_action_hndl_t action;
  mtapi_job_hndl_t job;
  mtapi_task_hndl_t task;
  mtapi_queue_hndl_t queue;
  const mtapi_size_t args_size = sizeof(int);
  const int args = 42;

  embb_mtapi_log_info("running testQueue...\n");

  status = MTAPI_ERR_UNKNOWN;
  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES, &info, &status);
  MTAPI_CHECK_STATUS(status);

  embb_mtapi_log_trace("mtapi successfully initialized...\n");
  embb_mtapi_log_trace(
    "hardware concurrency   : %d\n", info.hardware_concurrency);
  embb_mtapi_log_trace("used memory            : %d\n", info.used_memory);

  /* create action */
  status = MTAPI_ERR_UNKNOWN;
  action = mtapi_action_create(JOB_TEST_TASK, (testQueueAction),
    MTAPI_NULL, 0, MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  MTAPI_CHECK_STATUS(status);

  /* get job */
  status = MTAPI_ERR_UNKNOWN;
  job = mtapi_job_get(JOB_TEST_TASK, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);

  /* create queue */
  status = MTAPI_ERR_UNKNOWN;
  queue = mtapi_queue_create(QUEUE_TEST_ID, job,
    MTAPI_DEFAULT_QUEUE_ATTRIBUTES, &status);
  MTAPI_CHECK_STATUS(status);

  /* enqueue task in queue (repeat this for all tasks that should be
     processed by that queue) */
  status = MTAPI_ERR_UNKNOWN;
  task = mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue,
    reinterpret_cast<const void*>(&args),
    args_size, MTAPI_NULL, 0, MTAPI_DEFAULT_TASK_ATTRIBUTES,
    MTAPI_GROUP_NONE, &status);
  MTAPI_CHECK_STATUS(status);

  /* do something else */
  testDoSomethingElse();

  /* wait for task completion */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_delete(queue, 10, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(action, 10, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  PT_EXPECT(embb_get_bytes_allocated() == 0);

  embb_mtapi_log_info("...done\n\n");
}
