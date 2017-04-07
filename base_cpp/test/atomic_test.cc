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

#include <atomic_test.h>
#include <embb/base/atomic.h>
#include <embb/base/thread.h>
#include <iostream>

namespace embb {
namespace base {
namespace test {

size_t AtomicTest::numIterations_;

void AtomicTest::TestStressLoadStore::barrier(int thread) {
  const int my_sense(thread_sense[thread]);
  if (--count == 0) {
    const int tmp(z.Load());
    PT_EXPECT(tmp == 1 || tmp == 2);
    x.Store(0);
    y.Store(0);
    z.Store(0);
    count.Store(4);
    sense.Store(my_sense);
  } else {
    while (sense.Load() != my_sense) {}
  }
  thread_sense[thread] = !my_sense;
}

void AtomicTest::TestStressLoadStore::write_x() {
  x.Store(1);
  barrier(0);
}

void AtomicTest::TestStressLoadStore::write_y() {
  y.Store(1);
  barrier(1);
}

void AtomicTest::TestStressLoadStore::read_x_then_y() {
  while (!x.Load()) {}
  if (y.Load())
    z++;
  barrier(2);
}

void AtomicTest::TestStressLoadStore::read_y_then_x() {
  while (!y.Load()) {}
  if (x.Load())
    z++;
  barrier(3);
}

void AtomicTest::TestStressLoadStore::Init() {
}

void AtomicTest::TestStressLoadStore::CheckAndDestroy() {
}

AtomicTest::TestStressLoadStore::TestStressLoadStore(
  size_t number_threads, size_t number_iterations)
  : TestUnit("Load/Store Stress test for Atomics"), x(0), y(0), z(0),
  count(4), sense(0) {
  PT_ASSERT(number_threads == 1);
  thread_sense[0] = 1;
  thread_sense[1] = 1;
  thread_sense[2] = 1;
  thread_sense[3] = 1;
  Pre(&TestStressLoadStore::Init, this);
  Add(&TestStressLoadStore::write_x, this,
    number_threads, number_iterations);
  Add(&TestStressLoadStore::write_y, this,
    number_threads, number_iterations);
  Add(&TestStressLoadStore::read_x_then_y, this,
    number_threads, number_iterations);
  Add(&TestStressLoadStore::read_y_then_x, this,
    number_threads, number_iterations);
  Post(&TestStressLoadStore::CheckAndDestroy, this);
}

AtomicTest::TestStressProduceConsume::TestStressProduceConsume(
  size_t number_threads, size_t number_iterations)
  : TestUnit("Produce/Consume Stress test for Atomics")
  , prod_cons_value(0), flag(0)
  , counter_producer(0), counter_consumer(0) {
  PT_ASSERT(number_threads == 1);
  Pre(&TestStressProduceConsume::Init, this);

  Add(&TestStressProduceConsume::produce, this,
    number_threads, number_iterations);
  Add(&TestStressProduceConsume::consume, this,
    number_threads, number_iterations);

  Post(&TestStressProduceConsume::CheckAndDestroy, this);
}

void AtomicTest::TestStressProduceConsume::produce() {
  counter_consumer++;
  while (flag) {}
  prod_cons_value =  static_cast<size_t>(counter_consumer);
  while (flag.Swap(true)) {}
}

void AtomicTest::TestStressProduceConsume::consume() {
  counter_producer++;
  while (flag == 0) {}
  PT_EXPECT(prod_cons_value == static_cast<size_t>(counter_producer));
  flag = 0;
}

void AtomicTest::TestStressProduceConsume::Init() {
}

void AtomicTest::TestStressProduceConsume::CheckAndDestroy() {
}

AtomicTest::TestStressIncrementDecrement::TestStressIncrementDecrement(
  size_t number_threads, size_t number_iterations)
  : TestUnit("Increment/Decrement Stress test for Atomics"), inc_dec_value(0) {
  PT_ASSERT(number_threads == 1);
  Pre(&TestStressIncrementDecrement::Init, this);

  Add(&TestStressIncrementDecrement::increment, this,
    number_threads, number_iterations);
  Add(&TestStressIncrementDecrement::decrement, this,
    number_threads, number_iterations);

  Post(&TestStressIncrementDecrement::CheckAndDestroy, this);
}

void AtomicTest::TestStressIncrementDecrement::decrement() {
  inc_dec_value--;
}

void AtomicTest::TestStressIncrementDecrement::increment() {
  inc_dec_value++;
}

void AtomicTest::TestStressIncrementDecrement::Init() {
}

void AtomicTest::TestStressIncrementDecrement::CheckAndDestroy() {
  // Increment and decrement operations must neutralize each other
  PT_EXPECT(inc_dec_value == 0);
}



AtomicTest::TestStressSwap::TestStressSwap(
  size_t number_threads, size_t number_iterations)
  : TestUnit("Swap Stress test for Atomics"), swap1_counter(1)
  , swap2_counter(2) {
  PT_ASSERT(number_threads == 1);
  Pre(&TestStressSwap::Init, this);

  Add(&TestStressSwap::swap1, this,
    number_threads, number_iterations);
  Add(&TestStressSwap::swap2, this,
    number_threads, number_iterations);

  Post(&TestStressSwap::CheckAndDestroy, this);
}

void AtomicTest::TestStressSwap::Init() {
}

void AtomicTest::TestStressSwap::CheckAndDestroy() {
  // Each element is read at most once
  bitsets[2] = bitsets[0];
  bitsets[2] &= bitsets[1];
  PT_EXPECT(bitsets[2].none());
  bitsets[2] = bitsets[0];
  bitsets[2] |= bitsets[1];
  // All elements except one are read (push, pop, push and so on... the last
  // push is not read), however this must not always be the same element.
  PT_EXPECT(bitsets[2].count() ==
      static_cast<size_t>(AtomicTest::GetNumberOfIterations() * 2));
}

void AtomicTest::TestStressSwap::swap1() {
  const int j(swap_value.Swap(swap1_counter));
  PT_EXPECT(!(bitsets[0].test(static_cast<size_t>(j))));
  bitsets[0].set(static_cast<size_t>(j));
  swap1_counter += 2;
}

void AtomicTest::TestStressSwap::swap2() {
  const int j(swap_value.Swap(swap2_counter));
  PT_EXPECT(!(bitsets[1].test(static_cast<size_t>(j))));
  bitsets[1].set(static_cast<size_t>(j));
  swap2_counter += 2;
}

AtomicTest::TestStressCompareAndSwap::TestStressCompareAndSwap(
  size_t number_threads, size_t number_iterations)
  : TestUnit("Compare and Swap Stress test for Atomics"), swap1_counter(1)
  , swap2_counter(2) {
  PT_ASSERT(number_threads == 1);
  Pre(&TestStressCompareAndSwap::Init, this);

  Add(&TestStressCompareAndSwap::compare_and_swap1, this,
    number_threads, number_iterations);
  Add(&TestStressCompareAndSwap::compare_and_swap2, this,
    number_threads, number_iterations);

  Post(&TestStressCompareAndSwap::CheckAndDestroy, this);
}

void AtomicTest::TestStressCompareAndSwap::Init() {
}

void AtomicTest::TestStressCompareAndSwap::CheckAndDestroy() {
  // Each element is read at most once
  bitsets[2] = bitsets[0];
  bitsets[2] &= bitsets[1];
  PT_EXPECT(bitsets[2].none());
  bitsets[2] = bitsets[0];
  bitsets[2] |= bitsets[1];
  // All elements except one are read (push, pop, push and so on... the last
  // push is not read), however this must not always be the same element.
  PT_EXPECT(bitsets[2].count() ==
      static_cast<unsigned int>(AtomicTest::GetNumberOfIterations() * 2));
}

void AtomicTest::TestStressCompareAndSwap::compare_and_swap1() {
  int j(0);
  while (!swap_value.CompareAndSwap(j, swap1_counter)) {}
  PT_EXPECT(!(bitsets[0].test(static_cast<size_t>(j))));
  bitsets[0].set(static_cast<size_t>(j));
  swap1_counter += 2;
}

void AtomicTest::TestStressCompareAndSwap::compare_and_swap2() {
  int j(0);
  while (!swap_value.CompareAndSwap(j, swap2_counter)) {}
  PT_EXPECT(!(bitsets[1].test(static_cast<size_t>(j))));
  bitsets[1].set(static_cast<size_t>(j));
  swap2_counter += 2;
}



AtomicTest::AtomicTest() {
  numIterations_ = partest::TestSuite::GetDefaultNumIterations();
  PT_ASSERT_LT_MSG(numIterations_,
    static_cast<size_t>(ATOMIC_TESTS_ITERATIONS), "Maximum allowed iterations");
  CreateUnit("BasicTestsSingleThreaded")
    .Add(&AtomicTest::BasicTests, this);
  CreateUnit<TestStressLoadStore>(static_cast<size_t>(1), numIterations_);
  CreateUnit<TestStressProduceConsume>(static_cast<size_t>(1), numIterations_);
  CreateUnit<TestStressIncrementDecrement>(static_cast<size_t>(1),
    numIterations_);
  CreateUnit<TestStressSwap>(static_cast<size_t>(1), numIterations_);
  CreateUnit<TestStressCompareAndSwap>(static_cast<size_t>(1), numIterations_);
}

typedef enum { RED, GREEN, BLUE } colors_t;

void AtomicTest::BasicTests() {
  embb::base::Atomic<bool> b;          // Boolean
  embb::base::Atomic<colors_t> c;        // Enumeration
  embb::base::Atomic<void*> v;         // Void pointer
  embb::base::Atomic<int> i;           // Integer
  embb::base::Atomic<int*> n;          // Non-void pointer

  //template specializations
  PT_EXPECT(!b.IsArithmetic() && !b.IsInteger() && !b.IsPointer());
  PT_EXPECT(!c.IsArithmetic() && !c.IsInteger() && !c.IsPointer());
  PT_EXPECT(!v.IsArithmetic() && !v.IsInteger() && !v.IsPointer());
  PT_EXPECT(i.IsArithmetic() && i.IsInteger() && !i.IsPointer());
  PT_EXPECT(n.IsArithmetic() && !n.IsInteger() && n.IsPointer());


  // Constructors
  PT_EXPECT(c == RED);
  colors_t d(GREEN);
  PT_EXPECT(d == GREEN);
  // Assignment
  PT_EXPECT((c = GREEN) == GREEN);
  PT_EXPECT(c == GREEN);
  // Swap
  PT_EXPECT(c.Swap(BLUE) == GREEN);
  PT_EXPECT(c == BLUE);
  // Compare-and-swap

  d = RED;
  PT_EXPECT(!c.CompareAndSwap(d, GREEN));
  PT_EXPECT(d == BLUE);
  PT_EXPECT(c.CompareAndSwap(d, GREEN));
  PT_EXPECT(c == GREEN);

  //Arithmetic opertions...
  PT_EXPECT(i == 0);
  // Fetch-and-add
  PT_EXPECT(i.FetchAndAdd(10) == 0);
  PT_EXPECT(i == 10);
  // Fetch-and-sub
  PT_EXPECT(i.FetchAndSub(5) == 10);
  PT_EXPECT(i == 5);
  // Increment (postfix)
  PT_EXPECT(i++ == 5);
  PT_EXPECT(i == 6);
  // Decrement (postfix)
  PT_EXPECT(i-- == 6);
  PT_EXPECT(i == 5);
  // Increment (prefix)
  PT_EXPECT(++i == 6);
  PT_EXPECT(i == 6);
  // Decrement (prefix)
  PT_EXPECT(--i == 5);
  PT_EXPECT(i == 5);
  // Addition
  PT_EXPECT((i += 10) == 15);
  PT_EXPECT(i == 15);
  // Subtraction
  PT_EXPECT((i -= 10) == 5);
  PT_EXPECT(i == 5);

  //Boolean operations...
  // And
  i = 0;
  i &= 1;
  PT_EXPECT(i == 0);
  i = 1;
  i &= 1;
  PT_EXPECT(i == 1);
  // Or
  i = 1;
  i |= 0;
  PT_EXPECT(i == 1);
  i |= 1;
  PT_EXPECT(i == 1);
  i = 0;
  i |= 0;
  PT_EXPECT(i == 0);
  // Xor
  i = 0;
  i ^= 0;
  PT_EXPECT(i == 0);
  i ^= 1;
  PT_EXPECT(i == 1);

  //Pointers...;
  n = NULL;
  // Stride
  PT_EXPECT((uintptr_t)++n == sizeof(int));
  // Dereferencing
  n = new int(0x13579BDF);
  PT_EXPECT(*n == 0x13579BDF);
  delete n;

  // Scalar values
  embb::base::Atomic<int> *j = new embb::base::Atomic<int>();
  PT_EXPECT(*j == 0);
  int *k = new int(0);
  PT_EXPECT(j->CompareAndSwap(*k, 0x13579BDF));
  PT_EXPECT(*j == 0x13579BDF);
  delete j;
  delete k;
}

} // namespace test
} // namespace base
} // namespace embb
