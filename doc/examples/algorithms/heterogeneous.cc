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

#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/algorithms.h>
#include <embb/base/c/internal/unused.h>
#include <vector>
#include <cassert>

// snippet_begin:invoke_action
static int a = 0;

static void InvokeA(
  const void* /*args*/,
  mtapi_size_t /*args_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  a++;
}
// snippet_end

static int b = 0;

static void InvokeB(
  const void* /*args*/,
  mtapi_size_t /*args_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  b++;
}

static void DescendingCompare(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  EMBB_UNUSED_IN_RELEASE(args_size);
  EMBB_UNUSED_IN_RELEASE(result_buffer_size);
  // snippet_begin:struct_input_lhs_rhs
  typedef struct {
    int lhs;
    int rhs;
  } InT;
  // snippet_end
  // snippet_begin:struct_output_bool
  typedef struct {
    bool out;
  } OutT;
  // snippet_end
  assert(args_size == sizeof(InT));
  assert(result_buffer_size == sizeof(OutT));
  // snippet_begin:cast_parameters
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  // snippet_end
  // snippet_begin:sort_action_body
  outputs->out = inputs->lhs > inputs->rhs;
  // snippet_end
}

static void CheckZero(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  EMBB_UNUSED_IN_RELEASE(args_size);
  EMBB_UNUSED_IN_RELEASE(result_buffer_size);
  // snippet_begin:struct_input_val
  typedef struct {
    int val;
  } InT;
  // snippet_end
  typedef struct {
    bool out;
  } OutT;
  assert(args_size == sizeof(InT));
  assert(result_buffer_size == sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  // snippet_begin:count_action_body
  outputs->out = inputs->val == 0;
  // snippet_end
}

static void Double(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  EMBB_UNUSED_IN_RELEASE(args_size);
  EMBB_UNUSED_IN_RELEASE(result_buffer_size);
  typedef struct {
    int val;
  } InT;
  // snippet_begin:struct_output_int
  typedef struct {
    int out;
  } OutT;
  // snippet_end
  assert(args_size == sizeof(InT));
  assert(result_buffer_size == sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  // snippet_begin:double_action_body
  outputs->out = inputs->val * 2;
  // snippet_end
}

static void Add(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  EMBB_UNUSED_IN_RELEASE(args_size);
  EMBB_UNUSED_IN_RELEASE(result_buffer_size);
  typedef struct {
    int lhs;
    int rhs;
  } InT;
  typedef struct {
    int out;
  } OutT;
  assert(args_size == sizeof(InT));
  assert(result_buffer_size == sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  // snippet_begin:add_action_body
  outputs->out = inputs->lhs + inputs->rhs;
  // snippet_end
}

/**
 * Example demonstrating the use of the algorithms in heterogeneous systems.
 */
void RunHeterogeneous() {
  // snippet_begin:get_node
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  // snippet_end

  // invoke
  {
    // snippet_begin:invoke_jobs
    static const mtapi_job_id_t JOB_INVOKE_A = 10;
    embb::mtapi::Job job_a = node.GetJob(JOB_INVOKE_A);

    static const mtapi_job_id_t JOB_INVOKE_B = 11;
    embb::mtapi::Job job_b = node.GetJob(JOB_INVOKE_B);
    // snippet_end

    embb::mtapi::Action action_a = node.CreateAction(JOB_INVOKE_A, InvokeA);
    embb::mtapi::Action action_b = node.CreateAction(JOB_INVOKE_B, InvokeB);

    // snippet_begin:invoke_call
    embb::algorithms::Invoke(job_a, job_b);
    // snippet_end

    assert((a == 1) && (b == 1));

    action_a.Delete();
    action_b.Delete();
  }

  // quick sort
  {
    // snippet_begin:sort_job
    static const mtapi_job_id_t JOB_COMPARE = 10;
    embb::mtapi::Job job_compare = node.GetJob(JOB_COMPARE);
    // snippet_end

    embb::mtapi::Action action_compare =
      node.CreateAction(JOB_COMPARE, DescendingCompare);

    // snippet_begin:sort_data
    static const size_t kCountSize = 10;
    std::vector<int> vector(kCountSize);
    for (size_t i = 0; i < kCountSize; i++) {
      vector[i] = static_cast<int>(i + 2);
    }
    // snippet_end

    // snippet_begin:sort_call
    embb::algorithms::QuickSort(vector.begin(), vector.end(), job_compare);
    // snippet_end

    bool correct = true;
    for (size_t i = 0; i < kCountSize-1; i++) {
      correct = correct & (vector[i] >= vector[i + 1]);
    }
    assert(correct);

    action_compare.Delete();
  }

  // count if zero
  {
    // snippet_begin:count_job
    static const mtapi_job_id_t JOB_CHECK_ZERO = 10;
    embb::mtapi::Job job_check_zero = node.GetJob(JOB_CHECK_ZERO);
    // snippet_end

    embb::mtapi::Action action_check_zero =
      node.CreateAction(JOB_CHECK_ZERO, CheckZero);

    // snippet_begin:count_data
    static const size_t kCountSize = 10;
    std::vector<int> vector(kCountSize);
    for (size_t i = 0; i < kCountSize; i++) {
      vector[i] = int(i) % 2;
    }
    // snippet_end

    // snippet_begin:count_call
    std::vector<int>::iterator::difference_type count =
      embb::algorithms::CountIf(vector.begin(), vector.end(), job_check_zero);
    // snippet_end

    EMBB_UNUSED_IN_RELEASE(count);
    assert(count == 5);

    action_check_zero.Delete();
  }

  // for each
  {
    // snippet_begin:foreach_job
    static const mtapi_job_id_t JOB_DOUBLE = 10;
    embb::mtapi::Job job_double = node.GetJob(JOB_DOUBLE);
    // snippet_end

    embb::mtapi::Action action_double = node.CreateAction(JOB_DOUBLE, Double);

    static const size_t kCountSize = 10;
    std::vector<int> vector(kCountSize);
    for (size_t i = 0; i < kCountSize; i++) {
      vector[i] = int(i);
    }

    // snippet_begin:foreach_call
    embb::algorithms::ForEach(vector.begin(), vector.end(), job_double);
    // snippet_end

    bool correct = true;
    for (size_t i = 0; i < kCountSize - 1; i++) {
      correct = correct & (vector[i] == int(i) * 2);
    }
    assert(correct);

    action_double.Delete();
  }

  // reduce and scan
  {
    // snippet_begin:reduce_jobs
    static const mtapi_job_id_t JOB_DOUBLE = 10;
    embb::mtapi::Job job_double = node.GetJob(JOB_DOUBLE);

    static const mtapi_job_id_t JOB_ADD = 11;
    embb::mtapi::Job job_add = node.GetJob(JOB_ADD);
    // snippet_end

    embb::mtapi::Action action_double = node.CreateAction(JOB_DOUBLE, Double);
    embb::mtapi::Action action_add =
      node.CreateAction(JOB_ADD, Add);

    // snippet_begin:reduce_data
    static const size_t kCountSize = 10;
    std::vector<int> vector(kCountSize);
    for (size_t i = 0; i < kCountSize; i++) {
      vector[i] = int(i);
    }
    // snippet_end

    // snippet_begin:reduce_call
    int result =
      embb::algorithms::Reduce(vector.begin(), vector.end(), 0,
        job_add, job_double);
    // snippet_end

    EMBB_UNUSED_IN_RELEASE(result);
    assert(result == kCountSize * (kCountSize - 1));

    // snippet_begin:scan_call
    std::vector<int> output(kCountSize);
    embb::algorithms::Scan(vector.begin(), vector.end(), output.begin(), 0,
      job_add, job_double);
    // snippet_end

    bool correct = true;
    int sum = 0;
    for (size_t i = 0; i < kCountSize - 1; i++) {
      sum += vector[i] * 2;
      correct = correct & (output[i] == sum);
    }
    assert(correct);

    action_double.Delete();
    action_add.Delete();
  }
}
