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

#include <thread_index_test.h>
#include <embb/base/c/base.h>
#include <embb/base/c/internal/thread_index.h>
#include <embb/base/c/errors.h>
#include <limits.h>

#include <cassert>

namespace embb {
namespace base {
namespace test {

embb_atomic_int flag;

ThreadIndexTest::ThreadIndexTest()
    : number_threads_(partest::TestSuite::GetDefaultNumThreads()) {
  embb_atomic_init_int(&flag, 1);

  CreateUnit("Test 0 indices").Add(&ThreadIndexTest::Test0, this);
  CreateUnit("Test 1 index").Add(&ThreadIndexTest::Test1, this);
  CreateUnit("Test N indices").Add(&ThreadIndexTest::TestN, this, 1);
}

ThreadIndexTest::~ThreadIndexTest() {
  embb_atomic_destroy_int(&flag);
}

void ThreadIndexTest::Test0() {
  embb_internal_thread_index_reset();
  unsigned int old_max = embb_thread_get_max_count();
  embb_internal_thread_index_set_max(0);
  embb_thread_t thread;
  bool index_available = false;
  int status = embb_thread_create(&thread, NULL, ThreadStart, &index_available);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  embb_thread_join(&thread, NULL);
  embb_internal_thread_index_set_max(old_max);
}

void ThreadIndexTest::Test1() {
  embb_internal_thread_index_reset();
  unsigned int old_max = embb_thread_get_max_count();
  embb_internal_thread_index_set_max(1);
  {
    embb_thread_t thread;
    bool index_available = true;
    int status =
      embb_thread_create(&thread, NULL, ThreadStart, &index_available);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    embb_thread_join(&thread, NULL);
  }
  {
    embb_thread_t thread;
    bool index_available = false;
    int status =
      embb_thread_create(&thread, NULL, ThreadStart, &index_available);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    embb_thread_join(&thread, NULL);
  }
  embb_internal_thread_index_set_max(old_max);
}

void ThreadIndexTest::TestN() {
  embb_internal_thread_index_reset();
  unsigned int old_max = embb_thread_get_max_count();
  embb_internal_thread_index_set_max(
    static_cast<unsigned int>(number_threads_));
  embb_thread_t* threads = new embb_thread_t[number_threads_];

  embb_atomic_store_int(&flag, 0);
  for (size_t i = 0; i < number_threads_; i++) {
    bool index_available = true;
    int status = embb_thread_create(threads + i, NULL, ThreadStart,
                                    &index_available);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
  embb_atomic_store_int(&flag, 1);
  for (size_t i = 0; i < number_threads_; i++) {
    int status = embb_thread_join(threads + i, NULL);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
  embb_thread_t thread;
  bool index_available = false;
  int status = embb_thread_create(&thread, NULL, ThreadStart, &index_available);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  embb_thread_join(&thread, NULL);
  delete[] threads;
  embb_internal_thread_index_set_max(old_max);
}

int ThreadStart(void* arg) {
  assert(arg != NULL);
  unsigned int index = UINT_MAX;
  while (embb_atomic_load_int(&flag) == 0) { embb_thread_yield(); }
  int status = embb_internal_thread_index(&index);
  if (*static_cast<bool*>(arg) == true) {
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    PT_EXPECT_NE(index, UINT_MAX);
    PT_EXPECT_LT(index, partest::TestSuite::GetDefaultNumThreads());
  } else {
    PT_EXPECT_EQ(status, EMBB_ERROR);
    PT_EXPECT_EQ(index, UINT_MAX);
  }
  // Do it a second time
  status = embb_internal_thread_index(&index);
  if (*static_cast<bool*>(arg) == true) {
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    PT_EXPECT_NE(index, UINT_MAX);
    PT_EXPECT_LT(index, partest::TestSuite::GetDefaultNumThreads());
  } else {
    PT_EXPECT_EQ(status, EMBB_ERROR);
    PT_EXPECT_EQ(index, UINT_MAX);
  }
  return 0;
}

} // namespace test
} // namespace base
} // namespace embb
