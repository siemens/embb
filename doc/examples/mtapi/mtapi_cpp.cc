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

#include <iostream>

#include <embb/mtapi/mtapi.h>

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
static embb::mtapi::Job fibonacciJob;

static
// snippet_begin:mtapi_cpp_action_signature
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
  /* check size of arguments (in this case we only expect one int
     value)*/
  mtapi_status_t status;
  if (arg_size != sizeof(int)) {
    printf("wrong size of arguments\n");
    mtapi_context_status_set(task_context, MTAPI_ERR_ARG_SIZE, &status);
    MTAPI_CHECK_STATUS(status);
    return;
  }
  
  /* cast arguments to the desired type */
  int n = *(int*)args;
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
  /* get the node instance */
  embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
  /* calculate */
  // snippet_begin:mtapi_cpp_terminating_condition
  if (n < 2) {
    *result = n;
  } else {
  // snippet_end
    /* first recursive call spawned as task (x = fib(n - 1);) */
    // snippet_begin:mtapi_cpp_calc_task
    int a = n - 1;
    int x;
    embb::mtapi::Task task = node.Start(fibonacciJob, &a, &x);
    // snippet_end
    /* second recursive call can be called directly (y = fib(n - 2);) */
    // snippet_begin:mtapi_cpp_calc_direct
    int b = n - 2;
    int y;
    fibonacciActionFunction(
      &b, sizeof(int),
      &y, sizeof(int),
      MTAPI_NULL, 0,
      task_context);
    // snippet_end

    /* wait for completion */
    // snippet_begin:mtapi_cpp_wait_task
    mtapi_status_t task_status = task.Wait(MTAPI_INFINITE);
    if (task_status != MTAPI_SUCCESS) {
      printf("task failed with error: %d\n\n", task_status);
      exit(task_status);
    }
    // snippet_end

    /* add the two preceding numbers */
    // snippet_begin:mtapi_cpp_write_back
    *result = x + y;
    // snippet_end
  }
}

static
int fibonacci(int n) {
  // snippet_begin:mtapi_cpp_initialize
  /* initialize the node with default attributes */
  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);
  // snippet_end

  // snippet_begin:mtapi_cpp_get_node
  embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
  // snippet_end

  // snippet_begin:mtapi_cpp_register_action
  /* create action */
  embb::mtapi::Action fibonacciAction = node.CreateAction(
    FIBONACCI_JOB,                     /* action ID, defined by the
                                          application */
    (fibonacciActionFunction)          /* action function */
  );

  /* get job */
  fibonacciJob = node.GetJob(FIBONACCI_JOB, THIS_DOMAIN_ID);
  // snippet_end
  
  /* start calculation */
  // snippet_begin:mtapi_cpp_start_task
  int result;
  embb::mtapi::Task task = node.Start(fibonacciJob, &n, &result);
  // snippet_end

  /* wait for task completion */
  mtapi_status_t status = task.Wait(MTAPI_INFINITE);
  MTAPI_CHECK_STATUS(status);

  /* finalize the node */
  // snippet_begin:mtapi_cpp_finalize
  embb::mtapi::Node::Finalize();
  // snippet_end

  return result;
}

void RunMTAPI_CPP() {
  // snippet_begin:mtapi_cpp_main
  EMBB_TRY {
    int result = fibonacci(6);
    std::cout << "result: " << result << std::endl;
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    std::cout << "MTAPI error occured." << std::endl;
  }
  // snippet_end
}
