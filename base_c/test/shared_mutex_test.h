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

#ifndef BASE_C_TEST_SHARED_MUTEX_TEST_H_
#define BASE_C_TEST_SHARED_MUTEX_TEST_H_

#include <partest/partest.h>
#include <embb/base/c/internal/platform.h>
#include <embb/base/c/mutex.h>


namespace embb {
namespace base {
namespace test {

class SharedMutexTest : public partest::TestCase {
 public:
  SharedMutexTest();

 private:
  /**
   * Only readers.
   *
   * The readers use try_lock_shared() and test if that always obtains the
   * shared lock.
   */
  void TestSharedReadPre();
  void TestSharedReadThreadMethod();
  void TestSharedReadPost();

  /**
   * Multiple readers and writers.
   *
   * The writers use lock() and test if that always obtains the lock (after
   * waiting). The readers use lock_shared() and test if that always obtains the
   * shared lock (after waiting).
   */
  void TestMultipleWriterPre();
  void TestMultipleWriterReaderMethod();
  void TestMultipleWriterWriterMethod();
  void TestMultipleWriterPost();

  /**
   * Multiple readers, single writer.
   *
   * The single writer uses try_lock() and tests if that always obtains the
   * lock. The readers use lock_shared() and test if that always obtains the
   * shared lock (after waiting).
   */
  void TestSingleWriterPre();
  void TestSingleWriterReaderMethod();
  void TestSingleWriterWriterMethod();
  void TestSingleWriterPost();

  embb_shared_mutex_t shared_mutex_;
  size_t counter_;
  size_t num_threads_;
  size_t num_iterations_;
};

} // namespace test
} // namespace base
} // namespace embb

#endif  // BASE_C_TEST_SHARED_MUTEX_TEST_H_
