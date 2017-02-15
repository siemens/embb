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

#ifndef EMBB_CONTAINERS_INTERNAL_WAIT_FREE_ARRAY_VALUE_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_WAIT_FREE_ARRAY_VALUE_POOL_INL_H_

#include <utility>

namespace embb {
namespace containers {

template<typename Type, Type Undefined, class Allocator >
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::Iterator()
  : pool_(NULL)
  , index_(0) {
  // emtpy
}

template<typename Type, Type Undefined, class Allocator >
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::Iterator(WaitFreeArrayValuePool * pool)
  : pool_(pool)
  , index_(0) {
  Advance();
}

template<typename Type, Type Undefined, class Allocator >
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::Iterator(WaitFreeArrayValuePool * pool, int index)
  : pool_(pool)
  , index_(index) {
  Advance();
}

template<typename Type, Type Undefined, class Allocator >
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::Iterator(Iterator const & other)
  : pool_(other.pool_)
  , index_(other.index_) {
  // empty
}

template<typename Type, Type Undefined, class Allocator >
typename WaitFreeArrayValuePool<Type, Undefined, Allocator>::Iterator &
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::operator =(Iterator const & other) {
  pool_ = other.pool_;
  index_ = other.index_;
  return *this;
}

template<typename Type, Type Undefined, class Allocator >
void
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::Advance() {
  if (NULL != pool_) {
    while (index_ < pool_->size_ && pool_->pool_array_[index_] != Undefined) {
      index_++;
    }
  } else {
    index_ = 0;
  }
}

template<typename Type, Type Undefined, class Allocator >
typename WaitFreeArrayValuePool<Type, Undefined, Allocator>::Iterator &
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::operator ++() {
  index_++;
  Advance();
  return *this;
}

template<typename Type, Type Undefined, class Allocator >
typename WaitFreeArrayValuePool<Type, Undefined, Allocator>::Iterator
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::operator ++(int) {
  Iterator tmp(*this);
  operator++();
  return tmp;
}

template<typename Type, Type Undefined, class Allocator >
std::pair<int, Type>
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::operator *() {
  return std::make_pair(index_, pool_->pool_array_[index_].Load());
}

template<typename Type, Type Undefined, class Allocator >
bool
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::operator ==(Iterator const & rhs) {
  return (pool_ == rhs.pool_) && (index_ == rhs.index_);
}

template<typename Type, Type Undefined, class Allocator >
bool
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Iterator::operator !=(Iterator const & rhs) {
  return (pool_ != rhs.pool_) || (index_ != rhs.index_);
}

template<typename Type, Type Undefined, class Allocator >
typename WaitFreeArrayValuePool<Type, Undefined, Allocator>::Iterator
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Begin() {
  return Iterator(this);
}

template<typename Type, Type Undefined, class Allocator >
typename WaitFreeArrayValuePool<Type, Undefined, Allocator>::Iterator
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
End() {
  return Iterator(this, size_);
}

template<typename Type, Type Undefined, class Allocator >
void WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Free(Type element, int index) {
  assert(element != Undefined);

  // Just put back the element
  pool_array_[index].Store(element);
}

template<typename Type, Type Undefined, class Allocator >
int WaitFreeArrayValuePool<Type, Undefined, Allocator>::
Allocate(Type & element) {
  for (int i = 0; i != size_; ++i) {
    Type expected;

    // If the memory cell is not available, go ahead
    if (Undefined == (expected = pool_array_[i].Load()))
      continue;

    // Try to get the memory cell
    if (pool_array_[i].CompareAndSwap(expected, Undefined)) {
      // When the CAS was successful, this element is ours
      element = expected;
      return i;
    }
  }
  element = Type();
  return -1;
}

template<typename Type, Type Undefined, class Allocator >
template<typename ForwardIterator>
WaitFreeArrayValuePool<Type, Undefined, Allocator>::
WaitFreeArrayValuePool(ForwardIterator first, ForwardIterator last) {
  size_t dist = static_cast<size_t>(std::distance(first, last));

  size_ = static_cast<int>(dist);

  // conversion may result in negative number. check!
  assert(size_ >= 0);

  // Use the allocator to allocate an array of size dist
  pool_array_ = allocator_.allocate(dist);

  // invoke inplace new for each pool element
  for ( size_t i = 0; i != dist; ++i ) {
    new (&pool_array_[i]) embb::base::Atomic<Type>();
  }

  int i = 0;

  // Store the elements of the range
  for (ForwardIterator curIter(first); curIter != last; ++curIter) {
    pool_array_[i++] = *curIter;
  }
}

template<typename Type, Type Undefined, class Allocator >
WaitFreeArrayValuePool<Type, Undefined, Allocator>::~WaitFreeArrayValuePool() {
  // invoke destructor for each pool element
  for (int i = 0; i != size_; ++i) {
    pool_array_[i].~Atomic();
  }

  // free memory
  allocator_.deallocate(pool_array_, static_cast<size_t>(size_));
}

template<typename Type, Type Undefined, class Allocator >
size_t WaitFreeArrayValuePool<Type, Undefined, Allocator>::
GetMinimumElementCountForGuaranteedCapacity(size_t capacity) {
  // for this value pool, this is just capacity...
  return capacity;
}

} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_WAIT_FREE_ARRAY_VALUE_POOL_INL_H_
