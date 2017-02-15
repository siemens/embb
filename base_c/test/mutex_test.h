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

#ifndef BASE_C_TEST_MUTEX_TEST_H_
#define BASE_C_TEST_MUTEX_TEST_H_

#include <partest/partest.h>
#include <embb/base/c/internal/platform.h>
#include <embb/base/c/mutex.h>


namespace embb {
namespace base {
namespace test {
/**
 * Provides tests for class Mutex.
 */
class MutexTest : public partest::TestCase {
 public:
  /**
   * Constructs the test case and adds test units.
   */
  MutexTest();

 private:
  /**
   * Prepares TestMutexIncCpp.
   */
  void PreMutexInc();
  /**
   * Tests mutex locking and unlocking to protect shared counter.
   */
  void TestMutexInc();
  /**
   * Checks and tears down TestMutexIncCpp.
   */
  void PostMutexInc();

  /**
   * Tests the multiple locking and unlocking of a recursive mutex.
   */
  void TestRecursiveMutex();

  /**
   * Mutex for tests.
   */
  embb_mutex_t mutex_;

  /**
   * Shared counter to check effectiveness of mutex.
   */
  size_t counter_;

  /**
   * Number of threads used to run tests.
   */
  size_t number_threads_;

  /**
   * Number of times the test method is called by each thread.
   */
  size_t number_iterations_;
};


class SpinLockTest : public partest::TestCase {
 public:
  SpinLockTest();

 private:
  /**
   * Check that the try lock fails, when lock is already set.
   */
  void TestSpinLockTooManySpins();

  /**
   * Prepares TestMutexIncCpp.
   */
  void PreSpinLockInc();

  /**
   * Tests mutex locking and unlocking to protect shared counter.
   */
  void TestSpinLockIncUseLock();

  /**
   * Tests mutex locking and unlocking to protect shared counter using trylock.
   */
  void TestSpinLockIncUseTryLock();

  /**
   * Checks and tears down TestMutexIncCpp.
   */
  void PostSpinLockInc();

  /**
   * Shared counter to check effectiveness of mutex.
   */
  size_t counter_;

  /**
   * Number of threads used to run tests.
   */
  size_t number_threads_;

  /**
   * Number of times the test method is called by each thread.
   */
  size_t number_iterations_;

  /**
   * Number of internal iterations, for incrementing the counter.
   */
  size_t counter_iterations_;

  /**
   * The used spinlock
   */
  embb_spinlock_t spinlock_;
};


} // namespace test
} // namespace base
} // namespace embb

#endif  // BASE_C_TEST_MUTEX_TEST_H_
