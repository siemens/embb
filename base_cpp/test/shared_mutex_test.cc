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
#include <embb/base/mutex.h>

namespace embb {
namespace base {
namespace test {

SharedMutexTest::SharedMutexTest()
    : shared_mutex_(),
      counter_(0),
      num_threads_(partest::TestSuite::GetDefaultNumThreads()),
      num_iterations_(partest::TestSuite::GetDefaultNumIterations()) {
  CreateUnit("Shared read")
      .Add(&SharedMutexTest::TestSharedRead_ThreadMethod, this,
           num_threads_, num_iterations_);
  CreateUnit("Exclusive write")
      .Pre(&SharedMutexTest::TestExclusiveWrite_Pre, this)
      .Add(&SharedMutexTest::TestExclusiveWrite_ReaderMethod, this,
           num_threads_ / 2, num_iterations_)
      .Add(&SharedMutexTest::TestExclusiveWrite_WriterMethod, this,
           num_threads_ / 2, num_iterations_)
      .Post(&SharedMutexTest::TestExclusiveWrite_Post, this);
  CreateUnit("SharedLock")
      .Add(&SharedMutexTest::TestSharedLock_ThreadMethod, this,
           num_threads_, num_iterations_);
}

void SharedMutexTest::TestSharedRead_ThreadMethod() {
  SharedLock<embb::base::SharedMutex> lock(shared_mutex_, embb::base::try_lock);
  PT_ASSERT_EQ_MSG(lock.OwnsLock(), true, "Failed to lock for reading.");

  int spin = 10000;
  while (--spin != 0);
}

void SharedMutexTest::TestExclusiveWrite_Pre() {
  counter_ = 0;
}

void SharedMutexTest::TestExclusiveWrite_ReaderMethod() {
  // Just add some contention
  SharedLock<embb::base::SharedMutex> lock(shared_mutex_, embb::base::try_lock);
  
  if (lock.OwnsLock()) {
    int spin = 10000;
    while (--spin != 0);
  }
}

void SharedMutexTest::TestExclusiveWrite_WriterMethod() {
  UniqueLock<embb::base::SharedMutex> lock(shared_mutex_);

  ++counter_;
}

void SharedMutexTest::TestExclusiveWrite_Post() {
  PT_ASSERT_EQ_MSG(counter_, num_iterations_ * (num_threads_ / 2),
                   "Counter value is inconsistent.");
}

void SharedMutexTest::TestSharedLock_ThreadMethod() {
  // Test basic usage
  {
    SharedLock<> lock(shared_mutex_);
    PT_EXPECT_EQ(lock.OwnsLock(), true);

    lock.UnlockShared();
    PT_EXPECT_EQ(lock.OwnsLock(), false);

    lock.LockShared();
    PT_EXPECT_EQ(lock.OwnsLock(), true);

    lock.UnlockShared();
    PT_EXPECT_EQ(lock.OwnsLock(), false);

    bool locked_after_try = lock.TryLockShared();
    PT_EXPECT_EQ(locked_after_try, true);
    PT_EXPECT_EQ(lock.OwnsLock(), true);

    lock.Release()->UnlockShared();
    PT_EXPECT_EQ(lock.OwnsLock(), false);
  }

  // Test deferred lock construction
  {
    SharedLock<> lock(shared_mutex_, embb::base::defer_lock);
    PT_EXPECT_EQ(lock.OwnsLock(), false);
  }
  
  // Test try-lock construction
  {
    SharedLock<> lock(shared_mutex_, embb::base::try_lock);
    PT_EXPECT_EQ(lock.OwnsLock(), true);
  }
  
  // Test adopt lock construction
  {
    shared_mutex_.LockShared();
    SharedLock<> lock(shared_mutex_, embb::base::adopt_lock);
    PT_EXPECT_EQ(lock.OwnsLock(), true);
  }
  
  // Test lock swapping
  {
    SharedMutex another_mutex;
    SharedLock<> lock1(another_mutex);
    PT_EXPECT_EQ(lock1.OwnsLock(), true);

    {
      SharedLock<> lock2(shared_mutex_);
      PT_EXPECT_EQ(lock2.OwnsLock(), true);

      lock1.Swap(lock2);
      PT_EXPECT_EQ(lock1.OwnsLock(), true);
      PT_EXPECT_EQ(lock2.OwnsLock(), true);
    }

    // At this point, lock2 was destroyed and "another_mutex" must be unlocked
    SharedLock<> lock3(another_mutex, embb::base::try_lock);
    PT_EXPECT_EQ(lock3.OwnsLock(), true);

    // But lock1 must still be locking "shared_mutex_"
    PT_EXPECT_EQ(lock1.OwnsLock(), true);
    lock1.Release()->UnlockShared();
    PT_EXPECT_EQ(lock1.OwnsLock(), false);
  }
}

} // namespace test
} // namespace base
} // namespace embb
