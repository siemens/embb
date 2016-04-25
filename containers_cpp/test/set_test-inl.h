#pragma once
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

#ifndef CONTAINERS_CPP_TEST_SET_TEST_INL_H_
#define CONTAINERS_CPP_TEST_SET_TEST_INL_H_

#include <vector>
#include <algorithm>
#include <time.h>

namespace embb {
namespace containers {
namespace test {

template <typename T>
SetTest<T>::SetTest() :
  n_threads(static_cast<int>
    (partest::TestSuite::GetDefaultNumThreads())),
  n_iterations(200),
  n_operations_per_iteration(200),
  n_set_elements_per_thread(100),
  set(),
  set_contain_vector(static_cast<unsigned int>
    (n_set_elements_per_thread*n_threads)) {
  CreateUnit("SetTestThreadsInsertAndEraseToGlobalSet").
    Pre(&SetTest::SetTest1_Pre, this).
    Add(&SetTest::SetTest1_ThreadMethod, this,
      static_cast<size_t>(n_threads),
      static_cast<size_t>(n_iterations)).
    Post(&SetTest::SetTest1_Post, this);
}

template <typename T>
void SetTest<T>::SetTest1_Pre(){
  embb_internal_thread_index_reset();
  thread_local_vectors =
    new std::vector<int>[static_cast<unsigned int>(n_threads)];

  for (int i = 0; i != n_threads; ++i) {
    int offset = n_set_elements_per_thread;

    for (int i2 = 0; i2 != n_set_elements_per_thread; ++i2) {
      int insert_element = i2 + (offset*i);
      thread_local_vectors[i].push_back(insert_element);
      set_contain_vector[static_cast<unsigned int>(insert_element)] = false;
    }
  }
}

template <typename T>
void SetTest<T>::SetTest1_Post(){
  unsigned int set_elements = set_contain_vector.size();
  for (unsigned int i = 0; i < set_elements; i++) {
    PT_ASSERT(set_contain_vector[i] == set.Contains(static_cast<int>(i)));
  }

  delete[] thread_local_vectors;
}

template <typename T>
void SetTest<T>::SetTest1_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);

  PT_ASSERT(EMBB_SUCCESS == return_val);
  
  srand(time(NULL));

  std::vector<int>& my_elements = thread_local_vectors[thread_index];

  for (int i = 0; i < n_operations_per_iteration; i++) {
    int random_number = rand() % 100;
    int inserting = rand() % 2;

    int element = my_elements[static_cast<unsigned int>(random_number)];

    if (inserting) {
      PT_ASSERT(set.Insert(element) != set_contain_vector[static_cast<unsigned int>(element)]);
      set_contain_vector[static_cast<unsigned int>(element)] = true;
    }
    else {
      PT_ASSERT(set.Erase(element) == set_contain_vector[static_cast<unsigned int>(element)]);
      set_contain_vector[static_cast<unsigned int>(element)] = false;
    }
  }
}

      
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_SET_TEST_INL_H_