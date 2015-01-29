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

#ifndef EMBB_CONTAINERS_INTERNAL_LOCK_FREE_TREE_VALUE_POOL_INL_H_
#define EMBB_CONTAINERS_INTERNAL_LOCK_FREE_TREE_VALUE_POOL_INL_H_

namespace embb {
namespace containers {
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
  if (node >= size - 1 && node <= 2 * size - 1) {
    return true;
  }
  return false;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
bool LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
IsValid(int node) {
  return (node >= 0 && node <= 2 * size - 1);
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
  return(node - (size - 1));
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
PoolIndexToNodeIndex(int index) {
  int node = index + (size - 1);
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
  assert(parent >= 0 && parent < size - 1);
  return parent;
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
int LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
allocate_rec(int node, Type& element) {
  // If we are a leaf, we try to allocate a cell using CAS.
  if (IsLeaf(node)) {
    int pool_index = NodeIndexToPoolIndex(node);

    Type expected = pool[pool_index];
    if (expected == Undefined)
      return -1;

    if (pool[pool_index].CompareAndSwap(expected, Undefined)) {
      element = expected;
      return pool_index;
    }

    return -1;
  }

  int current;
  int desired;
  // Try to decrement node value.
  // This is the point, where the algorithm becomes not wait-free. We have to
  // atomically decrement the value in the node if the result is greater than
  // or equal to zero. This cannot be done atomically.
  do {
    current = tree[node];
    desired = current - 1;
    if (desired < 0)
      return -1;
  } while (!tree[node].CompareAndSwap(current, desired));

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

  tree[node] = elementsToStore;

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
  pool[index].Store(element);

  assert(index >= 0 && index < size);
  int node = PoolIndexToNodeIndex(index);

  while (!IsRoot(node)) {
    node = GetParentNode(node);
    tree[node].FetchAndAdd(1);
  }
}

template< typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
template< typename ForwardIterator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
LockFreeTreeValuePool(ForwardIterator first, ForwardIterator last) {
  // Number of elements to store
  real_size = static_cast<int>(::std::distance(first, last));

  // Let k be smallest number so that real_size <= 2^k, size = 2^k
  size = GetSmallestPowerByTwoValue(real_size);

  // Size of binary tree without the leaves
  tree_size = size - 1;

  // Pool stores elements of type T
  pool = poolAllocator.allocate(static_cast<size_t>(real_size));

  // Tree holds the counter of not allocated elements
  tree = treeAllocator.allocate(static_cast<size_t>(tree_size));

  int i = 0;

  // Store the elements from the range
  for (ForwardIterator curIter(first); curIter != last; ++curIter) {
    pool[i++] = *curIter;
  }

  // Initialize the binary tree without leaves (counters)
  Fill(0, static_cast<int>(::std::distance(first, last)), size);
}

template<typename Type, Type Undefined, class PoolAllocator,
  class TreeAllocator >
LockFreeTreeValuePool<Type, Undefined, PoolAllocator, TreeAllocator>::
~LockFreeTreeValuePool() {
  poolAllocator.deallocate(pool, static_cast<size_t>(real_size));
  treeAllocator.deallocate(tree, static_cast<size_t>(tree_size));
}

} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_TREE_VALUE_POOL_INL_H_
