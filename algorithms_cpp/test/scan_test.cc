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

#include <scan_test.h>
#include <embb/algorithms/scan.h>
#include <vector>
#include <deque>
#include <functional>

/**
 * Functor to compute the square of a number.
 */
struct Square {
  template<typename Type>
  Type operator()(Type& l) const {
    return l * l;
  }
};

static int SquareFunction(int &val) {
  return val * val;
}

static int AddFunction(int lhs, int rhs) {
  return lhs + rhs;
}

#define TRANSFORMATION_JOB 17
#define REDUCTION_JOB 18

static void SquareActionFunction(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/) {
  typedef struct {
    int in;
  } InT;
  typedef struct {
    int out;
  } OutT;
  PT_EXPECT_EQ(args_size, sizeof(InT));
  PT_EXPECT_EQ(result_buffer_size, sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  outputs->out = inputs->in * inputs->in;
}

static void AddActionFunction(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/) {
  typedef struct {
    int in1;
    int in2;
  } InT;
  typedef struct {
    int out;
  } OutT;
  PT_EXPECT_EQ(args_size, sizeof(InT));
  PT_EXPECT_EQ(result_buffer_size, sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  outputs->out = inputs->in1 + inputs->in2;
}

ScanTest::ScanTest() {
  CreateUnit("Different data structures")
      .Add(&ScanTest::TestDataStructures, this);
  CreateUnit("Transform").Add(&ScanTest::TestTransform, this);
  CreateUnit("Function Pointers").Add(&ScanTest::TestFunctionPointers, this);
  CreateUnit("Heterogeneous").Add(&ScanTest::TestHeterogeneous, this);
  CreateUnit("Ranges").Add(&ScanTest::TestRanges, this);
  CreateUnit("Block sizes").Add(&ScanTest::TestBlockSizes, this);
  CreateUnit("Policies").Add(&ScanTest::TestPolicy, this);
  CreateUnit("Stress test").Add(&ScanTest::StressTest, this);
}

void ScanTest::TestDataStructures() {
  using embb::algorithms::Scan;

  int array[kCountSize];
  std::vector<double> vector(kCountSize);
  std::deque<int> deque(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    array[i] = static_cast<int>(i+2);
    vector[i] = static_cast<int>(i+2);
    deque[i] = static_cast<int>(i+2);
  }

  int outputArray[kCountSize];
  std::vector<double> outputVector(kCountSize);
  std::deque<int> outputDeque(kCountSize);

  Scan(array, array + kCountSize, outputArray, 0, std::plus<int>());
  Scan(vector.begin(), vector.end(), outputVector.begin(),
      static_cast<double>(0), std::plus<double>());
  Scan(deque.begin(), deque.end(), outputDeque.begin(), 0, std::plus<int>());

  int expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += array[i];

    PT_EXPECT_EQ(expected, outputArray[i]);
    PT_EXPECT_EQ(expected, outputVector[i]);
    PT_EXPECT_EQ(expected, outputDeque[i]);
  }
}

void ScanTest::TestTransform() {
  using embb::algorithms::Scan;

  std::vector<int> vector(kCountSize);
  std::vector<int> outputVector(kCountSize);

  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i+2);
  }
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, std::plus<int>(),
       Square());

  int expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
     expected += vector[i] * vector[i];
     PT_EXPECT_EQ(expected, outputVector[i]);
  }
}

void ScanTest::TestFunctionPointers() {
  using embb::algorithms::Scan;

  std::vector<int> vector(kCountSize);
  std::vector<int> init(kCountSize);
  std::vector<int> outputVector(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i+2);
    init[i] = 0;
  }

  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, &AddFunction);
  int expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, &AddFunction,
       &SquareFunction);
  expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i] * vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, &AddFunction,
       Square());
  expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i] * vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, std::plus<int>(),
       &SquareFunction);
  expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i] * vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }
}

void ScanTest::TestHeterogeneous() {
  using embb::algorithms::Scan;

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Action reduce_action = node.CreateAction(
    REDUCTION_JOB, AddActionFunction);
  embb::mtapi::Job reduce_job = node.GetJob(REDUCTION_JOB);
  embb::mtapi::Action transform_action = node.CreateAction(
    TRANSFORMATION_JOB, SquareActionFunction);
  embb::mtapi::Job transform_job = node.GetJob(TRANSFORMATION_JOB);

  std::vector<int> vector(kCountSize);
  std::vector<int> init(kCountSize);
  std::vector<int> outputVector(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i+2);
    init[i] = 0;
  }

  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, reduce_job);
  int expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, reduce_job,
       transform_job);
  expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i] * vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, reduce_job,
       Square());
  expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i] * vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, std::plus<int>(),
       transform_job);
  expected = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    expected += vector[i] * vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  reduce_action.Delete();
  transform_action.Delete();
}

void ScanTest::TestRanges() {
  using embb::algorithms::Scan;
  using embb::algorithms::Identity;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  std::vector<int> outputVector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
  }
  vector = init;

  // Omit first element
  outputVector = init;
  Scan(vector.begin() + 1, vector.end(), outputVector.begin() + 1,
       0, std::plus<int>());
  PT_EXPECT_EQ(outputVector[0], vector[0]);
  int expected = 0;
  for (size_t i = 1; i < count; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  // Omit last element
  outputVector = init;
  Scan(vector.begin(), vector.end() - 1, outputVector.begin(), 0,
       std::plus<int>());
  expected = 0;
  for (size_t i = 0; i < count - 1; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }
  PT_EXPECT_EQ(outputVector[count - 1], vector[count - 1]);

  // Omit first and last element
  outputVector = init;
  Scan(vector.begin() + 1, vector.end() - 1, outputVector.begin() + 1, 0,
       std::plus<int>());
  PT_EXPECT_EQ(outputVector[0], vector[0]);
  expected = 0;
  for (size_t i = 1; i < count - 1; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }
  PT_EXPECT_EQ(outputVector[count - 1], vector[count - 1]);

  // Only do first element
  outputVector = init;
  Scan(vector.begin(), vector.begin() + 1, outputVector.begin(), 0,
       std::plus<int>());
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(outputVector[i], vector[i]);
  }

  // Only do last element
  outputVector = init;
  Scan(vector.end() - 1, vector.end(), outputVector.end() - 1, 0,
       std::plus<int>());
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(outputVector[i], vector[i]);
  }

  // Only do second element
  outputVector = init;
  Scan(vector.begin() + 1, vector.begin() + 2, outputVector.begin() + 1, 0,
       std::plus<int>());
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(outputVector[i], vector[i]);
  }
}

void ScanTest::TestBlockSizes() {
  using embb::algorithms::Scan;
  using embb::mtapi::ExecutionPolicy;
  using embb::algorithms::Identity;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  std::vector<int> outputVector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
  }
  vector = init;

  for (size_t block_size = 1; block_size < count + 2; block_size++) {
    outputVector = init;
    Scan(vector.begin(), vector.end(), outputVector.begin(), 0,
         std::plus<int>(), Identity(), ExecutionPolicy(), block_size);
    int expected = 0;
    for (size_t i = 0; i < count; i++) {
      expected += vector[i];
      PT_EXPECT_EQ(expected, outputVector[i]);
    }
  }
}

void ScanTest::TestPolicy() {
  using embb::algorithms::Scan;
  using embb::mtapi::ExecutionPolicy;
  using embb::algorithms::Identity;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  std::vector<int> outputVector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
  }
  vector = init;

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, std::plus<int>(),
    Identity(), ExecutionPolicy());
  int expected = 0;
  for (size_t i = 0; i < count; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, std::plus<int>(),
    Identity(), ExecutionPolicy(true));
  expected = 0;
  for (size_t i = 0; i < count; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }

  outputVector = init;
  Scan(vector.begin(), vector.end(), outputVector.begin(), 0, std::plus<int>(),
    Identity(), ExecutionPolicy(true, 1));
  expected = 0;
  for (size_t i = 0; i < count; i++) {
    expected += vector[i];
    PT_EXPECT_EQ(expected, outputVector[i]);
  }
  // Empty list should not throw and not change output:
  outputVector = init;
  std::vector<int>::iterator out_it = outputVector.begin();
  Scan(vector.begin(), vector.begin(), out_it, 0, std::plus<int>());
  PT_EXPECT(out_it == outputVector.begin());

#ifdef EMBB_USE_EXCEPTIONS
  bool empty_core_set_thrown = false;
  try {
    Scan(vector.begin(), vector.end(), outputVector.begin(),
         0, std::plus<int>(), Identity(),
         ExecutionPolicy(false));
  }
  catch (embb::base::ErrorException &) {
    empty_core_set_thrown = true;
  }
  PT_EXPECT_MSG(empty_core_set_thrown,
    "Empty core set should throw ErrorException");
  bool negative_range_thrown = false;
  try {
    std::vector<int>::iterator second = vector.begin() + 1;
    Scan(second, vector.begin(), outputVector.begin(), 0, std::plus<int>());
  }
  catch (embb::base::ErrorException &) {
    negative_range_thrown = true;
  }
  PT_EXPECT_MSG(negative_range_thrown,
    "Negative range should throw ErrorException");
#endif
}

void ScanTest::StressTest() {
  using embb::algorithms::Scan;
  using embb::algorithms::Identity;
  using embb::mtapi::ExecutionPolicy;
  size_t count = embb::mtapi::Node::GetInstance().GetCoreCount() *10;
  std::vector<int> large_vector(count);
  std::vector<int> large_vector_output(count);
  for (size_t i = 0; i < count; i++) {
    large_vector[i] = static_cast<int>((i + 2) % 1000);
  }
  Scan(large_vector.begin(), large_vector.end(), large_vector_output.begin(), 0,
       std::plus<int>(), Identity(), ExecutionPolicy(), 2000);
  int expected = 0;
  for (size_t i = 0; i < count; i++) {
    expected += large_vector[i];
    PT_EXPECT_EQ(expected, large_vector_output[i]);
  }
}
