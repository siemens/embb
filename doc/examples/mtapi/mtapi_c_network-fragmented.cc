/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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
#include "mtapi/mtapi_network_c_header-snippet.h"
#include <embb/base/c/internal/unused.h>

#include "mtapi/mtapi_check_status-snippet.h"

#define NETWORK_DOMAIN 1
#define NETWORK_LOCAL_NODE 3
#define NETWORK_LOCAL_JOB 3
#define NETWORK_REMOTE_NODE 3
#define NETWORK_REMOTE_JOB 4

#include "mtapi/mtapi_network_c_action_function-snippet.h"

void RunMTAPI_C_Network() {
  mtapi_status_t status;
  mtapi_job_hndl_t job;
  mtapi_task_hndl_t task;
  mtapi_action_hndl_t network_action, local_action;

  const int kElements = 64;
  float arguments[kElements * 2];
  float results[kElements];

  for (int ii = 0; ii < kElements; ii++) {
    arguments[ii] = static_cast<float>(ii);
    arguments[ii + kElements] = static_cast<float>(ii);
  }

  mtapi_initialize(
    NETWORK_DOMAIN,
    NETWORK_LOCAL_NODE,
    MTAPI_DEFAULT_NODE_ATTRIBUTES,
    MTAPI_NULL,
    &status);
  MTAPI_CHECK_STATUS(status);

#include "mtapi/mtapi_network_c_plugin_initialize-snippet.h"
  MTAPI_CHECK_STATUS(status);

#include "mtapi/mtapi_network_c_remote_action_create-snippet.h"
  MTAPI_CHECK_STATUS(status);

#include "mtapi/mtapi_network_c_local_action_create-snippet.h"
  MTAPI_CHECK_STATUS(status);

  job = mtapi_job_get(NETWORK_LOCAL_JOB, NETWORK_DOMAIN, &status);
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
    if (results[ii] != ii * 2 + 1) {
      printf("result %d was expected to be %d but was %f.\n",
        ii, ii * 2 + 1, results[ii]);
      exit(MTAPI_ERR_UNKNOWN);
    }
  }

#include "mtapi/mtapi_network_c_plugin_finalize-snippet.h"
  MTAPI_CHECK_STATUS(status);

  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);
}
