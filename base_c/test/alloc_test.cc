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

#include <alloc_test.h>
#include <embb/base/c/memory_allocation.h>
#include <cassert>
#include <string.h>

namespace embb {
namespace base {
namespace test {

AllocTest::AllocTest()
    : allocated_(partest::TestSuite::GetDefaultNumThreads()) {
  CreateUnit("Non-aligned")
    .Add(&AllocTest::TestNonAlignedAlloc, this,
         partest::TestSuite::GetDefaultNumThreads())
    .Post(&AllocTest::PostNonAligned, this);

  CreateUnit("Aligned")
    .Add(&AllocTest::TestAlignedAlloc, this,
         partest::TestSuite::GetDefaultNumThreads())
    .Post(&AllocTest::PostAligned, this);

  CreateUnit("Cache aligned")
    .Add(&AllocTest::TestCacheAlignedAlloc, this,
         partest::TestSuite::GetDefaultNumThreads())
    .Post(&AllocTest::PostCacheAligned, this);

  CreateUnit("AccessAllocatedMemory")
    .Add(&AllocTest::TestAccessAllocatedMemory, this)
    .Post(&AllocTest::PostAccessAllocatedMemory, this);

  CreateUnit("Mixed").Add(&AllocTest::TestMixedAllocs, this);
}

void AllocTest::TestAccessAllocatedMemory() {
  static const unsigned int memory_allocation_iterations = 1000;

  ::std::vector<void *> allocs_aligned;
  ::std::vector<void *> allocs_unaligned;

  for (unsigned int i = 0; i != memory_allocation_iterations; ++i) {
    allocs_aligned.push_back(embb_alloc_cache_aligned(8));
    allocs_unaligned.push_back(embb_alloc(8));
  }

  for (unsigned int i = 0; i != memory_allocation_iterations; ++i) {
    memcpy(allocs_aligned[i], &i, sizeof(int));
    memcpy(allocs_unaligned[i], &i, sizeof(int));
  }

  for (unsigned int i = 0; i != memory_allocation_iterations; ++i) {
    unsigned int alloc_aligned_test;
    unsigned int alloc_unaligned_test;
    memcpy(&alloc_aligned_test, allocs_aligned[i], sizeof(i));
    memcpy(&alloc_unaligned_test, allocs_unaligned[i], sizeof(i));

    PT_EXPECT_EQ(alloc_aligned_test, i);
    if (alloc_aligned_test != i) {
      PT_ASSERT(false);
    }
    if (alloc_unaligned_test != i) {
      PT_ASSERT(false);
    }
  }

  for (unsigned int i = 0; i != memory_allocation_iterations; ++i) {
    embb_free(allocs_unaligned[i]);
    embb_free_aligned(allocs_aligned[i]);
  }
}

void AllocTest::PostAccessAllocatedMemory() {
  size_t left = embb_get_bytes_allocated();
  PT_EXPECT_EQ(left, (size_t)0);
}

void AllocTest::TestNonAlignedAlloc() {
  size_t thread_num =
      partest::TestSuite::GetCurrentThreadID();
  assert(thread_num < allocated_.size());
  allocated_[thread_num] = NULL;
  allocated_[thread_num] = embb_alloc(1);
  PT_EXPECT_NE(allocated_[thread_num], static_cast<void*>(NULL));
  embb_free(allocated_[thread_num]);
}

void AllocTest::PostNonAligned() {
  size_t left = embb_get_bytes_allocated();
  PT_EXPECT_EQ(left, (size_t)0);
}

void AllocTest::TestAlignedAlloc() {
  size_t thread_num =
      partest::TestSuite::GetCurrentThreadID();
  assert(thread_num < allocated_.size());
  allocated_[thread_num] = NULL;
  allocated_[thread_num] = embb_alloc_aligned(2*sizeof(void*), 1);
  PT_EXPECT_NE(allocated_[thread_num], static_cast<void*>(NULL));
  embb_free_aligned(allocated_[thread_num]);
}

void AllocTest::PostAligned() {
  size_t left = embb_get_bytes_allocated();
  PT_EXPECT_EQ(left, (size_t)0);
}

void AllocTest::TestCacheAlignedAlloc() {
  size_t thread_num =
      partest::TestSuite::GetCurrentThreadID();
  assert(thread_num < allocated_.size());
  allocated_[thread_num] = NULL;
  allocated_[thread_num] = embb_alloc_cache_aligned(1);
  PT_EXPECT_NE(allocated_[thread_num], static_cast<void*>(NULL));
  embb_free_aligned(allocated_[thread_num]);
}

void AllocTest::PostCacheAligned() {
  size_t left = embb_get_bytes_allocated();
  PT_EXPECT_EQ(left, (size_t)0);
}

void AllocTest::TestMixedAllocs() {
  size_t expected = 0;
  size_t allocated = 0;

  // Plain memory allocation
  void* plain = NULL;
  plain = embb_alloc(2);
  PT_EXPECT_NE(plain, static_cast<void*>(NULL));
  if (NULL == plain) {
    return;
  }
  allocated = embb_get_bytes_allocated();
#ifdef EMBB_DEBUG
  expected += 2 + 2*sizeof(size_t);
#endif // else EMBB_DEBUG
  PT_EXPECT_EQ(allocated, expected);

  // Aligned memory allocation
  void* aligned = NULL;
  aligned = embb_alloc_aligned(2*sizeof(void*), 2);
  PT_EXPECT_NE(aligned, static_cast<void*>(NULL));
  if (NULL == aligned) {
    embb_free(plain);
    return;
  }
  allocated = embb_get_bytes_allocated();
#ifdef EMBB_DEBUG
  expected += (1 + 1) * 2 * sizeof(void*) + 3 * sizeof(size_t) - 1;
#endif // else EMBB_DEBUG
  PT_EXPECT_EQ(allocated, expected);

  // Cache-aligned memory allocation
  void* cache_aligned = NULL;
  cache_aligned = embb_alloc_cache_aligned(2);
  PT_EXPECT_NE(cache_aligned, static_cast<void*>(NULL));
  if (NULL == cache_aligned) {
    embb_free(plain);
    embb_free_aligned(aligned);
    return;
  }
  allocated = embb_get_bytes_allocated();
#ifdef EMBB_DEBUG
  expected += (1 + 1) * EMBB_PLATFORM_CACHE_LINE_SIZE + 3 * sizeof(size_t) - 1;
#endif // else EMBB_DEBUG
  PT_EXPECT_EQ(allocated, expected);

  embb_free(plain);
  embb_free_aligned(aligned);
  embb_free_aligned(cache_aligned);

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}

} // namespace test
} // namespace base
} // namespace embb
