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

#include <embb_mtapi_test_id_pool.h>
#include <vector>

IdPoolTest::IdPoolTest() {
#ifdef EMBB_THREADING_ANALYSIS_MODE
  const int iterations(10);
#else
  const int iterations(1000);
#endif
  CreateUnit("mtapi id pool test single threaded").
    Add(&IdPoolTest::TestBasic, this, 1, iterations).
    Pre(&IdPoolTest::TestBasicPre, this).
    Post(&IdPoolTest::TestBasicPost, this);

  CreateUnit("mtapi id pool test concurrent").
    Add(&IdPoolTest::TestParallel, this, concurrent_accessors_id_pool_2
    , 20).
    Post(&IdPoolTest::TestParallelPost, this).
    Pre(&IdPoolTest::TestParallelPre, this);
}

void IdPoolTest::TestParallel() {
  // allocate ID_ELEMENTS_PER_ACCESSOR elements. Each test thread is
  // guaranteed to be able to allocate this amount of elements.
  TestAllocateDeallocateNElementsFromPool(id_pool_parallel,
    id_elements_per_accessor);
}

void IdPoolTest::TestParallelPre() {
  // create second id pool with CONCURRENT_ACCESSORS_ID_POOL_2*
  // ID_ELEMENTS_PER_ACCESSOR elements
  embb_mtapi_id_pool_initialize(&id_pool_parallel,
    concurrent_accessors_id_pool_2*id_elements_per_accessor);
}

void IdPoolTest::TestParallelPost() {
  // after the parallel tests, try to again allocate and deallocate all
  // elements sequentially.
  TestAllocateDeallocateNElementsFromPool(id_pool_parallel,
    concurrent_accessors_id_pool_2*id_elements_per_accessor, true);

  // finalize pool
  embb_mtapi_id_pool_finalize(&id_pool_parallel);
}

void IdPoolTest::TestBasic() {
  TestAllocateDeallocateNElementsFromPool(id_pool, id_pool_size_1, true);
}

void IdPoolTest::TestBasicPre() {
  // create id pool with ID_POOL_SIZE_1 elements
  embb_mtapi_id_pool_initialize(&id_pool, id_pool_size_1);
}

void IdPoolTest::TestBasicPost() {
  // finalize pool
  embb_mtapi_id_pool_finalize(&id_pool);
}

void IdPoolTest::TestAllocateDeallocateNElementsFromPool(
  embb_mtapi_id_pool_t &pool,
  int count_elements,
  bool empty_check) {
  std::vector<unsigned int> allocated;

  for (int i = 0; i != count_elements; ++i) {
    allocated.push_back(embb_mtapi_id_pool_allocate(&pool));
  }

  // the allocated elements should be disjunctive, and never invalid element
  for (unsigned int x = 0; x != allocated.size(); ++x) {
    PT_ASSERT(allocated[x] != EMBB_MTAPI_IDPOOL_INVALID_ID);
    for (unsigned int y = 0; y != allocated.size(); ++y) {
      if (x == y) {
        continue;
      }
      PT_ASSERT(allocated[x] != allocated[y]);
    }
  }

  // now the id pool should be empty... try ten times to get an id,
  // we should always get the invalid element
  if (empty_check) {
    for (int i = 0; i != 10; ++i) {
      PT_ASSERT_EQ(embb_mtapi_id_pool_allocate(&pool),
        static_cast<unsigned int>(EMBB_MTAPI_IDPOOL_INVALID_ID)
        )
    }
  }

  // now return allocated elements in a shuffled manner.
  ::std::random_shuffle(allocated.begin(), allocated.end());

  for (int i = 0; i != count_elements; ++i) {
    embb_mtapi_id_pool_deallocate(&pool,
      allocated[static_cast<unsigned int>(i)]);
  }
}

