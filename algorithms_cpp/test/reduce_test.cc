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

#include <reduce_test.h>
#include <embb/algorithms/reduce.h>
#include <embb/mtapi/execution_policy.h>
#include <deque>
#include <vector>
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


ReduceTest::ReduceTest() {
  CreateUnit("Different data structures")
      .Add(&ReduceTest::TestDataStructures, this);
  CreateUnit("Transform").Add(&ReduceTest::TestTransform, this);
  CreateUnit("Function Pointers").Add(&ReduceTest::TestFunctionPointers, this);
  CreateUnit("Heterogeneous").Add(&ReduceTest::TestHeterogeneous, this);
  CreateUnit("Ranges").Add(&ReduceTest::TestRanges, this);
  CreateUnit("Block sizes").Add(&ReduceTest::TestBlockSizes, this);
  CreateUnit("Policies").Add(&ReduceTest::TestPolicy, this);
  CreateUnit("Stress test").Add(&ReduceTest::StressTest, this);
}

void ReduceTest::TestDataStructures() {
  using embb::algorithms::Reduce;
  int sum = 0;
  int array[kCountSize];
  std::vector<double> vector(kCountSize);
  std::deque<int> deque(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    array[i] = static_cast<int>(i+2);
    vector[i] = static_cast<int>(i+2);
    deque[i] = static_cast<int>(i+2);
    sum += static_cast<int>(i + 2);
  }

  PT_EXPECT_EQ(Reduce(array, array + kCountSize, 0, std::plus<int>()), sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), static_cast<double>(0),
               std::plus<double>()), sum);
  PT_EXPECT_EQ(Reduce(deque.begin(), deque.end(), 0, std::plus<int>()), sum);
}

void ReduceTest::TestTransform() {
  using embb::algorithms::Reduce;
  int sum = 0;
  std::vector<int> vector(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i+2);
    sum += static_cast<int>((i + 2) * (i + 2));
  }

  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>(),
               Square()), sum);
}

void ReduceTest::TestFunctionPointers() {
  using embb::algorithms::Reduce;
  std::vector<int> vector(kCountSize);
  int sum = 0;
  int sqr_sum = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i + 2);
    sum += static_cast<int>(i + 2);
    sqr_sum += static_cast<int>((i + 2) * (i + 2));
  }
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, &AddFunction), sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, &AddFunction,
                      &SquareFunction), sqr_sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>(),
                      &SquareFunction), sqr_sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, &AddFunction,
                      Square()), sqr_sum);
}

void ReduceTest::TestHeterogeneous() {
  using embb::algorithms::Reduce;

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Action reduce_action = node.CreateAction(
    REDUCTION_JOB, AddActionFunction);
  embb::mtapi::Job reduce_job = node.GetJob(REDUCTION_JOB);
  embb::mtapi::Action transform_action = node.CreateAction(
    TRANSFORMATION_JOB, SquareActionFunction);
  embb::mtapi::Job transform_job = node.GetJob(TRANSFORMATION_JOB);

  std::vector<int> vector(kCountSize);
  int sum = 0;
  int sqr_sum = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i + 2);
    sum += static_cast<int>(i + 2);
    sqr_sum += static_cast<int>((i + 2) * (i + 2));
  }
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, reduce_job), sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, reduce_job,
    transform_job), sqr_sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>(),
    transform_job), sqr_sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, reduce_job,
    Square()), sqr_sum);

  reduce_action.Delete();
  transform_action.Delete();
}

void ReduceTest::TestRanges() {
  using embb::algorithms::Reduce;
  size_t count = 4;
  int sum = 0;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
    sum += static_cast<int>(i + 2);
  }
  vector = init;

  // Omit first element
  PT_EXPECT_EQ(Reduce(vector.begin() + 1, vector.end(), 0, std::plus<int>()),
               sum - vector[0]);
  // Omit last element
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end() - 1, 0, std::plus<int>()),
               sum - vector[vector.size() - 1]);
  // Omit first and last element
  PT_EXPECT_EQ(Reduce(vector.begin() + 1, vector.end() - 1, 0,
               std::plus<int>()), sum - vector[0] - vector[vector.size() - 1]);
  // Only do first element
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.begin() + 1, 0, std::plus<int>()),
               vector[0]);
  // Only do last element
  PT_EXPECT_EQ(Reduce(vector.end() - 1, vector.end(), 0, std::plus<int>()),
               vector[vector.size() - 1]);
  // Only do second element
  PT_EXPECT_EQ(Reduce(vector.begin() + 1, vector.begin() + 2, 0,
               std::plus<int>()), vector[1]);
}

void ReduceTest::TestBlockSizes() {
  using embb::algorithms::Reduce;
  size_t count = 4;
  int sum = 0;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
    sum += static_cast<int>(i + 2);
  }
  vector = init;

  for (size_t block_size = 1; block_size < count + 2; block_size++) {
    PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>()),
                 sum);
  }
}

void ReduceTest::TestPolicy() {
  using embb::algorithms::Reduce;
  using embb::mtapi::ExecutionPolicy;
  using embb::algorithms::Identity;
  size_t count = 4;
  int sum = 0;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
    sum += static_cast<int>(i + 2);
  }
  vector = init;

  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>(),
               Identity(), ExecutionPolicy()), sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>(),
               Identity(), ExecutionPolicy(true)), sum);
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.end(), 0, std::plus<int>(),
               Identity(), ExecutionPolicy(true, 1)), sum);
  // Empty list should return neutral element:
  PT_EXPECT_EQ(Reduce(vector.begin(), vector.begin(), 41, std::plus<int>(),
               Identity(), ExecutionPolicy(true, 1)), 41);
#ifdef EMBB_USE_EXCEPTIONS
  bool empty_core_set_thrown = false;
  try {
    Reduce(vector.begin(), vector.end(), 0,
           std::plus<int>(), Identity(),
           ExecutionPolicy(false));
  } catch (embb::base::ErrorException &) {
    empty_core_set_thrown = true;
  }
  PT_EXPECT_MSG(empty_core_set_thrown,
    "Empty core set should throw ErrorException");
  bool negative_range_thrown = false;
  try {
    std::vector<int>::iterator second = vector.begin() + 1;
    Reduce(second, vector.begin(), 0, std::plus<int>());
  }
  catch (embb::base::ErrorException &) {
    negative_range_thrown = true;
  }
  PT_EXPECT_MSG(negative_range_thrown,
    "Negative range should throw ErrorException");
#endif
}

void ReduceTest::StressTest() {
  using embb::algorithms::Reduce;
  using embb::mtapi::ExecutionPolicy;
  using embb::algorithms::Identity;
  size_t count = embb::mtapi::Node::GetInstance().GetCoreCount() * 10;
  std::vector<int> large_vector(count);
  mtapi_int32_t expected = 0;
  for (size_t i = 0; i < count; i++) {
    large_vector[i] = static_cast<int>(i+2);
    expected += large_vector[i];
  }
  PT_EXPECT_EQ(Reduce(large_vector.begin(), large_vector.end(),
               mtapi_int32_t(0), std::plus<mtapi_int32_t>(), Identity(),
               ExecutionPolicy(), 1960), expected);
}
