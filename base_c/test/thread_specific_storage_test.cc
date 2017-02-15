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

#include <thread_specific_storage_test.h>
#include <embb/base/c/internal/thread_index.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/errors.h>
#include <iostream>

namespace embb {
namespace base {
namespace test {

ThreadSpecificStorageTest::ThreadSpecificStorageTest()
    : tss_(), number_threads_(partest::TestSuite::GetDefaultNumThreads()) {
  embb_tss_create(&tss_);
  CreateUnit("API")
      .Add(&ThreadSpecificStorageTest::Test, this, number_threads_, 1)
      .Post(&ThreadSpecificStorageTest::Post, this);
}

ThreadSpecificStorageTest::~ThreadSpecificStorageTest() {
  embb_tss_delete(&tss_);
}

void ThreadSpecificStorageTest::Test() {
  size_t rank = partest::TestSuite::GetCurrentThreadID();
  void* value = embb_tss_get(&tss_);
  if (value == NULL) {
    size_t * prank = new size_t(rank);
    int status = embb_tss_set(&tss_, prank);
    if (EMBB_SUCCESS != status) {
        delete prank;
    }
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  } else {
    size_t stored_rank = *static_cast<size_t*>(value);
    PT_EXPECT_EQ(rank, stored_rank);
  }
}

void ThreadSpecificStorageTest::Post() {
  size_t sum_ranks = 0;
  for (size_t i = 0; i < embb_thread_get_max_count(); i++) {
    void* value = tss_.values[i];
    if (value != NULL) {
      sum_ranks += *static_cast<size_t*>(value);
      delete static_cast<size_t*>(value);
    }
  }
  size_t expected_sum = 0;
  for (size_t i = 0; i < number_threads_; i++) {
    expected_sum += i;
  }
  PT_EXPECT_EQ(sum_ranks, expected_sum);
}

} // namespace test
} // namespace base
} // namespace embb
