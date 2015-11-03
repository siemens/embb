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

#include <rwlock_test.h>
#include <embb/base/c/errors.h>
#include <embb/base/mutex.h>

namespace embb {
namespace base {
namespace test {

RWLockTest::RWLockTest()
    : counter_(0),
      num_threads_(partest::TestSuite::GetDefaultNumThreads()),
      num_iterations_(partest::TestSuite::GetDefaultNumIterations()) {
  CreateUnit("Shared read")
      .Add(&RWLockTest::TestSharedRead_ThreadMethod, this,
           num_threads_, num_iterations_);
  CreateUnit("Exclusive write")
      .Pre(&RWLockTest::TestExclusiveWrite_Pre, this)
      .Add(&RWLockTest::TestExclusiveWrite_ReaderMethod, this,
           num_threads_, num_iterations_)
      .Add(&RWLockTest::TestExclusiveWrite_WriterMethod, this,
           num_threads_, num_iterations_)
      .Post(&RWLockTest::TestExclusiveWrite_Post, this);
}

void RWLockTest::TestSharedRead_ThreadMethod() {
  UniqueLock<embb::base::RWLock::ReaderLock> lock(rwlock_.GetReaderLock(),
                                                  embb::base::try_lock);
  PT_ASSERT_EQ_MSG(lock.OwnsLock(), true, "Failed to lock for reading.");

  int spin = 10000;
  while (--spin != 0);
}

void RWLockTest::TestExclusiveWrite_Pre() {
  counter_ = 0;
}

void RWLockTest::TestExclusiveWrite_ReaderMethod() {
  // Just add some contention
  UniqueLock<embb::base::RWLock::ReaderLock> lock(rwlock_.GetReaderLock(),
                                                  embb::base::try_lock);

  int spin = 10000;
  while (--spin != 0);
}

void RWLockTest::TestExclusiveWrite_WriterMethod() {
  UniqueLock<embb::base::RWLock::WriterLock> lock(rwlock_.GetWriterLock());

  ++counter_;
}

void RWLockTest::TestExclusiveWrite_Post() {
  PT_ASSERT_EQ_MSG(counter_, num_iterations_ * num_threads_,
                   "Counter value is inconsistent.");
}

} // namespace test
} // namespace base
} // namespace embb
