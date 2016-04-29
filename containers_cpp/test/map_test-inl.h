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

#ifndef CONTAINERS_CPP_TEST_MAP_TEST_INL_H_
#define CONTAINERS_CPP_TEST_MAP_TEST_INL_H_

#include <vector>

namespace embb {
namespace containers {
namespace test {

template <typename T>
MapTest<T>::MapTest() :
  n_threads(static_cast<int>
    (partest::TestSuite::GetDefaultNumThreads())),
  n_iterations(200),
  n_operations_per_iteration(200),
  n_map_elements_per_thread(100),
  map(),
  map_contain_vector(static_cast<unsigned int>
    (n_map_elements_per_thread*n_threads)) {
  CreateUnit("MapTestThreadsInsertAndEraseToGlobalMap").
    Pre(&MapTest::MapTest1_Pre, this).
    Add(&MapTest::MapTest1_ThreadMethod, this,
        static_cast<size_t>(n_threads),
        static_cast<size_t>(n_iterations)).
      Post(&MapTest::MapTest1_Post, this);
}

template <typename T>
void MapTest<T>::MapTest1_Pre() {
  embb_internal_thread_index_reset();
  thread_local_vectors_key =
    new std::vector<int>[static_cast<unsigned int>(n_threads)];

  thread_local_vectors_value =
    new std::vector<int>[static_cast<unsigned int>(n_threads)];

  for (int i = 0; i != n_threads; ++i) {
    int offset = n_map_elements_per_thread;

    for (int i2 = 0; i2 != n_map_elements_per_thread; ++i2) {
      int insert_element = i2 + (offset*i);
      thread_local_vectors_key[i].push_back(insert_element);
      thread_local_vectors_value[i].push_back(insert_element * 2);
      map_contain_vector[static_cast<unsigned int>(insert_element)] = false;
    }
  }
}

template <typename T>
void MapTest<T>::MapTest1_Post() {
  unsigned int map_elements = map_contain_vector.size();
  for (unsigned int i = 0; i < map_elements; i++) {
    PT_ASSERT(map_contain_vector[i] == map.Contains(static_cast<int>(i)));
    if (map_contain_vector[i])
      PT_ASSERT((map[i] == i * 2));
  }
}

template <typename T>
void MapTest<T>::MapTest1_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);

  PT_ASSERT((EMBB_SUCCESS == return_val));

  srand(time(NULL));

  std::vector<int>& my_values = thread_local_vectors_value[thread_index];
  std::vector<int>& my_keys = thread_local_vectors_key[thread_index];


  for (int i = 0; i < n_operations_per_iteration; i++) {
    int random_number = rand() % 100;
    int inserting = rand() % 2;

    int key   = my_keys[static_cast<unsigned int>(random_number)];
    int value = my_values[static_cast<unsigned int>(random_number)];

    if (inserting) {
      PT_ASSERT(map.Insert(key, value) !=
        map_contain_vector[static_cast<unsigned int>(key)]);
      map_contain_vector[static_cast<unsigned int>(key)] = true;

    } else {
      PT_ASSERT(map.Erase(key) ==
        map_contain_vector[static_cast<unsigned int>(key)]);
      map_contain_vector[static_cast<unsigned int>(key)] = false;
    }
  }
}




}  // namespace test
}  // namespace containers
}  // namespace embb


#endif  // CONTAINERS_CPP_TEST_MAP_TEST_INL_H_
