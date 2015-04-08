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

#include <dataflow_cpp_test_simple.h>

#include <iostream>
#include <sstream>

#include <embb/tasks/tasks.h>

#include <embb/base/function.h>
#include <embb/base/c/memory_allocation.h>

#include <embb/dataflow/dataflow.h>

#define NUM_SLICES 8
#define TEST_COUNT 12

typedef embb::dataflow::Network<8> MyNetwork;
typedef MyNetwork::ConstantSource< int > MyConstantSource;
typedef MyNetwork::Source< int > MySource;
typedef MyNetwork::SerialProcess< MyNetwork::Inputs<int>::Type,
  MyNetwork::Outputs<bool>::Type > MyPred;
typedef MyNetwork::ParallelProcess< MyNetwork::Inputs<int>::Type,
  MyNetwork::Outputs<int>::Type > MyFilter;
typedef MyNetwork::ParallelProcess< MyNetwork::Inputs<int, int>::Type,
  MyNetwork::Outputs<int>::Type > MyMult;
typedef MyNetwork::Sink< int > MySink;
typedef MyNetwork::Switch< int > MySwitch;
typedef MyNetwork::Select< int > MySelect;

embb::base::Atomic<int> source_counter;
int source_array[TEST_COUNT];

bool sourceFunc(int & out) {
  out = source_counter;

  source_array[source_counter] = out;
  source_counter++;

  return source_counter < TEST_COUNT;
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

SimpleTest::SimpleTest() {
  CreateUnit("dataflow_cpp simple test").Add(&SimpleTest::TestBasic, this);
}

#define MTAPI_DOMAIN_ID 1
#define MTAPI_NODE_ID 1

void SimpleTest::TestBasic() {
  // All available cores
  embb::base::CoreSet core_set(true);
  unsigned int num_cores = core_set.Count();
  embb::tasks::Node::Initialize(
    MTAPI_DOMAIN_ID,
    MTAPI_NODE_ID,
    core_set,
    1024, // max tasks (default: 1024)
    128,  // max groups (default: 128)
    // Currently needs to be initialized
    // with (max_queues + 1), see defect embb449
    num_cores + 1, // max queues (default: 16)
    1024, // queue capacity (default: 1024)
    4);   // num priorities (default: 4)

  for (int ii = 0; ii < 10000; ii++) {
    ArraySink<TEST_COUNT> asink;
    MyNetwork network;
    MyConstantSource constant(4);
    MySource source(embb::base::MakeFunction(sourceFunc));
    MyFilter filter(embb::base::MakeFunction(filterFunc));
    MyMult mult(embb::base::MakeFunction(multFunc));
    MySink sink(embb::base::MakeFunction(asink, &ArraySink<TEST_COUNT>::Run));
    MyPred pred(embb::base::MakeFunction(predFunc));
    MySwitch sw;
    MySelect sel;

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

    source.GetOutput<0>() >> pred.GetInput<0>();
    pred.GetOutput<0>() >> sw.GetInput<0>();
    pred.GetOutput<0>() >> sel.GetInput<0>();

    sw.GetOutput<0>() >> filter.GetInput<0>();
    filter.GetOutput<0>() >> sel.GetInput<1>();

    constant.GetOutput<0>() >> mult.GetInput<0>();
    sw.GetOutput<1>() >> mult.GetInput<1>();
    mult.GetOutput<0>() >> sel.GetInput<2>();

    sel.GetOutput<0>() >> sink.GetInput<0>();

    network.AddSource(constant);
    network.AddSource(source);

    try {
      network();
    } catch (embb::base::ErrorException & e) {
      PT_ASSERT_MSG(false, e.What());
    }

    PT_EXPECT(asink.Check());
  }

  embb::tasks::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}

