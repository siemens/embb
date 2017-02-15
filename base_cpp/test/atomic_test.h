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

#ifndef BASE_CPP_TEST_ATOMIC_TEST_H_
#define BASE_CPP_TEST_ATOMIC_TEST_H_

#include <partest/partest.h>
#include <embb/base/atomic.h>
#include <iostream>
#include <bitset>

//we need that statically, otherwise we cannot use bitset in test
#define ATOMIC_TESTS_ITERATIONS 10000

namespace embb {
namespace base {
namespace test {

class AtomicTest : public partest::TestCase {
 public:
  /**
  * Adds test methods.
  */
  AtomicTest();

  static size_t GetNumberOfIterations() {
    return numIterations_;
  }

 private:
  static size_t numIterations_;
  void BasicTests();


  class TestStressLoadStore : public partest::TestUnit {
   public:
    TestStressLoadStore(size_t number_threads, size_t number_iterations);

   private:
    void barrier(int thread);
    void write_x();
    void write_y();
    void read_x_then_y();
    void read_y_then_x();

    embb::base::Atomic<int> x, y;
    embb::base::Atomic<int> z;

    // A sense-reversing barrier
    // See M. Herlihy and N. Shavit. The Art of Multiprocessor Programming.
    // Morgan Kaufmann, 2008.

    embb::base::Atomic<int> count;
    embb::base::Atomic<int> sense;

    // Do not use a std::vector here, since std::vector<bool> may pack
    // multiple elements into a single word. As a result, they cannnot be
    // modified independently by multiple threads.
    bool thread_sense[4];

    void Init();
    void CheckAndDestroy();
  };

  class TestStressProduceConsume : public partest::TestUnit {
   public:
    TestStressProduceConsume(size_t number_threads, size_t number_iterations);

   private:
    size_t prod_cons_value;
    embb::base::Atomic<int> flag;
    int counter_producer;
    int counter_consumer;

    void produce();
    void consume();

    void Init();
    void CheckAndDestroy();
  };

  class TestStressIncrementDecrement : public partest::TestUnit {
   public:
    TestStressIncrementDecrement(size_t number_threads, size_t number_iterations);

   private:
    embb::base::Atomic<int> inc_dec_value;

    void increment();
    void decrement();

    void Init();
    void CheckAndDestroy();
  };

  class TestStressSwap : public partest::TestUnit {
   public:
    TestStressSwap(size_t number_threads, size_t number_iterations);

   private:
    std::bitset<ATOMIC_TESTS_ITERATIONS*2+1> bitsets[3];
    embb::base::Atomic<int> swap_value;

    int swap1_counter;
    int swap2_counter;

    void swap1();
    void swap2();

    void Init();
    void CheckAndDestroy();
  };

  class TestStressCompareAndSwap : public partest::TestUnit {
   public:
    TestStressCompareAndSwap(size_t number_threads, size_t number_iterations);

   private:
    std::bitset<ATOMIC_TESTS_ITERATIONS * 2 + 1> bitsets[3];
    embb::base::Atomic<int> swap_value;

    int swap1_counter;
    int swap2_counter;

    void compare_and_swap1();
    void compare_and_swap2();
    void Init();
    void CheckAndDestroy();
  };
};

} // namespace test
} // namespace base
} // namespace embb

#endif // BASE_CPP_TEST_ATOMIC_TEST_H_
