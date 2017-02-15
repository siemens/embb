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

#include <thread_specific_storage_test.h>
#include <embb/base/thread.h>
#include <iostream>

namespace embb {
namespace base {
namespace test {

ThreadSpecificStorageTest::ThreadSpecificStorageTest()
    : number_threads_(partest::TestSuite::GetDefaultNumThreads()), tss_() {
  PT_EXPECT_GT(Thread::GetThreadsMaxCount(), number_threads_);
  CreateUnit("Internal representation")
      .Add(&ThreadSpecificStorageTest::TestInternalRepresentation, this,
           1, partest::TestSuite::GetDefaultNumIterations());
  CreateUnit("Multiple TSS variables")
      .Add(&ThreadSpecificStorageTest::TestMultipleTSSVariables, this);
  CreateUnit("TestConstructors")
      .Add(&ThreadSpecificStorageTest::TestConstructors, this);
}

void ThreadSpecificStorageTest::TestInternalRepresentation() {
  embb_internal_thread_index_reset();
  size_t num_threads = partest::TestSuite::GetDefaultNumThreads();
  embb::base::Thread** threads = new embb::base::Thread*[num_threads];
  for (size_t i = 0; i < num_threads; i++) {
    threads[i] = new embb::base::Thread(
                 ThreadSpecificStorageTest::TestInternalRepresentationSetGet,
                 &tss_, i);
  }
  for (size_t i = 0; i < num_threads; i++) {
    threads[i]->Join();
    delete threads[i];
  }
  delete[] threads;

  // Check results
  int sum = 0;
  for (size_t i = 0; i < tss_.rep_.size; i++) {
    if (i < num_threads) {
      PT_EXPECT_EQ(tss_.usage_flags_[i], true);
      void* value = tss_.rep_.values[i];
      sum += *static_cast<int*>(value);
    } else {
      PT_EXPECT_EQ(tss_.usage_flags_[i], false);
    }
  }
  int expected_sum = 0;
  for (size_t i = 0; i < num_threads; i++) {
    expected_sum += static_cast<int>(i);
  }
  PT_EXPECT_EQ(sum, expected_sum);
}

void ThreadSpecificStorageTest::TestInternalRepresentationSetGet(
    ThreadSpecificStorage<size_t>* tss, size_t rank) {
  assert(tss != NULL);
  PT_EXPECT_NE(tss->rep_.values, static_cast<void*>(NULL));
  PT_EXPECT_EQ(embb_thread_get_max_count(), tss->rep_.size);
  PT_EXPECT_LT(rank, tss->rep_.size);
  for (unsigned int i = 0; i < tss->rep_.size; i++) {
    PT_EXPECT_NE(tss->rep_.values[i], static_cast<void*>(NULL));
  }
  tss->Get() = rank;
  for (size_t i = 0; i < 10; i++) {
    size_t stored_rank = tss->Get();
    PT_EXPECT_EQ(rank, stored_rank);
    if (rank != stored_rank) break;
  }
}

void ThreadSpecificStorageTest::TestMultipleTSSVariables() {
  ThreadSpecificStorage<size_t> tss2;
  PT_EXPECT_NE(&(tss_.Get()), &(tss2.Get()));
}

void ThreadSpecificStorageTest::TestConstructors() {
  {
    ThreadSpecificStorage<OneArgumentConstructorType> tss(1);
    PT_EXPECT_EQ(tss.Get().var, 1);
  }
  {
    ThreadSpecificStorage<TwoArgumentConstructorType> tss(1, 2);
    PT_EXPECT_EQ(tss.Get().var1, 1);
    PT_EXPECT_EQ(tss.Get().var2, 2);
  }
  {
    ThreadSpecificStorage<ThreeArgumentConstructorType> tss(1, 2, 3);
    PT_EXPECT_EQ(tss.Get().var1, 1);
    PT_EXPECT_EQ(tss.Get().var2, 2);
    PT_EXPECT_EQ(tss.Get().var3, 3);
  }
  {
    ThreadSpecificStorage<FourArgumentConstructorType> tss(1, 2, 3, 4);
    PT_EXPECT_EQ(tss.Get().var1, 1);
    PT_EXPECT_EQ(tss.Get().var2, 2);
    PT_EXPECT_EQ(tss.Get().var3, 3);
    PT_EXPECT_EQ(tss.Get().var4, 4);
  }
}

} // namespace test
} // namespace base
} // namespace embb
