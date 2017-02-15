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

#include <thread_test.h>
#include <embb/base/c/base.h>

namespace embb {
namespace base {
namespace test {

ThreadTest::ThreadTest() {
  CreateUnit("Starting and joining")
    .Add(&ThreadTest::TestStartingAndJoining, this,
      partest::TestSuite::GetDefaultNumThreads());
  CreateUnit("Affinities")
    .Add(&ThreadTest::TestThreadAffinities, this, 1,
      partest::TestSuite::GetDefaultNumIterations());
}

void ThreadTest::TestStartingAndJoining() {
  { // Run thread with no argument and do not retrieve return value
    embb_thread_t thread;
    int status = embb_thread_create(&thread, NULL, ThreadStartFunction, NULL);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    status = embb_thread_join(&thread, NULL);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
  { // Run thread with no argument and do retrieve return value
    embb_thread_t thread;
    int status = embb_thread_create(&thread, NULL, ThreadStartFunction, NULL);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    int return_value = -1;
    status = embb_thread_join(&thread, &return_value);
    PT_EXPECT_EQ(return_value, 0);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
  { // Run thread with argument and do retrieve return value
    embb_thread_t thread;
    int arg = 1;
    int status = embb_thread_create(&thread, NULL, ThreadStartFunction, &arg);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
    int return_value = -1;
    status = embb_thread_join(&thread, &return_value);
    PT_EXPECT_EQ(return_value, 1);
    PT_EXPECT_EQ(status, EMBB_SUCCESS);
  }
}

void ThreadTest::TestThreadAffinities() {
  embb_core_set_t core_set;
  embb_core_set_init(&core_set, 1);
  embb_thread_t thread;
  int status =
    embb_thread_create(&thread, &core_set, ThreadStartFunction, NULL);
  embb_thread_join(&thread, NULL);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
}

int ThreadStartFunction(void* arg) {
  int return_value = 0;
  if (arg != NULL) {
    return_value = *static_cast<int*>(arg);
  }
  return return_value;
}

} // namespace test
} // namespace base
} // namespace embb
