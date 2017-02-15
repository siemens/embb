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

#include <count_test.h>
#include <embb/algorithms/count.h>
#include <embb/mtapi/execution_policy.h>
#include <deque>
#include <vector>
#include <functional>

struct IsEven{
  bool operator()(int val) const {
    if (val % 2 == 0)
      return true;
    else
      return false;
  }
};

bool IsEvenFunction(int val) {
  if (val % 2 == 0)
    return true;
  else
    return false;
}

#define PREDICATE_JOB 17

static void IsEvenActionFunction(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/) {
  typedef struct {
    int val;
  } InT;
  typedef struct {
    bool out;
  } OutT;
  PT_EXPECT_EQ(args_size, sizeof(InT));
  PT_EXPECT_EQ(result_buffer_size, sizeof(OutT));
  InT const * inputs = static_cast<InT const *>(args);
  OutT * outputs = static_cast<OutT *>(result_buffer);
  outputs->out = (inputs->val % 2) == 0;
}

CountTest::CountTest() {
  CreateUnit("Different data structures")
    .Add(&CountTest::TestDataStructures, this);
  CreateUnit("CountIf").Add(&CountTest::TestCountIf, this);
  CreateUnit("Ranges").Add(&CountTest::TestRanges, this);
  CreateUnit("Block sizes").Add(&CountTest::TestBlockSizes, this);
  CreateUnit("Policies").Add(&CountTest::TestPolicy, this);
  CreateUnit("Stress test").Add(&CountTest::StressTest, this);
}

void CountTest::TestDataStructures() {
  using embb::algorithms::Count;
  const int size = 10;
  int array[] = { 10, 20, 30, 30, 20, 10, 10, 20, 20, 20 };
  std::vector<int> vector(array, array + size);
  std::deque<int> deque(array, array + size);
  const std::vector<int> const_vector(array, array + size);

  PT_EXPECT_EQ(Count(array, array + size, 10), 3);
  PT_EXPECT_EQ(Count(vector.begin(), vector.end(), 10), 3);
  PT_EXPECT_EQ(Count(deque.begin(), deque.end(), 10), 3);
  PT_EXPECT_EQ(Count(const_vector.begin(), const_vector.end(), 10), 3);
}

void CountTest::TestCountIf() {
  using embb::algorithms::CountIf;
  const int size = 10;
  int array[] = { 10, 21, 30, 31, 20, 11, 10, 21, 20, 20 };
  PT_EXPECT_EQ(CountIf(array, array + size, IsEven()), 6);
  PT_EXPECT_EQ(CountIf(array, array + size, &IsEvenFunction), 6);

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Action action = node.CreateAction(
    PREDICATE_JOB, IsEvenActionFunction);
  embb::mtapi::Job job = node.GetJob(PREDICATE_JOB);

  PT_EXPECT_EQ(CountIf(array, array + size, job), 6);

  action.Delete();
}

void CountTest::TestRanges() {
  using embb::algorithms::Count;
  size_t count = 4;
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    vector[i] = static_cast<int>(-1);
  }

  // Omit first element
  PT_EXPECT_EQ(Count(vector.begin() + 1, vector.end(), -1), 3);

  // Omit last element
  PT_EXPECT_EQ(Count(vector.begin(), vector.end() - 1, -1), 3);

  // Omit first and last element
  PT_EXPECT_EQ(Count(vector.begin() + 1, vector.end() - 1, -1), 2);

  // Only do first element
  PT_EXPECT_EQ(Count(vector.begin(), vector.begin() + 1, -1), 1);

  // Only do last element
  PT_EXPECT_EQ(Count(vector.end() - 1, vector.end(), -1), 1);

  // Only do second element
  PT_EXPECT_EQ(Count(vector.begin() + 1, vector.begin() + 2, -1), 1);
}

void CountTest::TestBlockSizes() {
  using embb::algorithms::Count;
  size_t count = 4;
  std::vector<int> vector(count);
  for (size_t i = 0; i < count; i++) {
    vector[i] = -1;
  }

  for (size_t block_size = 1; block_size < count + 2; block_size++) {
    PT_EXPECT_EQ(Count(vector.begin(), vector.end(), -1),
                 static_cast<int>(count));
  }
}

void CountTest::TestPolicy() {
  using embb::algorithms::Count;
  using embb::mtapi::ExecutionPolicy;
  int a[] = { 10, 20, 30, 30, 20, 10, 10, 20, 20, 20 };
  std::vector<int> vector(a, a + (sizeof a / sizeof a[0]));
  PT_EXPECT_EQ(Count(vector.begin(), vector.end(), 10, ExecutionPolicy()), 3);
  PT_EXPECT_EQ(Count(vector.begin(), vector.end(), 10, ExecutionPolicy(true)),
               3);
  PT_EXPECT_EQ(Count(vector.begin(), vector.end(), 10,
               ExecutionPolicy(true, 1)), 3);
}

void CountTest::StressTest() {
  using embb::algorithms::Count;
  size_t count = embb::mtapi::Node::GetInstance().GetCoreCount() * 10;
  std::vector<int> large_vector(count);
  for (size_t i = 0; i < count; i++) {
    large_vector[i] = static_cast<int>(0);
  }
  PT_EXPECT_EQ(Count(large_vector.begin(), large_vector.end(), 0),
               static_cast<int>(count));
}
