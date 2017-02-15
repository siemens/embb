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

#include <condition_var_test.h>
#include <embb/base/condition_variable.h>
#include <iostream>

#include <partest/test_method.h>
#include <partest/test_instance_info.h>


namespace embb {
namespace base {
namespace test {

ConditionVarTest::ConditionVarTest()
    : num_threads_(partest::TestSuite::GetDefaultNumThreads()) {
  embb_counter_init(&counter_);
  CreateUnit("Timed wait timouts")
      .Add(&ConditionVarTest::TestTimedWaitTimeouts, this);

  if (num_threads_ >= 2) {
    CreateUnit("Condition Notify Test")
        .Add(&ConditionVarTest::TestNotify, this, num_threads_);
  } else {
     std::cout <<
         "Warning: Condition Notify Test needs a minimum of two threads"
               << std::endl;
  }
}

void ConditionVarTest::TestTimedWaitTimeouts() {
  // Set up data structures
  ConditionVariable cond;
  Mutex mutex;
  UniqueLock<Mutex> lock(mutex);

  // Wait for now tests already passed time point
  bool success = cond.WaitUntil(lock, Time());
  PT_EXPECT_EQ(success, false);

  // Wait for a future timepoint
  success = cond.WaitUntil(lock, Time(DurationMilliseconds(1)));
  PT_EXPECT_EQ(success, false);

  // Wait for a zero duration
  success = cond.WaitFor(lock, DurationMilliseconds());
  PT_EXPECT_EQ(success, false);

  // Wait for some duration
  success = cond.WaitFor(lock, DurationMilliseconds(1));
  PT_EXPECT_EQ(success, false);
}

void ConditionVarTest::TestNotify() {
  size_t threadID = partest::TestSuite::GetCurrentThreadID();

  if (threadID != 0) {
    UniqueLock<Mutex> lock(mutex_cond_notify_);
    embb_counter_increment(&counter_);
    cond_notify_.Wait(lock);
    embb_counter_increment(&counter_);
  } else {
    while (embb_counter_get(&counter_) <
        static_cast<unsigned int>(num_threads_-1))
    {}  // all threads entered critical section
    UniqueLock<Mutex> lock_notify(mutex_cond_notify_);
    lock_notify.Unlock();
    // All threads called wait on the condition (Even last thread)

    embb_counter_reset(&counter_);
    UniqueLock<Mutex> lock_wait(mutex_cond_wait_);

    cond_notify_.NotifyOne();

    cond_wait_.WaitUntil(lock_wait, Time(DurationMilliseconds(1)));
    while (embb_counter_get(&counter_) == 0)
    {}  // If hangs here signal has not succeeded
    PT_ASSERT_EQ_MSG(embb_counter_get(&counter_),
        static_cast<unsigned int>(1), "Only 1 thread notified");
    cond_notify_.NotifyAll();

    cond_wait_.WaitUntil(lock_wait, Time(DurationMilliseconds(2)));

    while (embb_counter_get(&counter_) !=
        static_cast<unsigned int>(num_threads_-1))
    {}  // If this hangs then not all threads were notified.
  }
}

}  // namespace test
}  // namespace base
}  // namespace embb



