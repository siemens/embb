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

#include <merge_sort_test.h>
#include <embb/algorithms/merge_sort.h>
#include <embb/mtapi/execution_policy.h>
#include <vector>
#include <deque>
#include <sstream>
#include <algorithm>
#include <functional>

static bool DescendingComparisonFunction(double lhs, double rhs) {
  return lhs < rhs ? true : false;
}

#define COMPARISON_JOB 17

static void DescendingComparisonActionFunction(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/) {
  typedef struct {
    int lhs;
    int rhs;
  } InT;
  typedef struct {
    bool out;
  } OutT;
  PT_EXPECT_EQ(args_size, sizeof(InT));
  PT_EXPECT_EQ(result_buffer_size, sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  outputs->out = inputs->lhs < inputs->rhs;
}

MergeSortTest::MergeSortTest() {
  CreateUnit("Different data structures")
    .Add(&MergeSortTest::TestDataStructures, this);
  CreateUnit("Function Pointers").Add(&MergeSortTest::TestFunctionPointers,
      this);
  CreateUnit("Homogeneous").Add(&MergeSortTest::TestHomogeneous, this);
  CreateUnit("Ranges").Add(&MergeSortTest::TestRanges, this);
  //CreateUnit("Block sizes").Add(&MergeSortTest::TestBlockSizes, this);
  CreateUnit("Policies").Add(&MergeSortTest::TestPolicy, this);
  CreateUnit("Stress test").Add(&MergeSortTest::StressTest, this);
}

void MergeSortTest::TestDataStructures() {
  using embb::algorithms::MergeSortAllocate;
  using embb::mtapi::ExecutionPolicy;
  int array[kCountSize];
  std::vector<int> vector(kCountSize);
  std::deque<int> deque(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    array[i] = static_cast<int>(i+2);
    vector[i] = static_cast<int>(i+2);
    deque[i] = static_cast<int>(i+2);
  }
  std::vector<int> vector_copy(vector);
  std::sort(vector_copy.begin(), vector_copy.end());
  MergeSortAllocate(array, array + kCountSize);
  MergeSortAllocate(vector.begin(), vector.end());
  MergeSortAllocate(array, array + kCountSize, std::less<int>(),
    ExecutionPolicy(), 2);
  MergeSortAllocate(deque.begin(), deque.end());
  for (size_t i = 0; i < kCountSize; i++) {
    PT_EXPECT_EQ(vector_copy[i], array[i]);
    PT_EXPECT_EQ(vector_copy[i], vector[i]);
    PT_EXPECT_EQ(vector_copy[i], deque[i]);
  }
}

void MergeSortTest::TestFunctionPointers() {
  using embb::algorithms::MergeSortAllocate;
  using embb::mtapi::ExecutionPolicy;

  std::vector<int> vector(kCountSize);
  for (size_t i = kCountSize - 1; i > 0; i--) {
    vector[i] = static_cast<int>(i + 2);
  }
  std::vector<int> vector_copy(vector);
  std::sort(vector_copy.begin(), vector_copy.end(),
            &DescendingComparisonFunction);
  MergeSortAllocate(vector.begin(), vector.end(),
    &DescendingComparisonFunction);
  for (size_t i = 0; i < kCountSize; i++) {
    PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }
}

void MergeSortTest::TestHomogeneous() {
  using embb::algorithms::MergeSortAllocate;
  using embb::mtapi::ExecutionPolicy;

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Action action = node.CreateAction(
    COMPARISON_JOB, DescendingComparisonActionFunction);
  embb::mtapi::Job job = node.GetJob(COMPARISON_JOB);

  std::vector<int> vector(kCountSize);
  for (size_t i = kCountSize - 1; i > 0; i--) {
    vector[i] = static_cast<int>(i + 2);
  }
  std::vector<int> vector_copy(vector);
  std::sort(vector_copy.begin(), vector_copy.end(),
    &DescendingComparisonFunction);
  MergeSortAllocate(vector.begin(), vector.end(), job);
  for (size_t i = 0; i < kCountSize; i++) {
    PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  action.Delete();
}

void MergeSortTest::TestRanges() {
  using embb::algorithms::MergeSortAllocate;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  std::vector<int> vector_copy(count);
  for (size_t i = count - 1; i > 0; i--) {
    init[i] = static_cast<int>(i+2);
  }

  // Omit first element
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.begin() + 1, vector_copy.end());
  MergeSortAllocate(vector.begin() + 1, vector.end());
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  // Omit last element
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.begin(), vector_copy.end() - 1);
  MergeSortAllocate(vector.begin(), vector.end() - 1);
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  // Omit first and last element
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.begin() + 1, vector_copy.end() - 1);
  MergeSortAllocate(vector.begin() + 1, vector.end() - 1);
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  // Only do first two elements
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.begin(), vector_copy.begin() + 2);
  MergeSortAllocate(vector.begin(), vector.begin() + 2);
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  // Only do last two elements
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.end() - 2, vector_copy.end());
  MergeSortAllocate(vector.end() - 2, vector.end());
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  // Only do second & third elements
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.begin() + 1, vector_copy.begin() + 3);
  MergeSortAllocate(vector.begin() + 1, vector.begin() + 3);
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }
}

void MergeSortTest::TestBlockSizes() {
  using embb::algorithms::MergeSortAllocate;
  using embb::mtapi::ExecutionPolicy;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  std::vector<int> vector_copy(count);
  for (size_t i = count - 1; i > 0; i--) {
    init[i] = static_cast<int>(i+2);
  }
  vector_copy = init;
  std::sort(vector_copy.begin(), vector_copy.end());

  for (size_t block_size = 1; block_size < count + 2; block_size++) {
    vector = init;
    MergeSortAllocate(vector.begin(), vector.end(), std::less<int>(),
      ExecutionPolicy(), block_size);
    for (size_t i = 0; i < count; i++) {
      PT_EXPECT_EQ(vector[i], vector_copy[i]);
    }
  }
}

void MergeSortTest::TestPolicy() {
  using embb::algorithms::MergeSortAllocate;
  using embb::mtapi::ExecutionPolicy;
  size_t count = 4;
  std::vector<int> init(count);
  std::vector<int> vector(count);
  std::vector<int> vector_copy(count);
  for (size_t i = count - 1; i > 0; i--) {
    init[i] = static_cast<int>(i+2);
  }
  vector = init;
  vector_copy = init;
  std::sort(vector_copy.begin(), vector_copy.end());

  MergeSortAllocate(vector.begin(), vector.end(), std::less<int>(),
                    ExecutionPolicy());
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  vector = init;
  MergeSortAllocate(vector.begin(), vector.end(), std::less<int>(),
                    ExecutionPolicy(true));
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  vector = init;
  MergeSortAllocate(vector.begin(), vector.end(), std::less<int>(),
                    ExecutionPolicy(true, 1));
  for (size_t i = 0; i < count; i++) {
     PT_EXPECT_EQ(vector_copy[i], vector[i]);
  }

  // MergeSort on empty list should not throw:
  MergeSortAllocate(vector.begin(), vector.begin(), std::less<int>());

#ifdef EMBB_USE_EXCEPTIONS
  bool empty_core_set_thrown = false;
  try {
    MergeSortAllocate(vector.begin(), vector.end(), std::less<int>(),
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
    MergeSortAllocate(second, vector.begin(), std::less<int>());
  }
  catch (embb::base::ErrorException &) {
    negative_range_thrown = true;
  }
  PT_EXPECT_MSG(negative_range_thrown,
    "Negative range should throw ErrorException");
#endif
}

void MergeSortTest::StressTest() {
  using embb::algorithms::MergeSortAllocate;
  size_t count = embb::mtapi::Node::GetInstance().GetCoreCount() * 10;
  std::vector<int> large_vector(count);
  std::vector<int> vector_copy(count);
  for (size_t i = count - 1; i > 0; i--) {
    large_vector[i] = static_cast<int>((i + 2) % 1000);
  }
  vector_copy = large_vector;
  std::sort(vector_copy.begin(), vector_copy.end());
  MergeSortAllocate(large_vector.begin(), large_vector.end());
  for (size_t i = 0; i < count; i++) {
    PT_EXPECT_EQ(large_vector[i], vector_copy[i]);
  }
}
