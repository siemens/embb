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

#include <stdlib.h>

#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_queue.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/internal/unused.h>
#include <embb/base/c/atomic.h>

#define JOB_TEST_TASK 42
#define JOB_WAIT_TASK 43
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
  EMBB_UNUSED(workload_id);
}

static void testQueueWaitAction(
  const void* args,
  mtapi_size_t /*arg_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* task_context) {
  int workload_id = 42;
  mtapi_job_hndl_t job;
  const int kTasks = 100;
  mtapi_task_hndl_t task[kTasks];
  mtapi_status_t status;
  embb_atomic_int * test = reinterpret_cast<embb_atomic_int *>(
    const_cast<void*>(args));
  int ii;

  /* we're running from an ordered queue, so the common atomic should be 0
     and we're trying to set it to 1 */
  int expected = 0;
  if (0 == embb_atomic_compare_and_swap_int(test, &expected, 1)) {
    /* it is not, return an error */
    mtapi_context_status_set(task_context, MTAPI_ERR_ACTION_FAILED, &status);
    MTAPI_CHECK_STATUS(status);
    return;
  }

  job = mtapi_job_get(JOB_TEST_TASK, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);

  /* spawn some tasks to wait for */
  for (ii = 0; ii < kTasks; ii++) {
    task[ii] = mtapi_task_start(
      MTAPI_TASK_ID_NONE, job,
      &workload_id, sizeof(workload_id),
      MTAPI_NULL, 0,
      MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE,
      &status);
    MTAPI_CHECK_STATUS(status);
  }

  /* wait for the spawned tasks */
  for (ii = 0; ii < kTasks; ii++) {
    mtapi_task_wait(task[ii], MTAPI_INFINITE, &status);
    MTAPI_CHECK_STATUS(status);
  }

  /* indicate that we're done */
  embb_atomic_store_int(test, 0);
}

static void testDoSomethingElse() {
}

QueueTest::QueueTest() {
  CreateUnit("mtapi queue test").Add(&QueueTest::TestBasic, this);
}

void QueueTest::TrySimple() {
  mtapi_status_t status;
  mtapi_action_hndl_t action;
  mtapi_job_hndl_t job;
  mtapi_task_hndl_t task;
  mtapi_queue_hndl_t queue;
  const mtapi_size_t args_size = sizeof(int);
  const int args = 42;

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
  mtapi_queue_delete(queue, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);
}

void QueueTest::TryWithWait() {
  mtapi_status_t status;
  mtapi_action_hndl_t test_action, action;
  mtapi_job_hndl_t job;
  const int kTasks = 100;
  mtapi_task_hndl_t task[kTasks];
  mtapi_queue_hndl_t queue;
  int ii;
  embb_atomic_int test;
  const mtapi_size_t test_size = sizeof(embb_atomic_int);

  embb_atomic_init_int(&test, 0);

  /* create actions */
  status = MTAPI_ERR_UNKNOWN;
  test_action = mtapi_action_create(JOB_TEST_TASK, (testQueueAction),
    MTAPI_NULL, 0, MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  action = mtapi_action_create(JOB_WAIT_TASK, (testQueueWaitAction),
    MTAPI_NULL, 0, MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  MTAPI_CHECK_STATUS(status);

  /* get job */
  status = MTAPI_ERR_UNKNOWN;
  job = mtapi_job_get(JOB_WAIT_TASK, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);

  /* prepare queue attributes */
  mtapi_queue_attributes_t attr;
  mtapi_queueattr_init(&attr, &status);
  MTAPI_CHECK_STATUS(status);

  /* queue shall be ordered */
  mtapi_queueattr_set(&attr, MTAPI_QUEUE_ORDERED,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_TRUE), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  MTAPI_CHECK_STATUS(status);

  /* create queue */
  status = MTAPI_ERR_UNKNOWN;
  queue = mtapi_queue_create(QUEUE_TEST_ID, job, &attr, &status);
  MTAPI_CHECK_STATUS(status);

  /* enqueue task in queue (repeat this for all tasks that should be
     processed by that queue) */
  for (ii = 0; ii < kTasks; ii++) {
    status = MTAPI_ERR_UNKNOWN;
    task[ii] = mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue,
      reinterpret_cast<const void*>(&test), test_size,
      MTAPI_NULL, 0, MTAPI_DEFAULT_TASK_ATTRIBUTES,
      MTAPI_GROUP_NONE, &status);
    MTAPI_CHECK_STATUS(status);
  }

  /* do something else */
  testDoSomethingElse();

  /* wait for task completion */
  for (ii = 0; ii < kTasks; ii++) {
    status = MTAPI_ERR_UNKNOWN;
    mtapi_task_wait(task[ii], MTAPI_INFINITE, &status);
    MTAPI_CHECK_STATUS(status);
  }

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_delete(queue, 10, &status);
  PT_EXPECT((MTAPI_TIMEOUT == status) || (MTAPI_SUCCESS == status));
  if (MTAPI_SUCCESS != status) {
    status = MTAPI_ERR_UNKNOWN;
    mtapi_queue_delete(queue, MTAPI_INFINITE, &status);
    MTAPI_CHECK_STATUS(status);
  }

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(test_action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  embb_atomic_destroy_int(&test);
}

void QueueTest::TestBasic() {
  mtapi_status_t status;
  mtapi_info_t info;

  embb_mtapi_log_info("running testQueue...\n");

  status = MTAPI_ERR_UNKNOWN;
  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES, &info, &status);
  MTAPI_CHECK_STATUS(status);

  embb_mtapi_log_trace("mtapi successfully initialized...\n");
  embb_mtapi_log_trace(
    "hardware concurrency   : %d\n", info.hardware_concurrency);
  embb_mtapi_log_trace("used memory            : %d\n", info.used_memory);

  TrySimple();
  TryWithWait();

  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  PT_EXPECT(embb_get_bytes_allocated() == 0);

  embb_mtapi_log_info("...done\n\n");
}
