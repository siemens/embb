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

#include <mutex_test.h>
#include <embb/base/c/errors.h>
#include <iostream>

namespace embb {
namespace base {
namespace test {

MutexTest::MutexTest() : counter_(0),
    number_threads_(partest::TestSuite::GetDefaultNumThreads()),
    number_iterations_(partest::TestSuite::GetDefaultNumIterations()) {
  embb_mutex_init(&mutex_, EMBB_MUTEX_PLAIN);
  //embb_thread_set_max_count(number_threads_);
  CreateUnit("Protected counter")
      .Pre(&MutexTest::PreMutexInc, this)
      .Add(&MutexTest::TestMutexInc, this, number_threads_, number_iterations_)
      .Post(&MutexTest::PostMutexInc, this);
  CreateUnit("Recursive mutex")
      .Add(&MutexTest::TestRecursiveMutex, this);
}

void MutexTest::PreMutexInc() {
  counter_ = 0;
}

void MutexTest::TestMutexInc() {
  embb_mutex_lock(&mutex_);
  ++counter_;
  embb_mutex_unlock(&mutex_);
}

void MutexTest::PostMutexInc() {
  PT_EXPECT_EQ(counter_, number_iterations_ * number_threads_);
  embb_mutex_destroy(&mutex_);
}

void MutexTest::TestRecursiveMutex() {
  embb_mutex_t mutex;
  int status = embb_mutex_init(&mutex, EMBB_MUTEX_RECURSIVE);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  int number = 5;
  for (int i = 0; i < number; i++) {
    status = embb_mutex_lock(&mutex);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
  for (int i = 0; i < number; i++) {
    status = embb_mutex_unlock(&mutex);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
  embb_mutex_destroy(&mutex);
}

} // namespace test
} // namespace base
} // namespace embb
