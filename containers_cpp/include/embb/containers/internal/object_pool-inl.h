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

#ifndef EMBB_CONTAINERS_INTERNAL_OBJECT_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_OBJECT_POOL_INL_H_

namespace embb {
namespace containers {
template<class T, typename ValuePool, class ObjectAllocator>
ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::ReturningTrueIterator(size_t count_value) :
count_value(count_value),
  ret_value(true)
{}

template<class T, typename ValuePool, class ObjectAllocator>
typename ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::self_type
ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator++() {
  self_type i = *this;
  count_value++;
  return i;
}

template<class T, typename ValuePool, class ObjectAllocator>
typename ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::self_type ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator++(int junk) {
  count_value++;
  return *this;
}

template<class T, typename ValuePool, class ObjectAllocator>
typename ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::reference ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator*() {
  return ret_value;
}

template<class T, typename ValuePool, class ObjectAllocator>
typename ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::pointer ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator->() {
  return &ret_value;
}

template<class T, typename ValuePool, class ObjectAllocator>
bool ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator==(const self_type& rhs) {
  return count_value == rhs.count_value;
}

template<class T, typename ValuePool, class ObjectAllocator>
bool ObjectPool<T, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator!=(const self_type& rhs) {
  return count_value != rhs.count_value;
}

template<class T, typename ValuePool, class ObjectAllocator>
bool ObjectPool<T, ValuePool, ObjectAllocator>::
IsContained(const T &obj) const {
  if ((&obj < &objects[0]) || (&obj > &objects[capacity - 1])) {
    return false;
  } else {
    return true;
  }
}

template<class T, typename ValuePool, class ObjectAllocator>
int ObjectPool<T, ValuePool, ObjectAllocator>::
GetIndexOfObject(const T &obj) const {
  assert(IsContained(obj));
  return(static_cast<int>(&obj - &objects[0]));
}

template<class T, typename ValuePool, class ObjectAllocator>
T* ObjectPool<T, ValuePool, ObjectAllocator>::AllocateRaw() {
  bool val;
  int allocated_index = p.Allocate(val);
  if (allocated_index == -1) {
    return NULL;
  } else {
    T* ret_pointer = &(objects[allocated_index]);

    return ret_pointer;
  }
}

template<class T, typename ValuePool, class ObjectAllocator>
size_t ObjectPool<T, ValuePool, ObjectAllocator>::GetCapacity() {
  return capacity;
}

template<class T, typename ValuePool, class ObjectAllocator>
ObjectPool<T, ValuePool, ObjectAllocator>::ObjectPool(size_t capacity) :
capacity(capacity),
  p(ReturningTrueIterator(0), ReturningTrueIterator(capacity)) {
  // Allocate the objects (without construction, just get the memory)
  objects = objectAllocator.allocate(capacity);
}

template<class T, typename ValuePool, class ObjectAllocator>
void ObjectPool<T, ValuePool, ObjectAllocator>::Free(T* obj) {
  int index = GetIndexOfObject(*obj);
  obj->~T();

  p.Free(true, index);
}

template<class T, typename ValuePool, class ObjectAllocator>
T* ObjectPool<T, ValuePool, ObjectAllocator>::Allocate() {
  T* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)T();

  return rawObject;
}

template<class T, typename ValuePool, class ObjectAllocator>
template<typename Param1>
T* ObjectPool<T, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1) {
  T* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)T(param1);

  return rawObject;
}

template<class T, typename ValuePool, class ObjectAllocator>
template<typename Param1, typename Param2>
T* ObjectPool<T, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1, Param2 const& param2) {
  T* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)T(param1, param2);

  return rawObject;
}

template<class T, typename ValuePool, class ObjectAllocator>
template<typename Param1, typename Param2, typename Param3>
T* ObjectPool<T, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1, Param2 const& param2,
  Param3 const& param3) {
  T* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)T(param1, param2, param3);

  return rawObject;
}

template<class T, typename ValuePool, class ObjectAllocator>
template<typename Param1, typename Param2, typename Param3, typename Param4>
T* ObjectPool<T, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1, Param2 const& param2,
  Param3 const& param3, Param4 const& param4) {
  T* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)T(param1, param2, param3, param4);

  return rawObject;
}

template<class T, typename ValuePool, class ObjectAllocator>
ObjectPool<T, ValuePool, ObjectAllocator>::~ObjectPool() {
  // Deallocate the objects
  objectAllocator.deallocate(objects, capacity);
}
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_OBJECT_POOL_INL_H_
