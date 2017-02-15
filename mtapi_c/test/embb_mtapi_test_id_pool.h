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

#ifndef MTAPI_C_TEST_EMBB_MTAPI_TEST_ID_POOL_H_
#define MTAPI_C_TEST_EMBB_MTAPI_TEST_ID_POOL_H_

#include <partest/partest.h>
#include <embb_mtapi_id_pool_t.h>

// for shuffling a vector
#include <algorithm>

class IdPoolTest : public partest::TestCase {
 public:
  embb_mtapi_id_pool_t id_pool;
  embb_mtapi_id_pool_t id_pool_parallel;

  IdPoolTest();

 private:
  static const unsigned int id_pool_size_1 = 100;
  static const unsigned int concurrent_accessors_id_pool_2 = 10;
  static const unsigned int id_elements_per_accessor = 10;

  /**  
   * We create a pool of size number_accessors*elements_per_accessor, so
   * at each time we can guarantee each thread to be able to allocate 
   * elements_per_accessor elements.
   * We create number_accessor threads, where each thread iteratively 
   * allocates and frees elements_per_accessor elements, which in each case
   * has to be successful. Additionally, the sanity checks from the basic tests
   * are repeated. The TestParallelPost function also repeats all
   * sequential tests.
   */
  void TestParallel();
  void TestParallelPre();
  void TestParallelPost();

  /**
   * Create a pool of size N. We repeatedly allocate and free N elements, check
   * if the pool always returns disjunctive ids and check that the pool never
   * returns the invalid element, if the pool is not empty. Check that the 
   * invalid element is returned if the pool is empty.
   */
  void TestBasic();
  void TestBasicPre();
  void TestBasicPost();

  static void TestAllocateDeallocateNElementsFromPool(
    embb_mtapi_id_pool_t &pool,
    int count_elements,
    bool empty_check = false);
};

#endif // MTAPI_C_TEST_EMBB_MTAPI_TEST_ID_POOL_H_
