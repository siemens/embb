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

#include "mtapi/mtapi_check_status-snippet.h"

#include "mtapi/mtapi_c_plugin_task_schedule-snippet.h"

#include "mtapi/mtapi_c_plugin_task_start_cb-snippet.h"

#include "mtapi/mtapi_c_plugin_task_cancel_cb-snippet.h"

#include "mtapi/mtapi_c_plugin_action_finalize_cb-snippet.h"

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

#include "mtapi/mtapi_c_plugin_action_create-snippet.h"
  MTAPI_CHECK_STATUS(status);

#include "mtapi/mtapi_c_plugin_get_job-snippet.h"
  MTAPI_CHECK_STATUS(status);

#include "mtapi/mtapi_c_plugin_task_start-snippet.h"
  MTAPI_CHECK_STATUS(status);

  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);
}
