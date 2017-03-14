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

#ifndef CONTAINERS_CPP_TEST_STACK_TEST_INL_H_
#define CONTAINERS_CPP_TEST_STACK_TEST_INL_H_

#include <vector>
#include <algorithm>

namespace embb {
namespace containers {
namespace test {
template<typename Stack_t>
StackTest<Stack_t>::StackTest() :
n_threads(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())),
#ifdef EMBB_THREADING_ANALYSIS_MODE
  n_iterations(10),
#else
  n_iterations(200),
#endif
  n_stack_elements_per_thread(100),
  n_stack_elements(n_stack_elements_per_thread*n_threads),
  stack(static_cast<size_t>(n_stack_elements)),
  thread_local_vectors(NULL),
  stackSize(0) {
  CreateUnit("StackTestThreadsPushAndPopToGlobalStack").
  Pre(&StackTest::StackTest1_Pre, this).
  Add(&StackTest::StackTest1_ThreadMethod, this,
  static_cast<size_t>(n_threads),
  static_cast<size_t>(n_iterations)).
  Post(&StackTest::StackTest1_Post, this);
}

template<typename Stack_t>
void StackTest<Stack_t>::StackTest1_Pre() {
  embb_internal_thread_index_reset();
  thread_local_vectors =
    new std::vector<int>[static_cast<unsigned int>(n_threads)];

  for (int i = 0; i != n_threads; ++i) {
    int offset = n_stack_elements_per_thread * 2;

    for (int i2 = 0; i2 != n_stack_elements_per_thread; ++i2) {
      int push_element = i2 + (offset*i);
      thread_local_vectors[i].push_back(push_element);
      expected_stack_elements.push_back(push_element);
    }
  }
}

template<typename Stack_t>
void StackTest<Stack_t>::StackTest1_Post() {
  std::vector<int> produced;
  for (int i = 0; i != n_threads; ++i) {
    std::vector<int>& loc_elements = thread_local_vectors[i];
    for (std::vector<int>::iterator it = loc_elements.begin();
      it != loc_elements.end();
      ++it) {
      produced.push_back(*it);
    }
  }

  PT_ASSERT(produced.size() == expected_stack_elements.size());

  std::sort(expected_stack_elements.begin(), expected_stack_elements.end());
  std::sort(produced.begin(), produced.end());

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(produced.size()); ++i) {
    PT_ASSERT(expected_stack_elements[i] == produced[i]);
  }

  delete[] thread_local_vectors;
}

template<typename Stack_t>
void StackTest<Stack_t>::StackTest1_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);

  PT_ASSERT(EMBB_SUCCESS == return_val);

  std::vector<int>& my_elements = thread_local_vectors[thread_index];

  for (std::vector<int>::iterator it = my_elements.begin();
    it != my_elements.end();
    ++it) {
    bool success = stack.TryPush(*it);
    PT_ASSERT(success == true);
  }

  my_elements.clear();

  for (int i = 0; i != n_stack_elements_per_thread; ++i) {
    int return_elem;
    bool success = stack.TryPop(return_elem);
    PT_ASSERT(success == true);
    my_elements.push_back(return_elem);
  }
}
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_STACK_TEST_INL_H_
