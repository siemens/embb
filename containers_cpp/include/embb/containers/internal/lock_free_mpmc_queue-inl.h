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

#ifndef EMBB_CONTAINERS_INTERNAL_LOCK_FREE_MPMC_QUEUE_INL_H_
#define EMBB_CONTAINERS_INTERNAL_LOCK_FREE_MPMC_QUEUE_INL_H_

/*
 * The following algorithm uses hazard pointers and a lock-free value pool for
 * memory management. For a description of the algorithm, see
 * Maged M. Michael and Michael L. Scott. "Simple, fast, and practical
 * non-blocking and blocking concurrent queue algorithms". Proceedings of the
 * fifteenth annual ACM symposium on principles of distributed computing.
 * ACM, 1996. (Figure 1, Page 4)
 */

namespace embb {
namespace containers {
namespace internal {
template< typename T >
LockFreeMPMCQueueNode<T>::LockFreeMPMCQueueNode() :
next(NULL) {
}

template< typename T >
LockFreeMPMCQueueNode<T>::LockFreeMPMCQueueNode(T const& element) :
  next(NULL),
  element(element) {
}

template< typename T >
embb::base::Atomic< LockFreeMPMCQueueNode< T >* > &
  LockFreeMPMCQueueNode<T>::GetNext() {
  return next;
}

template< typename T >
T LockFreeMPMCQueueNode<T>::GetElement() {
  return element;
}
} // namespace internal

template< typename T, typename ValuePool >
void LockFreeMPMCQueue<T, ValuePool>::
DeletePointerCallback(internal::LockFreeMPMCQueueNode<T>* to_delete) {
  objectPool.Free(to_delete);
}

template< typename T, typename ValuePool >
LockFreeMPMCQueue<T, ValuePool>::~LockFreeMPMCQueue() {
  // Nothing to do here, did not allocate anything.
}

template< typename T, typename ValuePool >
LockFreeMPMCQueue<T, ValuePool>::LockFreeMPMCQueue(size_t capacity) :
capacity(capacity),
// Disable "this is used in base member initializer" warning.
// We explicitly want this.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  delete_pointer_callback(*this, &LockFreeMPMCQueue<T>::DeletePointerCallback),
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  hazardPointer(delete_pointer_callback, NULL, 2),
  // Object pool, size with respect to the maximum number of retired nodes not
  // eligible for reuse. +1 for dummy node.
  objectPool(
  hazardPointer.GetRetiredListMaxSize()*
  embb::base::Thread::GetThreadsMaxCount() +
  capacity + 1) {
  // Allocate dummy node to reduce the number of special cases to consider.
  internal::LockFreeMPMCQueueNode<T>* dummyNode = objectPool.Allocate();
  // Initially, head and tail point to the dummy node.
  head = dummyNode;
  tail = dummyNode;
}

template< typename T, typename ValuePool >
size_t LockFreeMPMCQueue<T, ValuePool>::GetCapacity() {
  return capacity;
}

template< typename T, typename ValuePool >
bool LockFreeMPMCQueue<T, ValuePool>::TryEnqueue(T const& element) {
  // Get node from the pool containing element to enqueue.
  internal::LockFreeMPMCQueueNode<T>* node = objectPool.Allocate(element);

  // Queue full, cannot enqueue
  if (node == NULL)
    return false;
  internal::LockFreeMPMCQueueNode<T>* my_tail;
  for (;;) {
    my_tail = tail;

    hazardPointer.GuardPointer(0, my_tail);

    // Check if pointer is still valid after guarding.
    if (my_tail != tail) {
      continue; // Hazard pointer outdated, retry
    }

    internal::LockFreeMPMCQueueNode<T>* my_tail_next = my_tail->GetNext();

    if (my_tail == tail) {
      // If the next pointer of the tail node is null, the tail pointer
      // points to the last object. We try to set the next pointer of the
      // tail node to our new node.
      if (my_tail_next == NULL) {
        internal::LockFreeMPMCQueueNode<T>* expected = NULL;
        // This fails if the next pointer of the "cached" tail is not null
        // anymore, i.e., another thread added a node before we could complete.
        if (my_tail->GetNext().CompareAndSwap(expected, node))
          break; // We successfully added our node.
        //Still missing: increase tail pointer.
        // The tail pointer points not to the last object, first increase
      } else {
        // Try to increase the tail pointer.
        tail.CompareAndSwap(my_tail, my_tail_next);
      }
    }
  }
  // We added our node. Try to update tail pointer. Need not succeed, if we
  // fail, another thread will help us.
  tail.CompareAndSwap(my_tail, node);

  return true;
}

template< typename T, typename ValuePool >
bool LockFreeMPMCQueue<T, ValuePool>::TryDequeue(T & element) {
  internal::LockFreeMPMCQueueNode<T>* my_head;
  internal::LockFreeMPMCQueueNode<T>* my_tail;
  internal::LockFreeMPMCQueueNode<T>* my_next;
  internal::LockFreeMPMCQueueNode<T>* expected;
  T data;
  for (;;) {
    my_head = head;
    hazardPointer.GuardPointer(0, my_head);
    if (my_head != head) continue;

    my_tail = tail;
    my_next = my_head->GetNext();
    hazardPointer.GuardPointer(1, my_next);
    if (head != my_head) continue;

    if (my_next == NULL)
      return false;

    if (my_head == my_tail) {
      expected = my_tail;
      tail.CompareAndSwap(expected, my_next);
      continue;
    }

    data = my_next->GetElement();

    expected = my_head;
    if (head.CompareAndSwap(expected, my_next))
      break;
  }

  hazardPointer.EnqueuePointerForDeletion(my_head);
  element = data;
  return true;
}
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_MPMC_QUEUE_INL_H_
