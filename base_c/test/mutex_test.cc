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

SpinLockTest::SpinLockTest() : counter_(0),
  number_threads_(partest::TestSuite::GetDefaultNumThreads()),
  number_iterations_(partest::TestSuite::GetDefaultNumIterations()),
#ifdef EMBB_THREADING_ANALYSIS_MODE
  counter_iterations_(10) {
#else
  counter_iterations_(10000) {
#endif
  CreateUnit("Protected counter using Lock")
  .Pre(&SpinLockTest::PreSpinLockInc, this)
  .Add(&SpinLockTest::TestSpinLockIncUseLock, this,
  number_threads_,
  number_iterations_)
  .Post(&SpinLockTest::PostSpinLockInc, this);

  CreateUnit("Protected counter using TryLock")
  .Pre(&SpinLockTest::PreSpinLockInc, this)
  .Add(&SpinLockTest::TestSpinLockIncUseTryLock, this,
  number_threads_,
  number_iterations_)
  .Post(&SpinLockTest::PostSpinLockInc, this);

  /* Disable tests that assume non-recursive behavior */
#if !(defined(EMBB_THREADING_ANALYSIS_MODE) && \
  defined(EMBB_PLATFORM_THREADING_WINTHREADS))
  CreateUnit("Test spinning (too many spins), single thread")
    .Add(&SpinLockTest::TestSpinLockTooManySpins, this,
    // one thread
    1,
    // one iteration
    1);
#endif
}

void SpinLockTest::TestSpinLockTooManySpins() {
  embb_spin_init(&spinlock_);
  embb_spin_lock(&spinlock_);

  int return_code = embb_spin_try_lock(&spinlock_, 100);
  PT_ASSERT(return_code == EMBB_BUSY);

  embb_spin_unlock(&spinlock_);

  return_code = embb_spin_try_lock(&spinlock_, 100);
  PT_ASSERT(return_code == EMBB_SUCCESS);

  embb_spin_unlock(&spinlock_);

  embb_spin_destroy(&spinlock_);
}

void SpinLockTest::PreSpinLockInc() {
  embb_spin_init(&spinlock_);
}

void SpinLockTest::TestSpinLockIncUseLock() {
  for (unsigned int i = 0; i != counter_iterations_; ++i) {
    embb_spin_lock(&spinlock_);
    counter_++;
    embb_spin_unlock(&spinlock_);
  }
}

void SpinLockTest::TestSpinLockIncUseTryLock() {
  for (unsigned int i = 0; i != counter_iterations_; ++i) {
    while (embb_spin_try_lock(&spinlock_, 100) != EMBB_SUCCESS) {}
    counter_++;
    embb_spin_unlock(&spinlock_);
  }
}

void SpinLockTest::PostSpinLockInc() {
  embb_spin_destroy(&spinlock_);
  PT_EXPECT_EQ(counter_, number_iterations_ *
    number_threads_*
    counter_iterations_);
  counter_ = 0;
}

} // namespace test
} // namespace base
} // namespace embb
