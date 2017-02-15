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

#include <memory_allocation_test.h>
#include <stdint.h>

namespace embb {
namespace base {
namespace test {

MemoryAllocationTest::MemoryAllocationTest() {
  CreateUnit("ClassAllocationTest")
    .Add(&MemoryAllocationTest::ClassAllocationTest, this);
  CreateUnit("AllocatorTest").Add(&MemoryAllocationTest::AllocatorTest, this);
}

void MemoryAllocationTest::AllocatorTest() {
  static const int alloc_iterations = 10000;

  ::std::vector < DummyClassForAllocatorTests,
    embb::base::AllocatorCacheAligned< DummyClassForAllocatorTests > >
    aligned_allocating_vector;

  ::std::vector < DummyClassForAllocatorTests,
    embb::base::Allocator< DummyClassForAllocatorTests > >
    unaligned_allocating_vector;

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(alloc_iterations);
    ++i) {
    DummyClassForAllocatorTests d;
    d.A = 0xF10000 | i;
    d.F = 0xF20000 | i;
    aligned_allocating_vector.push_back(d);
    unaligned_allocating_vector.push_back(d);
  }

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(alloc_iterations);
    ++i) {
    unsigned int A = 0xF10000 | i;
    unsigned int F = 0xF20000 | i;
    PT_ASSERT_EQ(aligned_allocating_vector[i].A, A);
    PT_ASSERT_EQ(aligned_allocating_vector[i].F, F);
    PT_ASSERT_EQ(unaligned_allocating_vector[i].A, A);
    PT_ASSERT_EQ(unaligned_allocating_vector[i].F, F);
  }
}

void MemoryAllocationTest::ClassAllocationTest() {
  ::std::vector< DummyClassForAlignedAllocation* > aligned_allocs;
  ::std::vector< DummyClassForUnalignedAllocation* > unaligned_allocs;

  size_t expected = 0;

  static const unsigned int alloc_iterations = 10;

  for (unsigned int i = 0; i != alloc_iterations; ++i) {
    aligned_allocs.push_back( new DummyClassForAlignedAllocation );
    //write something
    aligned_allocs[i]->b = 0xF000 | i;
#ifdef EMBB_DEBUG
    size_t n = (sizeof(DummyClassForAlignedAllocation)
      + (EMBB_PLATFORM_CACHE_LINE_SIZE - 1)) / EMBB_PLATFORM_CACHE_LINE_SIZE;
    expected += (n + 1)*EMBB_PLATFORM_CACHE_LINE_SIZE +
      (sizeof(size_t) * 3 - 1);
#endif // else EMBB_DEBUG

    //check that the memory is aligned!
    PT_ASSERT_EQ((uintptr_t)aligned_allocs[i] % EMBB_PLATFORM_CACHE_LINE_SIZE,
      (uintptr_t)0);
  }

  PT_ASSERT_EQ(embb::base::Allocation::AllocatedBytes(), expected);

  //delete!
  expected = 0;
  for (unsigned int i = 0; i != alloc_iterations; ++i) {
    //check if written correctly
    PT_ASSERT_EQ(aligned_allocs[i]->b, 0xF000 | i);
    delete aligned_allocs[i];
  }

  //everything should be deleted
  PT_ASSERT_EQ(embb::base::Allocation::AllocatedBytes(), expected);

  for (unsigned int i = 0; i != alloc_iterations; ++i) {
    unaligned_allocs.push_back(new DummyClassForUnalignedAllocation);
    //write something
    unaligned_allocs[i]->b = 0xA000 | i;


#ifdef EMBB_DEBUG
    expected += sizeof(DummyClassForUnalignedAllocation) + 2 * sizeof(size_t);
#endif // else EMBB_DEBUG
  }

  PT_ASSERT_EQ(embb::base::Allocation::AllocatedBytes(), expected);

  //delete!
  expected = 0;
  for (unsigned int i = 0; i != alloc_iterations; ++i) {
    //check if written correctly
    PT_ASSERT_EQ(unaligned_allocs[i]->b, 0xA000 | i);
    delete unaligned_allocs[i];
  }

  //everything should be deleted
  PT_ASSERT_EQ(embb::base::Allocation::AllocatedBytes(), expected);


  DummyClassForAlignedAllocation * aligned_allocated =
    new DummyClassForAlignedAllocation[alloc_iterations];
#ifdef EMBB_DEBUG
    size_t n = (sizeof(DummyClassForAlignedAllocation)*alloc_iterations
      + (EMBB_PLATFORM_CACHE_LINE_SIZE - 1)) / EMBB_PLATFORM_CACHE_LINE_SIZE;
    expected += (n + 1)*EMBB_PLATFORM_CACHE_LINE_SIZE +
      (sizeof(size_t) * 3 - 1);
#endif // else EMBB_DEBUG

    // This assert does _not_ hold, but is left for documentation.
    // It is not guaranteed that the pointer to the array is aligned.
    // See the documentation of the overloaded new[] operator in
    // class MemoryAllocation.
    // PT_ASSERT_EQ((uintptr_t)aligned_allocated %
    //   EMBB_PLATFORM_CACHE_LINE_SIZE, 0);

    //delete!
    expected = 0;
    delete[] aligned_allocated;

    PT_ASSERT_EQ(embb::base::Allocation::AllocatedBytes(), expected);

    DummyClassForUnalignedAllocation * unaligned_allocated =
      new DummyClassForUnalignedAllocation[alloc_iterations];
#ifdef EMBB_DEBUG
    expected += sizeof(DummyClassForUnalignedAllocation)*alloc_iterations
      + 2 * sizeof(size_t);
#endif // else EMBB_DEBUG

    PT_ASSERT(embb::base::Allocation::AllocatedBytes() >= expected);

    //delete!
    expected = 0;
    delete[] unaligned_allocated;

    PT_ASSERT_EQ(embb::base::Allocation::AllocatedBytes(), expected);
}

} // namespace test
} // namespace base
} // namespace embb
