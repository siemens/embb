/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_CONTAINERS_INTERNAL_INDEXED_OBJECT_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_INDEXED_OBJECT_POOL_INL_H_

#include <embb/containers/internal/returning_true_iterator.h>

namespace embb {
namespace containers {
namespace internal {

template<typename Type, class IndexPool, class Allocator>
template<typename RAI>
IndexedObjectPool<Type, IndexPool, Allocator>::
IndexedObjectPool(RAI first, RAI last) :
  size_(static_cast<size_t>(std::distance(first, last))),
  indexPool(internal::ReturningTrueIterator(0),
            internal::ReturningTrueIterator(size_)) {
  // use the allocator to allocate array of size dist
  elements = allocator.allocate(size_);
  // fill element pool with elements from the iteration
  int i = 0;
  for (RAI curIter(first); curIter != last; ++curIter, ++i) {
    // assign element from iteration
    elements[i] = *curIter;
  }
}

template<typename Type, class IndexPool, class Allocator>
IndexedObjectPool<Type, IndexPool, Allocator >::
IndexedObjectPool(size_t size, const Type & defaultInstance) :
  size_(size),
  indexPool(internal::ReturningTrueIterator(0),
            internal::ReturningTrueIterator(size_)) {
  // use the allocator to allocate array of size dist
  elements = allocator.allocate(size);
  // fill element pool with elements from the iteration
  for (size_t i = 0; i < size_; ++i) {
    // initialize element from default constructor and
    // assignment operator
    elements[i] = defaultInstance;
  }
}

template<typename Type, class IndexPool, class Allocator>
IndexedObjectPool<Type, IndexPool, Allocator >::
~IndexedObjectPool() {
  allocator.deallocate(elements, size_);
}

template<typename Type, class IndexPool, class Allocator>
void IndexedObjectPool<Type, IndexPool, Allocator >::
Free(int elementIndex) {
  // Call the referenced element's destructor:
  elements[elementIndex].~Type();
  // Release index of the element for reuse:
  indexPool.Free(true, elementIndex);
}

template<typename Type, class IndexPool, class Allocator>
Type & IndexedObjectPool<Type, IndexPool, Allocator >::
operator[](int elementIndex) {
  return elements[elementIndex];
}

template<class Type, typename ValuePool, class ObjectAllocator>
int IndexedObjectPool<Type, ValuePool, ObjectAllocator>::AllocateRaw(
  Type * & newElement) {
  newElement = NULL;
  // Reserve a pool index:
  bool val;
  int allocated_index = indexPool.Allocate(val);
  if (allocated_index >= 0) {
    // Return pointer to reserved element:
    Type * ret_pointer = &(elements[allocated_index]);
    newElement = ret_pointer;
  }
  return allocated_index;
}

template<typename Type, class IndexPool, class Allocator>
int IndexedObjectPool<Type, IndexPool, Allocator >::
Allocate() {
  Type * raw_object = NULL;
  int element_index = AllocateRaw(raw_object);
  if (element_index >= 0 && raw_object != NULL) {
    new (raw_object)Type();
  }
  return element_index;
}

template<typename Type, class IndexPool, class Allocator>
template<typename Param1>
int IndexedObjectPool<Type, IndexPool, Allocator >::Allocate(
  Param1 const & param1) {
  Type * raw_object = NULL;
  int element_index = AllocateRaw(raw_object);
  if (element_index >= 0 && raw_object != NULL) {
    new (raw_object)Type(param1);
  }
  return element_index;
}

template<typename Type, class IndexPool, class Allocator>
template<typename Param1, typename Param2>
int IndexedObjectPool<Type, IndexPool, Allocator >::Allocate(
  Param1 const & param1, Param2 const & param2) {
  Type * raw_object = NULL;
  int element_index = AllocateRaw(raw_object);
  if (element_index >= 0 && raw_object != NULL) {
    new (raw_object)Type(param1, param2);
  }
  return element_index;
}

template<typename Type, class IndexPool, class Allocator>
template<typename Param1, typename Param2, typename Param3>
int IndexedObjectPool<Type, IndexPool, Allocator >::Allocate(
  Param1 const & param1, Param2 const & param2,
  Param3 const & param3) {
  Type * raw_object = NULL;
  int element_index = AllocateRaw(raw_object);
  if (element_index >= 0 && raw_object != NULL) {
    new (raw_object)Type(param1, param2, param3);
  }
  return element_index;
}

template<typename Type, class IndexPool, class Allocator>
template<typename Param1, typename Param2, typename Param3, typename Param4>
int IndexedObjectPool<Type, IndexPool, Allocator >::Allocate(
  Param1 const & param1, Param2 const & param2,
  Param3 const & param3, Param4 const & param4) {
  Type * raw_object = NULL;
  int element_index = AllocateRaw(raw_object);
  if (element_index >= 0 && raw_object != NULL) {
    new (raw_object)Type(param1, param2, param3, param4);
  }
  return element_index;
}

} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_INDEXED_OBJECT_POOL_INL_H_
