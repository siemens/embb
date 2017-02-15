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

#ifndef EMBB_CONTAINERS_INTERNAL_LOCK_FREE_TREE_VALUE_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_LOCK_FREE_TREE_VALUE_POOL_INL_H_

#include <utility>

namespace embb {
namespace containers {

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::Iterator()
  : pool_(NULL)
  , index_(0) {
  // empty
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::Iterator(LockFreeTreeValuePool * pool)
  : pool_(pool)
  , index_(0) {
  Advance();
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::Iterator(LockFreeTreeValuePool * pool, int index)
  : pool_(pool)
  , index_(index) {
  Advance();
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::Iterator(Iterator const & other)
  : pool_(other.pool_)
  , index_(other.index_) {
  // empty
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
typename LockFreeTreeValuePool<Type, Undefined, PoolAllocator,
  TreeAllocator>::Iterator &
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::operator =(Iterator const & other) {
  pool_ = other.pool_;
  index_ = other.index_;
  return *this;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
void
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::Advance() {
  if (NULL != pool_) {
    while (index_ < pool_->real_size_ && pool_->pool_[index_] != Undefined) {
      index_++;
    }
  } else {
    index_ = 0;
  }
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
typename LockFreeTreeValuePool<Type, Undefined, PoolAllocator,
  TreeAllocator>::Iterator &
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::operator ++() {
  index_++;
  Advance();
  return *this;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
typename LockFreeTreeValuePool<Type, Undefined, PoolAllocator,
  TreeAllocator>::Iterator
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::operator ++(int) {
  Iterator tmp(*this);
  operator++();
  return tmp;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
std::pair<int, Type>
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::operator *() {
  return std::make_pair(index_, pool_->pool_[index_].Load());
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
bool
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::operator ==(Iterator const & rhs) {
  return (pool_ == rhs.pool_) && (index_ == rhs.index_);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
bool
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Iterator::operator !=(Iterator const & rhs) {
  return (pool_ != rhs.pool_) || (index_ != rhs.index_);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
typename LockFreeTreeValuePool<Type, Undefined, PoolAllocator,
  TreeAllocator>::Iterator
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Begin() {
  return Iterator(this);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
typename LockFreeTreeValuePool<Type, Undefined, PoolAllocator,
  TreeAllocator>::Iterator
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
End() {
  return Iterator(this, real_size_);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
GetSmallestPowerByTwoValue(int value) {
  int result = 1;
  while (result < value) result <<= 1;
  return result;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
bool LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
IsLeaf(int node) {
  if (node >= size_ - 1 && node <= 2 * size_ - 1) {
    return true;
  }
  return false;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
bool LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
IsValid(int node) {
  return (node >= 0 && node <= 2 * size_ - 1);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
GetLeftChildIndex(int node) {
  int index = 2 * node + 1;
  assert(IsValid(index));
  return index;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
GetRightChildIndex(int node) {
  int index = 2 * node + 2;
  assert(IsValid(index));
  return index;
}

template<typename T, T Undefined, class PoolAllocator, class TreeAllocator >
int LockFreeTreeValuePool<T, Undefined, PoolAllocator, TreeAllocator>::
NodeIndexToPoolIndex(int node) {
  assert(IsLeaf(node));
  return(node - (size_ - 1));
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
PoolIndexToNodeIndex(int index) {
  int node = index + (size_ - 1);
  assert(IsLeaf(node));
  return node;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
bool LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
IsRoot(int node) {
  return(0 == node);
}

template<typename T, T Undefined, class PoolAllocator, class TreeAllocator >
int LockFreeTreeValuePool<T, Undefined, PoolAllocator, TreeAllocator>::
GetParentNode(int node) {
  int parent = (node - 1) / 2;
  assert(parent >= 0 && parent < size_ - 1);
  return parent;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
allocate_rec(int node, Type& element) {
  // If we are a leaf, we try to allocate a cell using CAS.
  if (IsLeaf(node)) {
    int pool_index = NodeIndexToPoolIndex(node);

    Type expected = pool_[pool_index];
    if (expected == Undefined) {
      element = Type();
      return -1;
    }

    if (pool_[pool_index].CompareAndSwap(expected, Undefined)) {
      element = expected;
      return pool_index;
    }

    element = Type();
    return -1;
  }

  int current;
  int desired;
  // Try to decrement node value.
  // This is the point, where the algorithm becomes not wait-free. We have to
  // atomically decrement the value in the node if the result is greater than
  // or equal to zero. This cannot be done atomically.
  do {
    current = tree_[node];
    desired = current - 1;
    if (desired < 0) {
      element = Type();
      return -1;
    }
  } while (!tree_[node].CompareAndSwap(current, desired));

  int leftResult = allocate_rec(GetLeftChildIndex(node), element);
  if (leftResult != -1) {
    return leftResult;
  }
  int rightResult = (allocate_rec(GetRightChildIndex(node), element));
  // We are guaranteed to be successful either in the left or the right branch.
  // It should not happen that we cannot allocate in either branch.
  assert(rightResult != -1);

  return rightResult;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
void LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Fill(int node, int elementsToStore, int power2Value) {
  if (IsLeaf(node))
    return;

  tree_[node] = elementsToStore;

  int postPower2Value = power2Value >> 1;

  // Value fits in left cell, don't bother with right cells
  if (elementsToStore <= postPower2Value) {
    Fill(GetLeftChildIndex(node), elementsToStore, power2Value >> 1);
  } else {
    Fill(GetLeftChildIndex(node),
      postPower2Value,
      postPower2Value);

    Fill(GetRightChildIndex(node),
      elementsToStore - postPower2Value,
      postPower2Value);
  }
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Allocate(Type & element) {
  return allocate_rec(0, element);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
void LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
Free(Type element, int index) {
  assert(element != Undefined);

  // Put the element back
  pool_[index].Store(element);

  assert(index >= 0 && index < size_);
  int node = PoolIndexToNodeIndex(index);

  while (!IsRoot(node)) {
    node = GetParentNode(node);
    tree_[node].FetchAndAdd(1);
  }
}

template< typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
template< typename ForwardIterator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
LockFreeTreeValuePool(ForwardIterator first, ForwardIterator last) {
  // Number of elements to store
  real_size_ = static_cast<int>(::std::distance(first, last));

  // Let k be smallest number so that real_size <= 2^k, size = 2^k
  size_ = GetSmallestPowerByTwoValue(real_size_);

  // Size of binary tree without the leaves
  tree_size_ = size_ - 1;

  // make sure, signed values are not negative
  assert(tree_size_ >= 0);
  assert(real_size_ >= 0);

  size_t tree_size_unsigned = static_cast<size_t>(tree_size_);
  size_t real_size_unsigned = static_cast<size_t>(real_size_);

  // Pool stores elements of type T
  pool_ = pool_allocator_.allocate(real_size_unsigned);

  // invoke inplace new for each pool element
  for (size_t i = 0; i != real_size_unsigned; ++i) {
    new (&pool_[i]) embb::base::Atomic<Type>();
  }

  // Tree holds the counter of not allocated elements
  tree_ = tree_allocator_.allocate(tree_size_unsigned);

  // invoke inplace new for each tree element
  for (size_t i = 0; i != tree_size_unsigned; ++i) {
    new (&tree_[i]) embb::base::Atomic<int>();
  }

  int i = 0;

  // Store the elements from the range
  for (ForwardIterator curIter(first); curIter != last; ++curIter) {
    pool_[i++] = *curIter;
  }

  // Initialize the binary tree without leaves (counters)
  Fill(0, static_cast<int>(::std::distance(first, last)), size_);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
~LockFreeTreeValuePool() {
  size_t tree_size_unsigned = static_cast<size_t>(tree_size_);
  size_t real_size_unsigned = static_cast<size_t>(real_size_);

  // invoke destructor for each pool element
  for (size_t i = 0; i != real_size_unsigned; ++i) {
    pool_[i].~Atomic();
  }

  pool_allocator_.deallocate(pool_, real_size_unsigned);

  // invoke destructor for each tree element
  for (size_t i = 0; i != tree_size_unsigned; ++i) {
    tree_[i].~Atomic();
  }

  tree_allocator_.deallocate(tree_, tree_size_unsigned);
}

template<typename Type, Type Undefined, class PoolAllocator,
class TreeAllocator >
size_t LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
GetMinimumElementCountForGuaranteedCapacity(size_t capacity) {
  // for this value pool, this is just capacity...
  return capacity;
}

} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_TREE_VALUE_POOL_INL_H_
