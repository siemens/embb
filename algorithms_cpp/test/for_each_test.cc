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

#include <for_each_test.h>
#include <embb/algorithms/for_each.h>
#include <embb/mtapi/execution_policy.h>
#include <vector>
#include <deque>
#include <sstream>

/**
 * Functor to compute the square of a number.
 *
 * The result overwrites the original number.
 */
struct Square {
  template<typename Type>
  void operator()(Type& l) const {
    l = l * l;
  }
};

/**
 * Free function to compute the square of a number.
 *
 * The result overwrites the original number.
 */
static void SquareFunction(int &val) {
  val = val * val;
}

#define HETEROGENEOUS_JOB 17

static void SquareActionFunction(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/ ) {
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

ForEachTest::ForEachTest() {
  CreateUnit("ForLoop test").Add(&ForEachTest::TestLoop, this);
  CreateUnit("Different data structures")
    .Add(&ForEachTest::TestDataStructures, this);
  CreateUnit("Function Pointers").Add(&ForEachTest::TestFunctionPointers, this);
  CreateUnit("Heterogeneous").Add(&ForEachTest::TestHeterogeneous, this);
  CreateUnit("Ranges").Add(&ForEachTest::TestRanges, this);
  CreateUnit("Block sizes").Add(&ForEachTest::TestBlockSizes, this);
  CreateUnit("Policies").Add(&ForEachTest::TestPolicy, this);
  CreateUnit("Stress test").Add(&ForEachTest::StressTest, this);
}

void ForEachTest::TestDataStructures() {
  using embb::algorithms::ForEach;

  int array[kCountSize];
  std::vector<int> vector(kCountSize);
  std::deque<int> deque(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    array[i] = static_cast<int>(i+2);
    vector[i] = static_cast<int>(i+2);
    deque[i] = static_cast<int>(i+2);
  }

  ForEach(array, array + kCountSize, Square());
  ForEach(vector.begin(), vector.end(), Square());
  ForEach(deque.begin(), deque.end(), Square());

  for (size_t i = 0; i < kCountSize; i++) {
    int expected = static_cast<int>(i+2);
    expected = expected * expected;
    PT_EXPECT_EQ(expected, array[i]);
    PT_EXPECT_EQ(expected, vector[i]);
    PT_EXPECT_EQ(expected, deque[i]);
  }
}

void ForEachTest::TestFunctionPointers() {
  using embb::algorithms::ForEach;

  std::vector<int> vector(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i+2);
  }
  ForEach(vector.begin(), vector.end(), &SquareFunction);
  for (size_t i = 0; i < kCountSize; i++) {
    int expected = static_cast<int>(i+2);
    expected = expected * expected;
    PT_EXPECT_EQ(expected, vector[i]);
  }
}

void ForEachTest::TestHeterogeneous() {
  using embb::algorithms::ForEach;

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Action action = node.CreateAction(
    HETEROGENEOUS_JOB, SquareActionFunction);
  embb::mtapi::Job job = node.GetJob(HETEROGENEOUS_JOB);

  std::vector<int> vector(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vector[i] = static_cast<int>(i + 2);
  }
  ForEach(vector.begin(), vector.end(), job);
  for (size_t i = 0; i < kCountSize; i++) {
    int expected = static_cast<int>(i + 2);
    expected = expected * expected;
    PT_EXPECT_EQ(expected, vector[i]);
  }

  action.Delete();
}

void ForEachTest::TestRanges() {
  using embb::algorithms::ForEach;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
  }

  // Omit first element
  vector = init;
  ForEach(vector.begin() + 1, vector.end(), Square());
  PT_EXPECT_EQ(vector[0], init[0]);
  for (size_t i = 1; i < count; i++) {
    PT_EXPECT_EQ(vector[i], init[i]*init[i]);
  }

  // Omit last element
  vector = init;
  ForEach(vector.begin(), vector.end() - 1, Square());
  for (size_t i = 0; i < count - 1; i++) {
    PT_EXPECT_EQ(vector[i], init[i]*init[i]);
  }
  PT_EXPECT_EQ(vector[count - 1], init[count - 1]);

  // Omit first and last element
  vector = init;
  ForEach(vector.begin() + 1, vector.end() - 1, Square());
  PT_EXPECT_EQ(vector[0], init[0]);
  for (size_t i = 1; i < count - 1; i++) {
    PT_EXPECT_EQ(vector[i], init[i]*init[i]);
  }
  PT_EXPECT_EQ(vector[count - 1], init[count - 1]);

  // Only do first element
  vector = init;
  ForEach(vector.begin(), vector.begin() + 1, Square());
  PT_EXPECT_EQ(vector[0], init[0] * init[0]);
  for (size_t i = 1; i < count; i++) {
    PT_EXPECT_EQ(vector[i], init[i]);
  }

  // Only do last element
  vector = init;
  ForEach(vector.end() - 1, vector.end(), Square());
  for (size_t i = 0; i < count - 1; i++) {
    PT_EXPECT_EQ(vector[i], init[i]);
  }
  PT_EXPECT_EQ(vector[count - 1], init[count - 1] * init[count - 1]);

  // Only do second element
  vector = init;
  ForEach(vector.begin() + 1, vector.begin() + 2, Square());
  for (size_t i = 1; i < count; i++) {
    if (i != 1) {
      PT_EXPECT_EQ(vector[i], init[i]);
    } else {
      PT_EXPECT_EQ(vector[i], init[i] * init[i]);
    }
  }
}

void ForEachTest::TestBlockSizes() {
  using embb::algorithms::ForEach;
  using embb::mtapi::ExecutionPolicy;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
  }

  for (size_t block_size = 1; block_size < count + 2; block_size++) {
    vector = init;
    ForEach(vector.begin(), vector.end(), Square(), ExecutionPolicy(),
            block_size);
    for (size_t i = 0; i < count; i++) {
      PT_EXPECT_EQ(vector[i], init[i]*init[i]);
    }
  }
}

void ForEachTest::TestPolicy() {
  using embb::algorithms::ForEach;
  using embb::mtapi::ExecutionPolicy;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    init[i] = static_cast<int>(i+2);
  }

  vector = init;
  ForEach(vector.begin(), vector.end(), Square(), ExecutionPolicy());
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(vector[i], init[i]*init[i]);
  }

  vector = init;
  ForEach(vector.begin(), vector.end(), Square(), ExecutionPolicy(true));
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(vector[i], init[i]*init[i]);
  }

  vector = init;
  ForEach(vector.begin(), vector.end(), Square(), ExecutionPolicy(true, 1));
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(vector[i], init[i]*init[i]);
  }

  // ForEach on empty list should not throw:
  ForEach(vector.begin(), vector.begin(), Square());

#ifdef EMBB_USE_EXCEPTIONS
  bool empty_core_set_thrown = false;
  try {
    ForEach(vector.begin(), vector.end(), Square(), ExecutionPolicy(false));
  }
  catch (embb::base::ErrorException &) {
    empty_core_set_thrown = true;
  }
  PT_EXPECT_MSG(empty_core_set_thrown,
    "Empty core set should throw ErrorException");
  bool negative_range_thrown = false;
  try {
    std::vector<int>::iterator second = vector.begin() + 1;
    ForEach(second, vector.begin(), Square());
  }
  catch (embb::base::ErrorException &) {
    negative_range_thrown = true;
  }
  PT_EXPECT_MSG(negative_range_thrown,
    "Negative range should throw ErrorException");
#endif
}

static std::vector<size_t> loop_result;

static void SquareLoop(size_t value) {
  loop_result[value] = value * value;
}

void ForEachTest::TestLoop() {
  using embb::algorithms::ForLoop;

  size_t count = 10;
  loop_result.resize(count);
  ForLoop(size_t(0), count, SquareLoop);
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(loop_result[i], i * i);
  }
  loop_result.clear();

  size_t stride = 3;
  loop_result.resize(count * stride);
  ForLoop(size_t(0), count * stride, static_cast<int>(stride), SquareLoop);
  for (size_t i = 0; i < count * stride; i += stride) {
    PT_EXPECT_EQ(loop_result[i], i * i);
    for (size_t k = 1; k < stride; k++) {
      PT_EXPECT_EQ(loop_result[i + k], 0u);
    }
  }
  loop_result.clear();
}

void ForEachTest::StressTest() {
  using embb::algorithms::ForEach;
  using embb::mtapi::ExecutionPolicy;
  size_t count = embb::mtapi::Node::GetInstance().GetCoreCount() * 10;
  std::vector<int> large_vector(count);
  for (size_t i = 0; i < count; i++) {
    large_vector[i] = static_cast<int>((i + 2) % 1000);
  }
  ForEach(large_vector.begin(), large_vector.end(), Square(), ExecutionPolicy(),
          2000);
  for (size_t i = 0; i < count; i++) {
    int expected = static_cast<int>((i + 2) % 1000);
    expected = expected * expected;
    PT_EXPECT_EQ(large_vector[i], expected);
  }
}
