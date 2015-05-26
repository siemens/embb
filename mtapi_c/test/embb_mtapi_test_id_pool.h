/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#define ID_POOL_SIZE_1 100
#define CONCURRENT_ACCESSORS_ID_POOL_2 10
#define ID_ELEMENTS_PER_ACCESSOR 10

class IdPoolTest : public partest::TestCase {
public:
  embb_mtapi_id_pool_t id_pool;
  embb_mtapi_id_pool_t id_pool_parallel;

  IdPoolTest();

private:
  void TestParallel();
  void TestParallelPre();
  void TestParallelPost();

  void TestBasic();
  void TestBasicPre();
  void TestBasicPost();

  static void TestAllocateDeallocateNElementsFromPool(
    embb_mtapi_id_pool_t &pool,
    int count_elements, 
    bool empty_check = false);
};

#endif // MTAPI_C_TEST_EMBB_MTAPI_TEST_ID_POOL_H_
