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
#include <embb/base/c/thread.h>
#include <embb/base/c/memory_allocation.h>

#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_group.h>

#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23

struct result_example_struct {
  mtapi_uint_t value1;
  mtapi_uint_t value2;
};

typedef struct result_example_struct result_example_t;

static void testGroupAction(
  const void* args,
  mtapi_size_t /*arg_size*/,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* task_context) {
  result_example_t * result =
    reinterpret_cast<result_example_t*>(result_buffer);
  mtapi_uint_t workload_id = *reinterpret_cast<const mtapi_uint_t*>(args);
  int ii;
  mtapi_uint_t core_num = mtapi_context_corenum_get(task_context, MTAPI_NULL);
  srand(core_num);
  for (ii = 10; ii < 10 + rand() % 100; ii++) {
    embb_thread_yield();
  }
  embb_mtapi_log_info(
    "testGroupAction %d called from worker %d...\n", workload_id, core_num);
  if (MTAPI_NULL != result_buffer &&
    sizeof(result_example_t) == result_buffer_size) {
    result->value1 = workload_id;
    result->value2 = core_num;
  }
}

static void testDoSomethingElse() {
}

GroupTest::GroupTest() {
#ifdef EMBB_THREADING_ANALYSIS_MODE
  const int iterations(10);
#else
  const int iterations(100);
#endif
  CreateUnit("mtapi group test").
  Add(&GroupTest::TestBasic, this, 1, iterations);
}

void GroupTest::TestBasic() {
  mtapi_info_t info;
  mtapi_status_t status = MTAPI_ERR_UNKNOWN;
  mtapi_action_hndl_t action;
  mtapi_job_hndl_t job;
  mtapi_task_attributes_t task_attributes;
  mtapi_group_hndl_t group;
#define NUM_TASKS 10
  int argument[NUM_TASKS];
  const mtapi_size_t args_size = sizeof(int);
  const mtapi_boolean_t att_val_true = MTAPI_TRUE;
  const mtapi_size_t bool_size = sizeof(mtapi_boolean_t);
  result_example_t results[NUM_TASKS];
  result_example_t* tmp_result;
  int ii;

  embb_mtapi_log_info("running testGroup...\n");

  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES, &info, &status);
  MTAPI_CHECK_STATUS(status);

  embb_mtapi_log_trace("mtapi successfully initialized...\n");
  embb_mtapi_log_trace(
    "hardware concurrency   : %d\n", info.hardware_concurrency);
  embb_mtapi_log_trace("used memory            : %d\n", info.used_memory);

  /* create action */
  action = mtapi_action_create(JOB_TEST_TASK, (testGroupAction),
    MTAPI_NULL, 0, MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  MTAPI_CHECK_STATUS(status);

  /* get job */
  job = mtapi_job_get(JOB_TEST_TASK, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);

  /* set task attribute DETACHED because we are not interested in task
     handles after the tasks have been started */
  mtapi_taskattr_init(&task_attributes, &status);
  MTAPI_CHECK_STATUS(status);
  mtapi_taskattr_set(&task_attributes, MTAPI_TASK_DETACHED,
    reinterpret_cast<const void*>(&att_val_true), bool_size, &status);
  MTAPI_CHECK_STATUS(status);

  /* ---- mtapi_group_wait_all test ---- */

  /* prepare group */
  group = mtapi_group_create(MTAPI_GROUP_ID_NONE,
    MTAPI_DEFAULT_GROUP_ATTRIBUTES, &status);

  /* create several tasks using the same group (in this example we use the
     same action for all of them, of course it is possible to use different
     actions for the different tasks) */
  for (ii = 0; ii < NUM_TASKS; ii++) {
    argument[ii] = ii;
    /* start task */
    mtapi_task_start(MTAPI_TASK_ID_NONE, job,
      reinterpret_cast<void*>(&argument[ii]),
      args_size, MTAPI_NULL, 0, &task_attributes, group, &status);
    MTAPI_CHECK_STATUS(status);
  }

  /* do something else */
  testDoSomethingElse();

  /* wait for completion of all tasks in the group */
  mtapi_group_wait_all(group, 10, &status);
  PT_EXPECT((MTAPI_TIMEOUT == status) || (MTAPI_SUCCESS == status));
  if (MTAPI_SUCCESS != status) {
    status = MTAPI_ERR_UNKNOWN;
    mtapi_group_wait_all(group, MTAPI_INFINITE, &status);
    MTAPI_CHECK_STATUS(status);
  }

  /* ---- mtapi_group_wait_any test ---- */

  /* prepare group */
  group = mtapi_group_create(MTAPI_GROUP_ID_NONE,
    MTAPI_DEFAULT_GROUP_ATTRIBUTES, &status);

  /* create several tasks using the same group (in this example we use the
     same action for all of them, of course it is possible to use different
     actions for the different tasks) */
  for (ii = 0; ii < NUM_TASKS; ii++) {
    argument[ii] = ii;
    /* start task */
    mtapi_task_start(MTAPI_TASK_ID_NONE, job,
      reinterpret_cast<void*>(&argument[ii]),
      args_size, &results[ii], sizeof(result_example_t),
      &task_attributes, group, &status);
    MTAPI_CHECK_STATUS(status);
  }

  /* do something else */
  testDoSomethingElse();

  /* wait for completion of tasks in the group and handle results */
  bool run = true;
  while (run) {
    mtapi_group_wait_any(group, reinterpret_cast<void**>(&tmp_result),
      10, &status);
    if (status == MTAPI_TIMEOUT) {
      mtapi_group_wait_any(group, reinterpret_cast<void**>(&tmp_result),
        MTAPI_INFINITE, &status);
    }
    if (status == MTAPI_GROUP_COMPLETED) {
      run = false;
    } else if (status == MTAPI_SUCCESS) {
      /* ... process 'tmp_result' here ...
      temp_result contains the pointer that was passed at mtapi_task_start
      to the task which just returned */
      embb_mtapi_log_trace(
        "result.value1 = %i, result.value2 = %i\n",
        tmp_result->value1, tmp_result->value2);
    } else {
      /* status will be MTAPI_ERR_RESULT_SIZE on result size mismatch */
      run = false;
    }
  }
  PT_EXPECT(MTAPI_GROUP_COMPLETED == status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  PT_EXPECT(embb_get_bytes_allocated() == 0);

  embb_mtapi_log_info("...done\n\n");
}
