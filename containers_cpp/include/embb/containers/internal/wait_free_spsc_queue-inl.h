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

#ifndef EMBB_CONTAINERS_INTERNAL_WAIT_FREE_SPSC_QUEUE_INL_H_
#define EMBB_CONTAINERS_INTERNAL_WAIT_FREE_SPSC_QUEUE_INL_H_

/*
 * The following algorithm is described in:
 * Maurice Herlihy and Nir Shavit. "The Art of Multiprocessor Programming."
 * Page 46. Morgan Kaufmann, 2008. (original: L. Lamport. "Specifying concurrent
 * programs").
 */

namespace embb {
namespace containers {
template<typename Type, class Allocator>
size_t WaitFreeSPSCQueue<Type, Allocator>::
AlignCapacityToPowerOfTwo(size_t capacity) {
  size_t result = 1;
  while (result < capacity) result <<= 1;
  return result;
}

template<typename Type, class Allocator>
WaitFreeSPSCQueue<Type, Allocator>::WaitFreeSPSCQueue(size_t capacity)
    : capacity(AlignCapacityToPowerOfTwo(capacity)),
      head_index(0),
      tail_index(0) {
  queue_array = allocator.allocate(this->capacity);
}

template<typename Type, class Allocator>
size_t WaitFreeSPSCQueue<Type, Allocator>::GetCapacity() {
  return capacity;
}

template<typename Type, class Allocator>
bool WaitFreeSPSCQueue<Type, Allocator>::TryEnqueue(Type const & element) {
  if (tail_index - head_index == capacity)
    return false;

  queue_array[tail_index % capacity] = element;
  this->tail_index++;
  return true;
}

template<typename Type, class Allocator>
bool WaitFreeSPSCQueue<Type, Allocator>::TryDequeue(Type & element) {
  if (tail_index - head_index == 0)
    return false;

  Type x = queue_array[head_index % capacity];
  this->head_index++;
  element = x;
  return true;
}

template<typename Type, class Allocator>
WaitFreeSPSCQueue<Type, Allocator>::~WaitFreeSPSCQueue() {
  allocator.deallocate(queue_array, capacity);
}
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_WAIT_FREE_SPSC_QUEUE_INL_H_
