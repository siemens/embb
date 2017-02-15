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

#ifndef CONTAINERS_CPP_TEST_OBJECT_POOL_TEST_INL_H_
#define CONTAINERS_CPP_TEST_OBJECT_POOL_TEST_INL_H_

#include <vector>
#include <embb/base/c/internal/thread_index.h>

namespace embb {
namespace containers {
namespace test {
template<typename ValuePool>
ObjectPoolTest<ValuePool>::ObjectPoolTest() :
number_threads_(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())),
number_iterations_(static_cast<int>
  (partest::TestSuite::GetDefaultNumIterations())),
allocations_per_thread(100),
allocations(allocations_per_thread*number_threads_),
objectPool(static_cast<size_t>(allocations)) {
  CreateUnit("ParallelObjectPoolTest").
    Pre(&ObjectPoolTest::ParallelObjectPoolTest_Pre, this).
    Add(&ObjectPoolTest::ParallelObjectPoolTest_ThreadMethod, this,
    static_cast<size_t>(number_threads_),
    static_cast<size_t>(number_iterations_)).
    Post(&ObjectPoolTest::ParallelObjectPoolTest_Post, this);
}

template<typename ValuePool>
void ObjectPoolTest<ValuePool>::ParallelObjectPoolTest_Pre() {
  embb_internal_thread_index_reset();
}

template<typename ValuePool>
void ObjectPoolTest<ValuePool>::ParallelObjectPoolTest_Post() {
  //everything should be freed, we should be able to allocate everything...
  ::std::vector<ObjectPoolTestStruct*> allocated;

  for (int i = 0; i != allocations_per_thread * number_threads_; ++i) {
    // write number to allocate object, to later check that the objects
    // are disjoint.
    allocated.push_back(objectPool.Allocate(i));
  }

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(allocated.size()); ++i) {
    // check that objects are disjoint
    PT_ASSERT(static_cast<unsigned int>(allocated[i]->GetThreadId()) == i);
    objectPool.Free(allocated[i]);
  }
}

template<typename ValuePool>
void ObjectPoolTest<ValuePool>::ParallelObjectPoolTest_ThreadMethod() {
  unsigned int thread_index;

  int return_val = embb_internal_thread_index(&thread_index);

  PT_ASSERT(EMBB_SUCCESS == return_val);

  ::std::vector<ObjectPoolTestStruct*> allocated;

  for (int i = 0; i != allocations_per_thread; ++i) {
    // all threads allocate without synchronization
    ObjectPoolTestStruct* t = objectPool.Allocate(static_cast<int>
      (thread_index));
    allocated.push_back(t);
  }

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(allocations_per_thread); ++i) {
    // check that no other thread wrote to the object, we have allocated...
    PT_ASSERT(allocated[i]->GetThreadId() ==
      static_cast<int>(thread_index));
    objectPool.Free(allocated[i]);
  }
}
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_OBJECT_POOL_TEST_INL_H_
