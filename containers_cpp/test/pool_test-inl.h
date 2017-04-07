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

#ifndef CONTAINERS_CPP_TEST_POOL_TEST_INL_H_
#define CONTAINERS_CPP_TEST_POOL_TEST_INL_H_

#include <utility>
#include <algorithm>
#include <vector>

namespace embb {
namespace containers {
namespace test {
template<typename ValuePool_t>
const int PoolTest<ValuePool_t>::pool_elements_per_thread = 5;

template<typename ValuePool_t>
PoolTest<ValuePool_t>::PoolTest() :
  number_threads_(static_cast<int>
    (partest::TestSuite::GetDefaultNumThreads())),
  pool(NULL) {
  CreateUnit("PoolTestStatic").Add(&PoolTest::PoolTestStatic, this);

  CreateUnit("AllocFreeParallel")
    .Pre(&PoolTest::PreAllocFreeParallel, this)
    .Add(&PoolTest::AllocFreeParallel, this,
    static_cast<size_t>(number_threads_),
#ifdef EMBB_THREADING_ANALYSIS_MODE
    static_cast<size_t>(100))
#else
    static_cast<size_t>(10000))
#endif
    .Post(&PoolTest::PostAllocFreeParallel, this);
}

template<typename ValuePool_t>
void PoolTest<ValuePool_t>::PreAllocFreeParallel() {
  std::vector< int > elements;

  for (int i = 0; i != (number_threads_*pool_elements_per_thread); ++i) {
    elements.push_back(i + 1);
  }

  pool = new ValuePool_t(elements.begin(), elements.end());
}

template<typename ValuePool_t>
void PoolTest<ValuePool_t>::AllocFreeParallel() {
  ::std::vector< ::std::pair<int, int> > allocated;

  for (int i = 0; i != pool_elements_per_thread; ++i) {
    int element = 0;
    int index = pool->Allocate(element);

    //memory is not allowed to run out
    PT_ASSERT(index != -1);

    //we aren't not allowed to get anything that was not contained in the
    //beginning
    PT_ASSERT(element > 0 &&
      element <= (number_threads_*pool_elements_per_thread));

    allocated.push_back(::std::make_pair(element, index));
  }

  for (int i = 0; i != pool_elements_per_thread; ++i) {
    for (int j = 0; j != pool_elements_per_thread; ++j) {
      if (i == j) continue;

      //we should never get equal elements!
      PT_EXPECT(allocated[static_cast<unsigned int>(i)]
        .first != allocated[static_cast<unsigned int>(j)].first);
    }
  }

  for (int i = 0; i != pool_elements_per_thread; ++i) {
    pool->Free(allocated[static_cast<unsigned int>(i)].first,
      allocated[static_cast<unsigned int>(i)].second);
  }
}

template<typename ValuePool_t>
void PoolTest<ValuePool_t>::PostAllocFreeParallel() {
  int poolsize = (number_threads_*pool_elements_per_thread);

  ::std::vector<int> elements;

  for (int i = 0; i != poolsize; ++i) {
    int element;
    int index = pool->Allocate(element);

    //all elements shall be returned... we shall not run out of mem.
    PT_EXPECT(index != -1);

    elements.push_back(element);
  }

  ::std::sort(elements.begin(), elements.end());
  //after all threads are finished, the same elements than in the beginning
  //shall be contained
  for (int i = 0; i != poolsize; ++i) {
    PT_EXPECT(elements[static_cast<unsigned int>(i)] == i+1);
  }

  delete pool;
}

template<typename ValuePool_t>
void PoolTest<ValuePool_t>::PoolTestStatic() {
  size_t size = 100;
  int* arr = new int[size];

  for (int i = 0; i != static_cast<int>(size); ++i) {
    arr[static_cast<unsigned int>(i)] = i;
  }

  //create pool with bottom element -1, elements 0-(size-1) are added
  ValuePool_t ap(arr, arr + size);

  delete[] arr;

  int element;
  int index;
  std::vector< std::pair<int, int> > allocated;

  //as long as possible, allocate elements
  while ((index = ap.Allocate(element)) != -1) {
    //write allocated elements to vector, to be able to
    //free again!
    allocated.push_back(std::make_pair(element, index));
  }

  std::vector<int> indexes_to_free;

  //determine some elements we want to free
  indexes_to_free.push_back(5);
  indexes_to_free.push_back(16);
  indexes_to_free.push_back(43);
  indexes_to_free.push_back(90);

  for (int i = 0; i != static_cast<int>(indexes_to_free.size()); ++i) {
    //free those elements
    ap.Free(allocated[static_cast<unsigned int>(
      indexes_to_free[static_cast<unsigned int>(i)])].first,
      allocated[static_cast<unsigned int>(
      indexes_to_free[static_cast<unsigned int>(i)])]
      .second);
  }

  //if we allocate again, we should get those elements
  for (int i = 0; i != static_cast<int>(indexes_to_free.size()); i++) {
    index = ap.Allocate(element);

    PT_EXPECT((index != -1));

    std::vector<int>::iterator it;

    it = std::find(indexes_to_free.begin(), indexes_to_free.end(), element);

    PT_EXPECT(it != indexes_to_free.end());
  }
}
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_POOL_TEST_INL_H_
