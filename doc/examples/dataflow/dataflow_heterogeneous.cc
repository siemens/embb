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
#include <embb/dataflow/dataflow.h>

static void Generate(
  const void* /*args*/,
  mtapi_size_t /*args_size*/,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  EMBB_UNUSED_IN_RELEASE(result_buffer_size);
  // snippet_begin:source_output_struct_int
  typedef struct {
    bool more;
    int out;
  } OutT;
  // snippet_end
  assert(result_buffer_size == sizeof(OutT));
  // snippet_begin:cast_output
  OutT * outputs = static_cast<OutT *>(result_buffer);
  // snippet_end
  // snippet_begin:source_action_body
  static int value = 0;
  outputs->out = value;
  outputs->more = value < 10;
  value++;
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
  // snippet_begin:input_struct_int
  typedef struct {
    int val;
  } InT;
  // snippet_end
  // snippet_begin:output_struct_int
  typedef struct {
    int out;
  } OutT;
  // snippet_end
  assert(args_size == sizeof(InT));
  assert(result_buffer_size == sizeof(OutT));
  // snippet_begin:cast_input
  InT const * inputs = static_cast<InT const *>(args);
  // snippet_end
  OutT * outputs = static_cast<OutT *>(result_buffer);
  // snippet_begin:double_action_body
  outputs->out = inputs->val * 2;
  // snippet_end
}

// snippet_begin:sink_result
static int accumulated_result = 0;
// snippet_end

static void Accumulate(
  const void* args,
  mtapi_size_t args_size,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/
) {
  EMBB_UNUSED_IN_RELEASE(args_size);
  typedef struct {
    int val;
  } InT;
  assert(args_size == sizeof(InT));
  InT const * inputs = static_cast<InT const *>(args);
  // snippet_begin:sink_action_body
  accumulated_result += inputs->val;
  // snippet_end
}

void RunDataflowHeterogeneous() {
  // snippet_begin:get_node
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  // snippet_end

  // snippet_begin:get_jobs
  static const mtapi_job_id_t JOB_GENERATE = 10;
  embb::mtapi::Job job_generate = node.GetJob(JOB_GENERATE);

  static const mtapi_job_id_t JOB_DOUBLE = 11;
  embb::mtapi::Job job_double = node.GetJob(JOB_DOUBLE);

  static const mtapi_job_id_t JOB_ACCUMULATE = 12;
  embb::mtapi::Job job_accumulate = node.GetJob(JOB_ACCUMULATE);
  // snippet_end

  embb::mtapi::Action action_generate =
    node.CreateAction(JOB_GENERATE, Generate);
  embb::mtapi::Action action_double = node.CreateAction(JOB_DOUBLE, Double);
  embb::mtapi::Action action_accumulate =
    node.CreateAction(JOB_ACCUMULATE, Accumulate);

  // snippet_begin:define_net
  typedef embb::dataflow::Network Net;
  Net net(4);

  Net::Source<int> source(net, job_generate);

  Net::ParallelProcess<Net::Inputs<int>, Net::Outputs<int> >
    filter(net, job_double);

  Net::Sink<int> sink(net, job_accumulate);
  // snippet_end

  // snippet_begin:net_connect
  source >> filter >> sink;
  // snippet_end

  // snippet_begin:net_run
  net();
  // snippet_end

  assert(accumulated_result == 90);

  action_generate.Delete();
  action_double.Delete();
  action_accumulate.Delete();
}
