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

namespace embb {
namespace base {
namespace test {

ConditionVarTest::ConditionVarTest()
:num_threads_(partest::TestSuite::GetDefaultNumThreads()) {
  embb_counter_init(&counter_);
  embb_condition_init(&cond_notify_);
  embb_mutex_init(&mutex_cond_notify_, EMBB_MUTEX_PLAIN);
  embb_condition_init(&cond_wait_);
  embb_mutex_init(&mutex_cond_wait_, EMBB_MUTEX_PLAIN);

  CreateUnit("Timed wait timeouts")
      .Add(&ConditionVarTest::TestTimedWaitTimeouts, this);
  if (num_threads_ >= 2) {
    CreateUnit("Condition Notify Test")
         .Add(&ConditionVarTest::TestNotify, this, num_threads_);
  } else {
    std::cout << "Warning: Condition Notify Test needs a minimum of two threads"
        << std::endl;
  }
}

void ConditionVarTest::TestNotify() {
  size_t threadID =  partest::TestSuite::GetCurrentThreadID();

  if (threadID != 0) {
    embb_mutex_lock(&mutex_cond_notify_);
    embb_counter_increment(&counter_);
    embb_condition_wait(&cond_notify_, &mutex_cond_notify_);
    embb_mutex_unlock(&mutex_cond_notify_);
    embb_counter_increment(&counter_);
  } else {
    embb_duration_t duration = EMBB_DURATION_INIT;
    embb_duration_set_milliseconds(&duration, 1);

    while (embb_counter_get(&counter_)
        < static_cast<unsigned int>(num_threads_-1))
    {}  // All threads entered critical section
    embb_mutex_lock(&mutex_cond_notify_);
    embb_mutex_unlock(&mutex_cond_notify_);
    // All threads called wait on the condition (even last thread)

    embb_counter_reset(&counter_);

    embb_condition_notify_one(&cond_notify_);
    embb_mutex_lock(&mutex_cond_wait_);
    embb_condition_wait_for(&cond_wait_, &mutex_cond_wait_, &duration);
    while (embb_counter_get(&counter_) == 0)
    {} // If test hangs here, signalling has not succeeded
    PT_ASSERT_EQ_MSG(embb_counter_get(&counter_), static_cast<unsigned int>(1),
        "Only one thread notified");

    embb_condition_notify_all(&cond_notify_);

    embb_condition_wait_for(&cond_wait_, &mutex_cond_wait_, &duration);

    while (embb_counter_get(&counter_) !=
        static_cast<unsigned int>(num_threads_-1))
    {} // If test hangs here, not all threads were notified

    embb_mutex_unlock(&mutex_cond_wait_);
    embb_mutex_destroy(&mutex_cond_wait_);
    embb_mutex_destroy(&mutex_cond_notify_);
    embb_condition_destroy(&cond_wait_);
    embb_condition_destroy(&cond_notify_);
  }
}


void ConditionVarTest::TestTimedWaitTimeouts() {
  // Set up data structures
  embb_condition_t cond;
  embb_mutex_t mutex;
  embb_condition_init(&cond);
  embb_mutex_init(&mutex, EMBB_MUTEX_PLAIN);
  embb_time_t time;
  embb_duration_t duration = EMBB_DURATION_INIT;

  // Wait for "now" tests already passed time point
  embb_time_now(&time);
  embb_mutex_lock(&mutex);
  int status = embb_condition_wait_until(&cond, &mutex, &time);
  PT_EXPECT_EQ(status, EMBB_TIMEDOUT);

  // Wait for a future time point
  status = embb_duration_set_milliseconds(&duration, 1);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  status = embb_time_in(&time, &duration); // Time now
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  status = embb_condition_wait_until(&cond, &mutex, &time);
  PT_EXPECT_EQ(status, EMBB_TIMEDOUT);

  // Wait for a zero duration
  status = embb_duration_set_milliseconds(&duration, 0);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  status = embb_condition_wait_for(&cond, &mutex, &duration);
  PT_EXPECT_EQ(status, EMBB_TIMEDOUT);

  // Wait for some duration
  status = embb_duration_set_milliseconds(&duration, 1);
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
  status = embb_condition_wait_for(&cond, &mutex, &duration);
  PT_EXPECT_EQ(status, EMBB_TIMEDOUT);

  // Tear down data structures
  embb_mutex_unlock(&mutex);
  embb_mutex_destroy(&mutex);
  embb_condition_destroy(&cond);
}

} // namespace test
} // namespace base
} // namespace embb
