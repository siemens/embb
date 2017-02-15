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

#ifndef BASE_C_TEST_THREAD_INDEX_TEST_H_
#define BASE_C_TEST_THREAD_INDEX_TEST_H_

#include <partest/partest.h>

namespace embb {
namespace base {
namespace test {

class ThreadIndexTest : public partest::TestCase {
 public:
  /**
   * Adds test methods and allocates temporary data.
   */
  ThreadIndexTest();

  /**
   * Destroys temporary data.
   */
  ~ThreadIndexTest();

 private:
  /**
   * Tests 0 available indices.
   */
  void Test0();

  /**
   * Tests 1 available index.
   */
  void Test1();

  /**
   * Tests number_thread_ available indices and starting threads at same time.
   */
  void TestN();

 private:
  /**
   * Configurable number of threads (and indices) used in TestN().
   */
  size_t number_threads_;
};

/**
 * Thread start method, performs checks with retrieving thread index.
 *
 * An own thread start is used to ensure that the thread does not yet have an
 * index. This is necessary, since partest assigns thread indices before the
 * threads are calling the user test functions (when using EMBB for it).
 */
int ThreadStart(void* arg);

} // namespace test
} // namespace base
} // namespace embb



#endif  // BASE_C_TEST_THREAD_INDEX_TEST_H_
