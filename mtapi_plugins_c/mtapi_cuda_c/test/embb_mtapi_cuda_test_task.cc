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

#include <embb_mtapi_cuda_test_task.h>

#include <embb/mtapi/c/mtapi_cuda.h>

#define MTAPI_CHECK_STATUS(status) \
PT_ASSERT(MTAPI_SUCCESS == status)

#define CUDA_DOMAIN 1
#define CUDA_NODE 2
#define CUDA_JOB 2

// CUDA Kernel Function for element by element vector addition
#include <embb_mtapi_cuda_test_kernel.h>

TaskTest::TaskTest() {
  CreateUnit("mtapi cuda task test").Add(&TaskTest::TestBasic, this);
}

void TaskTest::TestBasic() {
  mtapi_status_t status;
  mtapi_job_hndl_t job;
  mtapi_task_hndl_t task;
  mtapi_action_hndl_t action;

  const int kElements = 64;
  float arguments[kElements * 2];
  float results[kElements];

  for (int ii = 0; ii < kElements; ii++) {
    arguments[ii] = static_cast<float>(ii);
    arguments[ii + kElements] = static_cast<float>(ii);
  }

  mtapi_cuda_plugin_initialize(&status);
  if (status == MTAPI_ERR_FUNC_NOT_IMPLEMENTED) {
    // CUDA unavailable
    return;
  }
  MTAPI_CHECK_STATUS(status);

  mtapi_initialize(
    CUDA_DOMAIN,
    CUDA_NODE,
    MTAPI_NULL,
    MTAPI_NULL,
    &status);
  MTAPI_CHECK_STATUS(status);

  float node_local = 1.0f;
  action = mtapi_cuda_action_create(
    CUDA_JOB,
    reinterpret_cast<char const *>(imageBytes), "test", 32, 4,
    &node_local, sizeof(float),
    &status);
  MTAPI_CHECK_STATUS(status);

  status = MTAPI_ERR_UNKNOWN;
  job = mtapi_job_get(CUDA_JOB, CUDA_DOMAIN, &status);
  MTAPI_CHECK_STATUS(status);

  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,
    job,
    arguments, kElements * 2 * sizeof(float),
    results, kElements*sizeof(float),
    MTAPI_DEFAULT_TASK_ATTRIBUTES,
    MTAPI_GROUP_NONE,
    &status);
  MTAPI_CHECK_STATUS(status);

  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  for (int ii = 0; ii < kElements; ii++) {
    PT_EXPECT_EQ(results[ii], ii * 2 + 1);
  }

  mtapi_action_delete(action, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  mtapi_cuda_plugin_finalize(&status);
  MTAPI_CHECK_STATUS(status);
}
