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

#ifndef EMBB_CONTAINERS_INTERNAL_OBJECT_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_OBJECT_POOL_INL_H_

namespace embb {
namespace containers {
template<class Type, typename ValuePool, class ObjectAllocator>
ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::ReturningTrueIterator(size_t count_value) :
count_value(count_value),
  ret_value(true)
{}

template<class Type, typename ValuePool, class ObjectAllocator>
typename ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::self_type
ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator++() {
  self_type i = *this;
  count_value++;
  return i;
}

template<class Type, typename ValuePool, class ObjectAllocator>
typename ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::self_type ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator++(int) {
  count_value++;
  return *this;
}

template<class Type, typename ValuePool, class ObjectAllocator>
typename ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::reference ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator*() {
  return ret_value;
}

template<class Type, typename ValuePool, class ObjectAllocator>
typename ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::pointer ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator->() {
  return &ret_value;
}

template<class Type, typename ValuePool, class ObjectAllocator>
bool ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator==(const self_type& rhs) {
  return count_value == rhs.count_value;
}

template<class Type, typename ValuePool, class ObjectAllocator>
bool ObjectPool<Type, ValuePool, ObjectAllocator>::
ReturningTrueIterator::operator!=(const self_type& rhs) {
  return count_value != rhs.count_value;
}

template<class Type, typename ValuePool, class ObjectAllocator>
bool ObjectPool<Type, ValuePool, ObjectAllocator>::
IsContained(const Type &obj) const {
  if ((&obj < &objects_array_[0]) ||
    (&obj > &objects_array_[value_pool_size_ - 1])) {
    return false;
  } else {
    return true;
  }
}

template<class Type, typename ValuePool, class ObjectAllocator>
int ObjectPool<Type, ValuePool, ObjectAllocator>::
GetIndexOfObject(const Type &obj) const {
  assert(IsContained(obj));
  return(static_cast<int>(&obj - &objects_array_[0]));
}

template<class Type, typename ValuePool, class ObjectAllocator>
Type* ObjectPool<Type, ValuePool, ObjectAllocator>::AllocateRaw() {
  bool val;
  int allocated_index = value_pool_.Allocate(val);
  if (allocated_index == -1) {
    return NULL;
  } else {
    Type* ret_pointer = &(objects_array_[allocated_index]);

    return ret_pointer;
  }
}

template<class Type, typename ValuePool, class ObjectAllocator>
size_t ObjectPool<Type, ValuePool, ObjectAllocator>::GetCapacity() {
  return capacity_;
}

template<class Type, typename ValuePool, class ObjectAllocator>
ObjectPool<Type, ValuePool, ObjectAllocator>::ObjectPool(size_t capacity) :
  capacity_(capacity),
  value_pool_size_(
  ValuePool::GetMinimumElementCountForGuaranteedCapacity(capacity)),
  value_pool_(ReturningTrueIterator(0), ReturningTrueIterator(
  value_pool_size_)),
  objects_array_(object_allocator_.allocate(value_pool_size_)) {
}

template<class Type, typename ValuePool, class ObjectAllocator>
void ObjectPool<Type, ValuePool, ObjectAllocator>::Free(Type* obj) {
  int index = GetIndexOfObject(*obj);
  obj->~Type();

  value_pool_.Free(true, index);
}

template<class Type, typename ValuePool, class ObjectAllocator>
Type* ObjectPool<Type, ValuePool, ObjectAllocator>::Allocate() {
  Type* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)Type();

  return rawObject;
}

template<class Type, typename ValuePool, class ObjectAllocator>
template<typename Param1>
Type* ObjectPool<Type, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1) {
  Type* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)Type(param1);

  return rawObject;
}

template<class Type, typename ValuePool, class ObjectAllocator>
template<typename Param1, typename Param2>
Type* ObjectPool<Type, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1, Param2 const& param2) {
  Type* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)Type(param1, param2);

  return rawObject;
}

template<class Type, typename ValuePool, class ObjectAllocator>
template<typename Param1, typename Param2, typename Param3>
Type* ObjectPool<Type, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1, Param2 const& param2,
  Param3 const& param3) {
  Type* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)Type(param1, param2, param3);

  return rawObject;
}

template<class Type, typename ValuePool, class ObjectAllocator>
template<typename Param1, typename Param2, typename Param3, typename Param4>
Type* ObjectPool<Type, ValuePool, ObjectAllocator>::Allocate(
  Param1 const& param1, Param2 const& param2,
  Param3 const& param3, Param4 const& param4) {
  Type* rawObject = AllocateRaw();
  if (rawObject != NULL)
    new (rawObject)Type(param1, param2, param3, param4);

  return rawObject;
}

template<class Type, typename ValuePool, class ObjectAllocator>
ObjectPool<Type, ValuePool, ObjectAllocator>::~ObjectPool() {
  // Destroy still allocated objects
  typename ValuePool::Iterator it = value_pool_.Begin();
  for (; it != value_pool_.End(); ++it) {
    objects_array_[(*it).first].~Type();
  }
  // Deallocate the objects
  object_allocator_.deallocate(objects_array_, value_pool_size_);
}
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_OBJECT_POOL_INL_H_
