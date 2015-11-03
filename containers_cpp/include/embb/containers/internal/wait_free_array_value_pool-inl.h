/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#ifndef EMBB_CONTAINERS_INTERNAL_WAIT_FREE_ARRAY_VALUE_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_WAIT_FREE_ARRAY_VALUE_POOL_INL_H_

namespace embb {
namespace containers {
template<typename Type, Type Undefined, class Allocator >
void WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Free(Type element, int index) {
  assert(element != Undefined);

  // Just put back the element
  pool[index].Store(element);
}

template<typename Type, Type Undefined, class Allocator >
int WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Allocate(Type & element) {
  for (int i = 0; i != size; ++i) {
    Type expected;

    // If the memory cell is not available, go ahead
    if (Undefined == (expected = pool[i].Load()))
      continue;

    // Try to get the memory cell
    if (pool[i].CompareAndSwap(expected, Undefined)) {
      // When the CAS was successful, this element is ours
      element = expected;
      return i;
    }
  }
  return -1;
}

template<typename Type, Type Undefined, class Allocator >
template<typename ForwardIterator>
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
WaitFreeArrayValuePool(ForwardIterator first, ForwardIterator last) {
  size_t dist = static_cast<size_t>(std::distance(first, last));

  size = static_cast<int>(dist);

  // Use the allocator to allocate an array of size dist
  pool = allocator.allocate(dist);

  int i = 0;

  // Store the elements of the range
  for (ForwardIterator curIter(first); curIter != last; ++curIter) {
    pool[i++] = *curIter;
  }
}

template<typename Type, Type Undefined, class Allocator >
WaitFreeArrayValuePool<Type, Undefined, Allocator>::~WaitFreeArrayValuePool() {
  allocator.deallocate(pool, (size_t)size);
}
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_WAIT_FREE_ARRAY_VALUE_POOL_INL_H_
