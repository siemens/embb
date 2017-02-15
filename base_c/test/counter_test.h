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

#ifndef BASE_C_TEST_COUNTER_TEST_H_
#define BASE_C_TEST_COUNTER_TEST_H_

#include <partest/partest.h>
#include <embb/base/c/counter.h>

namespace embb {
namespace base {
namespace test {

/**
 * Provides tests for C counter functionality.
 */
class CounterTest : public partest::TestCase {
 public:
  /**
   * Adds test units.
   */
  CounterTest();

 private:
  /**
   * Checks the correct return values of inc., dec., and get methods.
   */
  void TestBase();

  /**
   * Test repeated incrementing and decrement by several threads.
   */
  class TestStress : public partest::TestUnit {
   public:
    /**
     * Adds test methods to unit.
     */
    TestStress();

   private:
    /**
     * Inits the counter.
     */
    void Init() {
      embb_counter_init(&counter_);
    }
    /**
     * Increases the counter.
     */
    void TestCounterIncrement() {
      embb_counter_increment(&counter_);
    }

    /**
     * Decreases the counter.
     */
    void TestCounterDecrement() {
      embb_counter_decrement(&counter_);
    }

    /**
     * Checks the value of the counter and deletes the counter.
     */
    void CheckAndDestroyCounter();

    /**
     * Counter used in tests.
     */
    embb_counter_t counter_;
  };
};

} // namespace test
} // namespace base
} // namespace embb

#endif  // BASE_C_TEST_COUNTER_TEST_H_
