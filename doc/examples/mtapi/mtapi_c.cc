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

#include <stdio.h>
#include <embb/mtapi/c/mtapi.h>

#include <stdlib.h>
#include <stdio.h>
#include <embb/base/c/log.h>
#define MTAPI_CHECK_STATUS(status) \
if (MTAPI_SUCCESS != status) { \
  printf("...error %d\n\n", status); \
  exit(status); \
}
#define THIS_DOMAIN_ID 1
#define THIS_NODE_ID 1

#define FIBONACCI_JOB 1
static mtapi_job_hndl_t fibonacciJob;

static
// snippet_begin:mtapi_c_action_signature
void fibonacciActionFunction(
  const void* args,
  mtapi_size_t arg_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* task_context
  ) {
// snippet_end
  // snippet_begin:mtapi_c_validate_arguments
  /* check size of arguments (in this case we only expect one int
     value) */
  mtapi_status_t status;
  if (arg_size != sizeof(int)) {
    printf("wrong size of arguments\n");
    mtapi_context_status_set(task_context, MTAPI_ERR_ARG_SIZE,
                             &status);
    MTAPI_CHECK_STATUS(status);
    return;
  }

  /* cast arguments to the desired type */
  int n = *(int*)args;
  // snippet_end

  // snippet_begin:mtapi_c_validate_result_buffer
  /* if the caller is not interested in results, result_buffer may be
     MTAPI_NULL. Of course, this depends on the application */
  int* result = MTAPI_NULL;
  if (result_buffer == MTAPI_NULL) {
    mtapi_context_status_set(
      task_context, MTAPI_ERR_RESULT_SIZE, &status);
    MTAPI_CHECK_STATUS(status);
  } else {
    /* if results are expected by the caller, check result buffer
       size... */
    if (result_buffer_size == sizeof(int)) {
      /* ... and cast the result buffer */
      result = (int*)result_buffer;
    } else {
      printf("wrong size of result buffer\n");
      mtapi_context_status_set(
        task_context, MTAPI_ERR_RESULT_SIZE, &status);
      MTAPI_CHECK_STATUS(status);
      return;
    }
  }
  // snippet_end

  /* calculate */
  // snippet_begin:mtapi_c_terminating_condition
  if (n < 2) {
    *result = n;
  } else {
  // snippet_end
    /* first recursive call spawned as task (x = fib(n - 1);) */
    // snippet_begin:mtapi_c_calc_task
    int a = n - 1;
    int x;
    mtapi_task_hndl_t task = mtapi_task_start(
      MTAPI_TASK_ID_NONE,              /* optional task ID */
      fibonacciJob,                    /* job */
      (void*)&a,                       /* arguments passed to action
                                          functions */
      sizeof(int),                     /* size of arguments */
      (void*)&x,                       /* result buffer */
      sizeof(int),                     /* size of result buffer */
      MTAPI_DEFAULT_TASK_ATTRIBUTES,   /* task attributes */
      MTAPI_GROUP_NONE,                /* optional task group */
      &status                          /* status out - parameter */
    );
    MTAPI_CHECK_STATUS(status);
    // snippet_end
    /* second recursive call can be called directly (y = fib(n - 2);) */
    // snippet_begin:mtapi_c_calc_direct
    int b = n - 2;
    int y;
    fibonacciActionFunction(
      &b, sizeof(int),
      &y, sizeof(int),
      MTAPI_NULL, 0,
      task_context);
    // snippet_end
    /* wait for completion */
    // snippet_begin:mtapi_c_wait_task
    mtapi_task_wait(task, MTAPI_INFINITE, &status);
    // snippet_end
    MTAPI_CHECK_STATUS(status);
    /* add the two preceeding numbers */
    // snippet_begin:mtapi_c_write_back
    *result = x + y;
    // snippet_end
  }
}

static int fibonacci(int n) {
  // snippet_begin:mtapi_c_initialize
  mtapi_status_t status;

  /* initialize node attributes to default values */
  mtapi_node_attributes_t node_attr;
  mtapi_nodeattr_init(&node_attr, &status);
  MTAPI_CHECK_STATUS(status);

  /* set node type to SMP */
  mtapi_nodeattr_set(
    &node_attr,
    MTAPI_NODE_TYPE,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_NODE_TYPE_SMP),
    MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  MTAPI_CHECK_STATUS(status);

  /* initialize the node */
  mtapi_info_t info;
  mtapi_initialize(
    THIS_DOMAIN_ID,
    THIS_NODE_ID,
    &node_attr,
    &info,
    &status);
  MTAPI_CHECK_STATUS(status);
  // snippet_end

  // snippet_begin:mtapi_c_register_action
  /* create action */
  mtapi_action_hndl_t fibonacciAction;
  fibonacciAction = mtapi_action_create(
    FIBONACCI_JOB,                     /* action ID, defined by the
                                          application */
    (fibonacciActionFunction),         /* action function */
    MTAPI_NULL,                        /* no shared data */
    0,                                 /* length of shared data */
    MTAPI_DEFAULT_ACTION_ATTRIBUTES,   /* action attributes */
    &status                            /* status out - parameter */
  );
  MTAPI_CHECK_STATUS(status);

  /* get job */
  mtapi_task_hndl_t task;
  fibonacciJob = mtapi_job_get(FIBONACCI_JOB, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);
  // snippet_end

  // snippet_begin:mtapi_c_start_task
  /* start task */
  int result;
  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,                /* optional task ID */
    fibonacciJob,                      /* job */
    (void*)&n,                         /* arguments passed to action
                                          functions */
    sizeof(int),                       /* size of arguments */
    (void*)&result,                    /* result buffer */
    sizeof(int),                       /* size of result buffer */
    MTAPI_DEFAULT_TASK_ATTRIBUTES,     /* task attributes */
    MTAPI_GROUP_NONE,                  /* optional task group */
    &status                            /* status out - parameter */
  );
  MTAPI_CHECK_STATUS(status);
  // snippet_end

  /* wait for task completion */
  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);

  // snippet_begin:mtapi_c_finalize
  /* delete action */
  mtapi_action_delete(fibonacciAction, 100, &status);
  MTAPI_CHECK_STATUS(status);

  /* finalize the node */
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);
  // snippet_end

  return result;
}

void RunMTAPI_C() {
  /* run calculation */
  int result;
  result = fibonacci(6);

  /* print result */
  printf("result: %i\n", result);
}
