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

#include <shared_mutex_test.h>
#include <embb/base/c/errors.h>

namespace embb {
namespace base {
namespace test {

SharedMutexTest::SharedMutexTest()
    : counter_(0),
      num_threads_(partest::TestSuite::GetDefaultNumThreads()),
      num_iterations_(partest::TestSuite::GetDefaultNumIterations()) {
  CreateUnit("Shared read")
      .Pre(&SharedMutexTest::TestSharedRead_Pre, this)
      .Add(&SharedMutexTest::TestSharedRead_ThreadMethod, this,
           num_threads_, num_iterations_)
      .Post(&SharedMutexTest::TestSharedRead_Post, this);
  CreateUnit("Exclusive write")
      .Pre(&SharedMutexTest::TestExclusiveWrite_Pre, this)
      .Add(&SharedMutexTest::TestExclusiveWrite_ReaderMethod, this,
           num_threads_ / 2, num_iterations_)
      .Add(&SharedMutexTest::TestExclusiveWrite_WriterMethod, this,
           num_threads_ / 2, num_iterations_)
      .Post(&SharedMutexTest::TestExclusiveWrite_Post, this);
}

void SharedMutexTest::TestSharedRead_Pre() {
  int success = embb_shared_mutex_init(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to initialize shared mutex.");
}

void SharedMutexTest::TestSharedRead_ThreadMethod() {
  int success = embb_shared_mutex_try_lock_shared(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to lock for reading.");

  int spin = 10000;
  while (--spin != 0);

  success = embb_shared_mutex_unlock_shared(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to unlock (reading).");
}

void SharedMutexTest::TestSharedRead_Post() {
  embb_shared_mutex_destroy(&shared_mutex_);
}

void SharedMutexTest::TestExclusiveWrite_Pre() {
  int success = embb_shared_mutex_init(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to initialize shared mutex.");

  counter_ = 0;
}

void SharedMutexTest::TestExclusiveWrite_ReaderMethod() {
  // Just add some contention

  int success = embb_shared_mutex_try_lock_shared(&shared_mutex_);
  if (success != EMBB_SUCCESS) return;

  int spin = 10000;
  while (--spin != 0);

  success = embb_shared_mutex_unlock_shared(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to unlock (reading).");
}

void SharedMutexTest::TestExclusiveWrite_WriterMethod() {
  int success = embb_shared_mutex_lock(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to lock for writing.");

  ++counter_;

  success = embb_shared_mutex_unlock(&shared_mutex_);
  PT_ASSERT_EQ_MSG(success, EMBB_SUCCESS, "Failed to unlock (writing).");
}

void SharedMutexTest::TestExclusiveWrite_Post() {
  PT_ASSERT_EQ_MSG(counter_, num_iterations_ * (num_threads_ / 2),
                   "Counter value is inconsistent.");
  embb_shared_mutex_destroy(&shared_mutex_);
}

} // namespace test
} // namespace base
} // namespace embb
