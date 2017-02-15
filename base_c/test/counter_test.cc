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

#include <counter_test.h>
#include <embb/base/c/base.h>
#include <iostream>

namespace embb {
namespace base {
namespace test {

CounterTest::CounterTest() {
  CreateUnit("Single threaded API test").Add(&CounterTest::TestBase, this);
  CreateUnit<TestStress>();
}

void CounterTest::TestBase() {
  embb_counter_t counter;
  embb_counter_init(&counter);

  unsigned int value = embb_counter_get(&counter);
  PT_EXPECT_EQ(value, static_cast<unsigned int>(0));
  value = embb_counter_increment(&counter);
  PT_EXPECT_EQ(value, static_cast<unsigned int>(0));
  value = embb_counter_get(&counter);
  PT_EXPECT_EQ(value, static_cast<unsigned int>(1));
  value = embb_counter_decrement(&counter);
  PT_EXPECT_EQ(value, static_cast<unsigned int>(1));
  value = embb_counter_get(&counter);
  PT_EXPECT_EQ(value, static_cast<unsigned int>(0));
  embb_counter_destroy(&counter);
}

CounterTest::TestStress::TestStress()
    : TestUnit("Stress test for incrementing and decrementing"), counter_() {
  size_t num_threads = partest::TestSuite::GetDefaultNumThreads();
  size_t num_iterations = partest::TestSuite::GetDefaultNumIterations();
  Pre(&TestStress::Init, this);
  Add(&TestStress::TestCounterIncrement, this, num_threads, num_iterations);
  Add(&TestStress::TestCounterDecrement, this, num_threads, num_iterations);
  Post(&TestStress::CheckAndDestroyCounter, this);
}

void CounterTest::TestStress::CheckAndDestroyCounter() {
  unsigned int value = embb_counter_get(&counter_);
  PT_EXPECT_EQ(value, static_cast<unsigned int>(0));
  embb_counter_destroy(&counter_);
}

} // namespace test
} // namespace base
} // namespace embb


