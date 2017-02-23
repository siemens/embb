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

#ifndef EMBB_CONTAINERS_INTERNAL_LOCK_FREE_STACK_INL_H_
#define EMBB_CONTAINERS_INTERNAL_LOCK_FREE_STACK_INL_H_

#include <embb/base/internal/config.h>

/*
 * The following algorithm uses hazard pointers and a lock-free value pool for
 * memory management. For a description of the algorithm, see
 * Maged M. Michael. "Hazard pointers: Safe memory reclamation for lock-free
 * objects". IEEE Transactions on Parallel and Distributed Systems, 15.6 (2004):
 * 491-504.
 */

namespace embb {
namespace containers {
namespace internal {
  template< typename T >
  LockFreeStackNode< T >::LockFreeStackNode(T const& element) :
    next(NULL), element(element) {
  }

  template< typename T >
  void LockFreeStackNode< T >::SetNext(LockFreeStackNode< T >* next_to_set) {
    this->next = next_to_set;
  }

  template< typename T >
  LockFreeStackNode< T >* LockFreeStackNode< T >::GetNext() {
    return next;
  }

  template< typename T >
  T LockFreeStackNode< T >::GetElement() {
    return element;
  }
} // namespace internal

template< typename Type, typename ValuePool >
void LockFreeStack< Type, ValuePool >::
DeletePointerCallback(internal::LockFreeStackNode<Type>* to_delete) {
  objectPool.Free(to_delete);
}

template< typename Type, typename ValuePool >
LockFreeStack< Type, ValuePool >::LockFreeStack(size_t capacity) :
capacity(capacity),
// Disable "this is used in base member initializer" warning.
// We explicitly want this.
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  delete_pointer_callback(*this,
    &LockFreeStack<Type>::DeletePointerCallback),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
  // Object pool, size with respect to the maximum number of retired nodes not
  // eligible for reuse:
  objectPool(
  StackNodeHazardPointer_t::ComputeMaximumRetiredObjectCount(1) +
  capacity),
  hazardPointer(delete_pointer_callback, NULL, 1) {
}

template< typename Type, typename ValuePool >
size_t LockFreeStack< Type, ValuePool >::GetCapacity() {
  return capacity;
}

template< typename Type, typename ValuePool >
LockFreeStack< Type, ValuePool >::~LockFreeStack() {
  // Nothing to do here, did not allocate anything.
}

template< typename Type, typename ValuePool >
bool LockFreeStack< Type, ValuePool >::TryPush(Type const& element) {
  internal::LockFreeStackNode<Type>* newNode =
    objectPool.Allocate(element);

  // Stack full, cannot push
  if (newNode == NULL)
    return false;

  for (;;) {
    internal::LockFreeStackNode<Type>* top_cached = top;
    newNode->SetNext(top_cached);
    if (top.CompareAndSwap(top_cached, newNode))
      return true;
  }
}

template< typename Type, typename ValuePool >
bool LockFreeStack< Type, ValuePool >::TryPop(Type & element) {
  internal::LockFreeStackNode<Type>* top_cached = top;
  for (;;) {
    top_cached = top;

    // Stack empty, cannot pop
    if (top_cached == NULL) {
      element = Type();
      return false;
    }

    // Guard top_cached
    hazardPointer.Guard(0, top_cached);

    // Check if top is still top. If this is the case, it has not been
    // retired yet (because before retiring that thing, the retiring thread
    // would first have to update the top pointer). If not, we wait for the
    // next round.
    if (top != top_cached)
      continue;

    bool compare_and_swap_suc = top.CompareAndSwap(top_cached,
      top_cached->GetNext());

    if (compare_and_swap_suc) {
      break;
    } else {
      // We continue with the next and can unguard top_cached
      hazardPointer.Guard(0, NULL);
    }
  }

  Type data = top_cached->GetElement();

  // We don't need to read from this reference anymore, unguard it
  hazardPointer.Guard(0, NULL);

  hazardPointer.EnqueueForDeletion(top_cached);

  element = data;
  return true;
}
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_STACK_INL_H_
