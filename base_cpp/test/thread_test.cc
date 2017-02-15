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
#include <embb/base/thread.h>
#include <embb/base/mutex.h>
#include <partest/test_unit.h>
#include <partest/test_case.h>
#include <partest/test_suite.h>
#include <partest/test_instance_info.h>
#include <partest/partest.h>
#include <iostream>

namespace embb {
namespace base {
namespace test {

const double ThreadTest::eps_ = 1e-15;

ThreadTest::ThreadTest()
  : number_threads_(partest::TestSuite::GetDefaultNumThreads()) {
  CreateUnit("Starting and joining")
      .Pre(&ThreadTest::PreStartingAndJoining, this)
      .Add(&ThreadTest::TestStartingAndJoining, this, number_threads_);
  CreateUnit("Affinities")
      .Add(&ThreadTest::TestThreadAffinities, this);
}

void ThreadTest::PreStartingAndJoining() {
  embb::base::Thread::SetThreadsMaxCount(
    static_cast<unsigned int>(6 * (number_threads_+1)));
}

void ThreadTest::TestStartingAndJoining() {
  // Static member start methods
  embb::base::Thread thread1(ThreadTest::StaticThreadStart);
  int arg1 = 2;
  embb::base::Thread thread2(ThreadTest::StaticThreadStartArg1, arg1);
  double arg2 = 3.0;
  embb::base::Thread thread3(ThreadTest::StaticThreadStartArg2, arg1, arg2);
  embb::base::Thread::ID thread3id;
  thread3id = thread3.GetID();
  PT_EXPECT_EQ(thread3id, thread3.GetID());

  // Non-static member start methods with functor
  MemberStart<void(ThreadTest::*)()> start4(&ThreadTest::ThreadStart, this);
  embb::base::Thread thread4(start4);

  std::string arg3("StringArgument");
  MemberStartArg1<void(ThreadTest::*)(const std::string&),
                  const std::string&> start5(&ThreadTest::ThreadStartArg1,
                                             this, arg3);
  embb::base::Thread thread5(start5);

  std::vector<int> arg4;
  arg4.push_back(1);
  arg4.push_back(2);
  double arg5 = 5.0;
  MemberStartArg2<void(ThreadTest::*)(const std::vector<int>, double&),
                  const std::vector<int>, double&> start6(
                      &ThreadTest::ThreadStartArg2, this, arg4, arg5);
  embb::base::Thread thread6(start6);

  thread1.Join();
  thread2.Join();
  thread3.Join();
  thread4.Join();
  thread5.Join();
  thread6.Join();
}

void ThreadTest::TestThreadAffinities() {
  embb::base::CoreSet core_set(true);
  bool success = true;
  EMBB_TRY {
    embb::base::Thread thread(core_set, StaticThreadStart);
    thread.Join();
  }
  EMBB_CATCH(embb::base::Exception&) {
    success = false;
  }
  PT_EXPECT_EQ(success, true);
}

void ThreadTest::StaticThreadStart() {
}

void ThreadTest::StaticThreadStartArg1(int arg1) {
  PT_EXPECT_EQ(arg1, 2);
}

void ThreadTest::StaticThreadStartArg2(int arg1, double arg2) {
  PT_EXPECT_EQ(arg1, 2);
  PT_EXPECT_GT(arg2 - 3.0 + eps_, 0.0);
}

void ThreadTest::ThreadStart() {
}

void ThreadTest::ThreadStartArg1(const std::string& arg1) {
  PT_EXPECT_EQ(arg1, std::string("StringArgument"));
}

void ThreadTest::ThreadStartArg2(std::vector<int> arg1, double& arg2) {
  PT_EXPECT_EQ(arg1[0], 1);
  PT_EXPECT_EQ(arg1[1], 2);
  PT_EXPECT_GT(arg2 - 5.0 + eps_, 0.0);
}

} // namespace test
} // namespace base
} // namespace embb

