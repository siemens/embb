/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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

#ifndef CONTAINERS_CPP_TEST_BLOCKING_CONTAINER_TEST_INL_H_
#define CONTAINERS_CPP_TEST_BLOCKING_CONTAINER_TEST_INL_H_

#include <vector>
#include <algorithm>

namespace embb {
namespace containers {
namespace test {
template<typename Container_t>
BlockingContainerTest<Container_t>::BlockingContainerTest() :
n_threads(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())),
  n_iterations(200),
  n_container_elements_per_thread(100),
  n_container_elements(n_container_elements_per_thread*n_threads),
  container(){
  std::cout << "num thread = " << n_threads << std::endl;
  CreateUnit("BlockingContainerTestThreadsPushAndPopToGlobalBlockingContainer").
  Pre(&BlockingContainerTest::BlockingContainerTest1_Pre, this).
  Add(&BlockingContainerTest::BlockingContainerTest1_ThreadMethod, this,
  static_cast<size_t>(n_threads),
  static_cast<size_t>(n_iterations)).
  Post(&BlockingContainerTest::BlockingContainerTest1_Post, this);
}

template<typename Container_t>
void BlockingContainerTest<Container_t>::BlockingContainerTest1_Pre() {
  embb_internal_thread_index_reset();
  thread_local_vectors =
    new std::vector<int>[static_cast<unsigned int>(n_threads)];

  for (int i = 0; i != n_threads; ++i) {
    int offset = n_container_elements_per_thread * 2;

    for (int i2 = 0; i2 != n_container_elements_per_thread; ++i2) {
      int push_element = i2 + (offset*i);
      thread_local_vectors[i].push_back(push_element);
      expected_container_elements.push_back(push_element);
    }
  }
}

template<typename Container_t>
void BlockingContainerTest<Container_t>::BlockingContainerTest1_Post() {
  std::vector<int> produced;
  for (int i = 0; i != n_threads; ++i) {
    std::vector<int>& loc_elements = thread_local_vectors[i];
    for (std::vector<int>::iterator it = loc_elements.begin();
      it != loc_elements.end();
      ++it) {
      produced.push_back(*it);
    }
  }

  PT_ASSERT(produced.size() == expected_container_elements.size());

  std::sort(expected_container_elements.begin(), expected_container_elements.end());
  std::sort(produced.begin(), produced.end());

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(produced.size()); ++i) {
    PT_ASSERT(expected_container_elements[i] == produced[i]);
  }

  delete[] thread_local_vectors;
}

template<typename Container_t>
void BlockingContainerTest<Container_t>::BlockingContainerTest1_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);

  PT_ASSERT(EMBB_SUCCESS == return_val);

  std::vector<int>& my_elements = thread_local_vectors[thread_index];

  for (std::vector<int>::iterator it = my_elements.begin();
    it != my_elements.end();
    ++it) {
    SpecializezdPush(*it);
  }

  my_elements.clear();

  for (int i = 0; i != n_container_elements_per_thread; ++i) {
    int return_elem;
    SpecializedPop(return_elem);
    my_elements.push_back(return_elem);
  }
}
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_BLOCKING_CONTAINER_TEST_INL_H_
