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

#include <embb_mtapi_zynq_test_task.h>

#include <embb/mtapi/c/mtapi_zynq.h>

#define MTAPI_CHECK_STATUS(status) \
PT_ASSERT(MTAPI_SUCCESS == status)

#define ZYNQ_DOMAIN 1
#define ZYNQ_NODE 2
#define ZYNQ_JOB 2

//debugging
#include <iostream>

TaskTest::TaskTest() {
  CreateUnit("mtapi zynq task test").Add(&TaskTest::TestBasic, this);
}

void TaskTest::TestBasic() {
  mtapi_status_t status;
  mtapi_job_hndl_t job;
  mtapi_task_hndl_t task;
  mtapi_action_hndl_t action;

  uint32_t arguments[2];
  uint32_t result_buffer[1];
  uint32_t node_local_data[1];

  arguments[0] = 1;
  arguments[1] = 2;
  node_local_data[0] = 3;
  result_buffer[0] = 0;
 
  char name[9] = "UIO_test";
  std::cout << "Plugin initialize" << std::endl;
  mtapi_zynq_plugin_initialize(name, &status);
  if (status == MTAPI_ERR_FUNC_NOT_IMPLEMENTED) {
    // Zynq unavailable
    return;
  }
  MTAPI_CHECK_STATUS(status);

  mtapi_initialize(
    ZYNQ_DOMAIN,
    ZYNQ_NODE,
    MTAPI_NULL,
    MTAPI_NULL,
    &status);
  MTAPI_CHECK_STATUS(status);
  
  
  // Use /dev/mem for memory mapping
  action = mtapi_zynq_action_create(
    ZYNQ_JOB,
    MTAPI_NULL,
    0xa0010000,
    node_local_data, 1 *sizeof(uint32_t),
    0,
    &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  job = mtapi_job_get(ZYNQ_JOB, ZYNQ_DOMAIN, &status);
  MTAPI_CHECK_STATUS(status);
  
  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,
    job,
    arguments, 2 * sizeof(uint32_t),
    result_buffer, 1 *sizeof(uint32_t),
    MTAPI_DEFAULT_TASK_ATTRIBUTES,
    MTAPI_GROUP_NONE,
    &status);
  MTAPI_CHECK_STATUS(status);

  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);
  
  PT_EXPECT_EQ(result_buffer[0], (uint32_t) 6);
  std::cout << "Result: " << result_buffer[0] << std::endl;

  mtapi_action_delete(action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);


  // Use UIO for memory mapping
  action = mtapi_zynq_action_create(
    ZYNQ_JOB,
    name,
    0,
    node_local_data, 1 *sizeof(uint32_t),
    0,
    &status);
  MTAPI_CHECK_STATUS(status);
  
  status = MTAPI_ERR_UNKNOWN;
  job = mtapi_job_get(ZYNQ_JOB, ZYNQ_DOMAIN, &status);
  MTAPI_CHECK_STATUS(status);
  
  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,
    job,
    arguments, 2 * sizeof(uint32_t),
    result_buffer, 1 *sizeof(uint32_t),
    MTAPI_DEFAULT_TASK_ATTRIBUTES,
    MTAPI_GROUP_NONE,
    &status);
  MTAPI_CHECK_STATUS(status);

  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);
  
  PT_EXPECT_EQ(result_buffer[0], (uint32_t) 6);
  std::cout << "Result: " << result_buffer[0] << std::endl;

  mtapi_action_delete(action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);
  
  mtapi_zynq_plugin_finalize(&status);
  MTAPI_CHECK_STATUS(status);
  
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);
}
