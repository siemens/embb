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

#include <dataflow_cpp_test_simple.h>

#include <iostream>
#include <sstream>

#include <embb/mtapi/mtapi.h>

#include <embb/base/function.h>
#include <embb/base/c/memory_allocation.h>

#include <embb/dataflow/dataflow.h>

#define NUM_SLICES 8
#define TEST_COUNT 12

typedef embb::dataflow::Network MyNetwork;
typedef MyNetwork::ConstantSource< int > MyConstantSource;
typedef MyNetwork::Source< int > MySource;
typedef MyNetwork::SerialProcess< MyNetwork::Inputs<int>,
  MyNetwork::Outputs<bool> > MyPred;
typedef MyNetwork::ParallelProcess< MyNetwork::Inputs<int>,
  MyNetwork::Outputs<int> > MyFilter;
typedef MyNetwork::ParallelProcess< MyNetwork::Inputs<int, int>,
  MyNetwork::Outputs<int> > MyMult;
typedef MyNetwork::Sink< int > MySink;
typedef MyNetwork::Switch< int > MySwitch;
typedef MyNetwork::Select< int > MySelect;

embb::base::Atomic<int> source_counter;
int source_array[TEST_COUNT];

bool sourceFunc(int & out) {
  if (source_counter < TEST_COUNT) {
    out = source_counter;

    source_array[source_counter] = out;
    source_counter++;

    return true;
  } else {
    return false;
  }
}

embb::base::Atomic<int> pred_counter;
bool pred_array[TEST_COUNT];

void predFunc(int const & in, bool & out) {
  out = (0 == (in % 2));

  pred_array[pred_counter] = out;
  pred_counter++;
}

embb::base::Atomic<int> filter_counter;
int filter_array[TEST_COUNT];

void filterFunc(int const &in, int & out) {
  out = in + 1;

  filter_array[filter_counter] = out;
  filter_counter++;
}

embb::base::Atomic<int> mult_counter;
int mult_array[TEST_COUNT];

void multFunc(int const & in_a, int const & in_b, int & out) {
  out = in_a * in_b;

  mult_array[mult_counter] = out;
  mult_counter++;
}

template <int SIZE>
class ArraySink {
 private:
  int values_[SIZE];
  int pos_;

 public:
  ArraySink() {
    Init();
  }

  void Print() const {
    std::cout << values_[0];
    for (int ii = 1; ii < SIZE; ii++) {
      std::cout << ", " << values_[ii];
    }
    std::cout << std::endl;
  }

  void Init() {
    for (int ii = 0; ii < SIZE; ii++) {
      values_[ii] = -1;
    }
    pos_ = 0;
  }

  bool Check() const {
    for (int ii = 0; ii < SIZE; ii++) {
      int expected;
      if (0 == (ii % 2))
        expected = ii + 1;
      else
        expected = ii * 4;
      if (values_[ii] != expected)
        return false;
    }
    return true;
  }

  void Run(int const & in) {
    values_[pos_] = in;
    pos_++;
  }
};

#define SOURCE_JOB 17
#define SINK_JOB 18
#define MULT_JOB 19

static void sourceAction(
  const void* /*args*/,
  mtapi_size_t /*arg_size*/,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/) {
  typedef struct {
    mtapi_boolean_t result;
    int out;
  } OutT;
  PT_EXPECT_EQ(result_buffer_size, sizeof(OutT));
  OutT * output = static_cast<OutT*>(result_buffer);
  output->result = sourceFunc(output->out) ? MTAPI_TRUE : MTAPI_FALSE;
}

static ArraySink<TEST_COUNT> * array_sink;

static void sinkAction(
  const void* args,
  mtapi_size_t arg_size,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/) {
  typedef struct {
    int in;
  } InT;
  PT_EXPECT_EQ(arg_size, sizeof(InT));
  InT const * input = static_cast<InT const *>(args);
  array_sink->Run(input->in);
}

static void multAction(
  const void* args,
  mtapi_size_t arg_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/) {
  typedef struct {
    int in1;
    int in2;
  } InT;
  typedef struct {
    int out;
  } OutT;
  PT_EXPECT_EQ(arg_size, sizeof(InT));
  PT_EXPECT_EQ(result_buffer_size, sizeof(OutT));
  InT const * input = static_cast<InT const *>(args);
  OutT * output = static_cast<OutT*>(result_buffer);
  multFunc(input->in1, input->in2, output->out);
}

SimpleTest::SimpleTest() {
  CreateUnit("dataflow_cpp simple test").Add(&SimpleTest::TestBasic, this);
}

#define MTAPI_DOMAIN_ID 1
#define MTAPI_NODE_ID 1

void SimpleTest::TrySimple(bool reuse_main_thread) {
  // All available cores
  embb::base::CoreSet core_set(true);
  embb::mtapi::NodeAttributes node_attr;
  node_attr
    .SetReuseMainThread(reuse_main_thread ? MTAPI_TRUE : MTAPI_FALSE)
    .SetCoreAffinity(core_set)
    .SetMaxQueues(2);
  embb::mtapi::Node::Initialize(
    MTAPI_DOMAIN_ID,
    MTAPI_NODE_ID,
    node_attr);

#ifdef EMBB_THREADING_ANALYSIS_MODE
  const int iterations(10);
#else
  const int iterations(1000);
#endif
  for (int ii = 0; ii < iterations; ii++) {
    ArraySink<TEST_COUNT> asink;
    MyNetwork network(NUM_SLICES);
    MyConstantSource constant(network, 4);
    MySource source(network, embb::base::MakeFunction(sourceFunc));
    MyFilter filter(network, embb::base::MakeFunction(filterFunc));
    MyMult mult(network, embb::base::MakeFunction(multFunc));
    MySink sink(network,
      embb::base::MakeFunction(asink, &ArraySink<TEST_COUNT>::Run));
    MyPred pred(network, embb::base::MakeFunction(predFunc));
    MySwitch sw(network);
    MySelect sel(network);

    for (int kk = 0; kk < TEST_COUNT; kk++) {
      source_array[kk] = -1;
      pred_array[kk] = false;
      filter_array[kk] = -1;
      mult_array[kk] = -1;
    }

    source_counter = 0;
    pred_counter = 0;
    mult_counter = 0;
    filter_counter = 0;

    filter.HasInputs();
    filter.HasOutputs();

    source.GetOutput<0>() >> sw.GetInput<1>();

    // connection chain representing the commented single connections below
    source >> pred >> sw >> filter;

    //source.GetOutput<0>() >> pred.GetInput<0>();
    //pred.GetOutput<0>() >> sw.GetInput<0>();
    pred.GetOutput<0>() >> sel.GetInput<0>();

    //sw.GetOutput<0>() >> filter.GetInput<0>();
    filter.GetOutput<0>() >> sel.GetInput<1>();

    constant.GetOutput<0>() >> mult.GetInput<0>();
    sw.GetOutput<1>() >> mult.GetInput<1>();
    mult.GetOutput<0>() >> sel.GetInput<2>();

    sel.GetOutput<0>() >> sink.GetInput<0>();

    try {
      if (!network.IsValid()) {
        EMBB_THROW(embb::base::ErrorException, "network is invalid");
      }
      network();
    } catch (embb::base::ErrorException & e) {
      PT_ASSERT_MSG(false, e.What());
    }

    PT_EXPECT(asink.Check());
  }

  embb::mtapi::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}

void SimpleTest::TryHeterogeneous() {
  embb::mtapi::NodeAttributes node_attr;
  node_attr
    .SetReuseMainThread(MTAPI_TRUE);
  embb::mtapi::Node::Initialize(
    MTAPI_DOMAIN_ID,
    MTAPI_NODE_ID,
    node_attr);

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

  embb::mtapi::Action source_action =
    node.CreateAction(SOURCE_JOB, sourceAction);
  embb::mtapi::Job source_job = node.GetJob(SOURCE_JOB);

  embb::mtapi::Action sink_action = node.CreateAction(SINK_JOB, sinkAction);
  embb::mtapi::Job sink_job = node.GetJob(SINK_JOB);

  embb::mtapi::Action mult_action = node.CreateAction(MULT_JOB, multAction);
  embb::mtapi::Job mult_job = node.GetJob(MULT_JOB);

  {
    ArraySink<TEST_COUNT> asink;
    array_sink = &asink;
    MyNetwork network(NUM_SLICES);
    MyConstantSource constant(network, 4);
    MySource source(network, source_job);
    MyFilter filter(network, embb::base::MakeFunction(filterFunc));
    MyMult mult(network, mult_job);
    MySink sink(network, sink_job);
    MyPred pred(network, embb::base::MakeFunction(predFunc));
    MySwitch sw(network);
    MySelect sel(network);

    for (int kk = 0; kk < TEST_COUNT; kk++) {
      source_array[kk] = -1;
      pred_array[kk] = false;
      filter_array[kk] = -1;
      mult_array[kk] = -1;
    }

    source_counter = 0;
    pred_counter = 0;
    mult_counter = 0;
    filter_counter = 0;

    filter.HasInputs();
    filter.HasOutputs();

    source.GetOutput<0>() >> sw.GetInput<1>();

    source >> pred >> sw >> filter;

    pred.GetOutput<0>() >> sel.GetInput<0>();

    filter.GetOutput<0>() >> sel.GetInput<1>();

    constant.GetOutput<0>() >> mult.GetInput<0>();
    sw.GetOutput<1>() >> mult.GetInput<1>();
    mult.GetOutput<0>() >> sel.GetInput<2>();

    sel.GetOutput<0>() >> sink.GetInput<0>();

    try {
      if (!network.IsValid()) {
        EMBB_THROW(embb::base::ErrorException, "network is invalid");
      }
      network();
    }
    catch (embb::base::ErrorException & e) {
      PT_ASSERT_MSG(false, e.What());
    }

    PT_EXPECT(asink.Check());
  }

  source_action.Delete();
  sink_action.Delete();
  mult_action.Delete();

  embb::mtapi::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}

void SimpleTest::TestBasic() {
  TrySimple(false);
  TrySimple(true);
  TryHeterogeneous();
}
