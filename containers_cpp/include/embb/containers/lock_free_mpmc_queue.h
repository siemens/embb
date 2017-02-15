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

#ifndef EMBB_CONTAINERS_LOCK_FREE_MPMC_QUEUE_H_
#define EMBB_CONTAINERS_LOCK_FREE_MPMC_QUEUE_H_

#include <embb/base/atomic.h>
#include <embb/base/function.h>

#include <embb/containers/lock_free_tree_value_pool.h>
#include <embb/containers/object_pool.h>
#include <embb/containers/internal/hazard_pointer.h>

#include <limits>
#include <stdexcept>

namespace embb {
namespace containers {
namespace internal {
/**
 * Queue node
 *
 * Single linked lists, contains the element (\c element) and a pointer to the
 * next node (\c next).
 *
 * \tparam Type Element type
 */
  template< typename Type >
class LockFreeMPMCQueueNode {
 private:
  /**
   * Pointer to the next node
   */
  embb::base::Atomic< LockFreeMPMCQueueNode< Type >* > next;

  /**
   * The stored element
   */
  Type element;

 public:
  /**
   * Creates a queue node
   *
   * Explicitly allow uninitialized \c element, used for dummy node
   */
  LockFreeMPMCQueueNode();

  /**
   * Creates a queue node
   */
  LockFreeMPMCQueueNode(
    Type const& element
    /**< [IN] The element of this queue node */);

  /**
   * Returns the next pointer
   *
   * \return The next pointer
   */
  embb::base::Atomic< LockFreeMPMCQueueNode< Type >* > & GetNext();

  /**
   * Returns the element held by this node
   */
  Type GetElement();
};
} // namespace internal

/**
 * Lock-free queue for multiple producers and multiple consumers
 *
 * \concept{CPP_CONCEPTS_QUEUE}
 *
 * \ingroup CPP_CONTAINERS_QUEUES
 *
 * \see WaitFreeSPSCQueue
 *
 * \tparam Type Type of the queue elements
 * \tparam ValuePool Type of the value pool used as basis for the ObjectPool
 *         which stores the elements.
 */
template< typename Type,
  typename ValuePool = embb::containers::LockFreeTreeValuePool < bool, false >
>
class LockFreeMPMCQueue {
 private:
  /**
   * The capacity of the queue. It is guaranteed that the queue can hold at
   * least as many elements, maybe more.
   */
  size_t capacity;

  /**
   * The object pool, used for lock-free memory allocation.
   *
   * Warning: the objectPool has to be initialized before the hazardPointer
   * object, to be sure that the hazardPointer object is destructed before the
   * Pool as the hazardPointer object might return elements to the pool in its
   * destructor. So the ordering of the members objectPool and hazardPointer is
   * important here!
   */
  ObjectPool< internal::LockFreeMPMCQueueNode<Type>, ValuePool > objectPool;

  /**
   * Callback to the method that is called by hazard pointers if a pointer is
   * not hazardous anymore, i.e., can safely be reused.
   */
  embb::base::Function < void, internal::LockFreeMPMCQueueNode<Type>* >
    delete_pointer_callback;

  /**
   * Definition of the used hazard pointer type
   */
  typedef embb::containers::internal::HazardPointer
    < internal::LockFreeMPMCQueueNode<Type>* >
    MPMCQueueNodeHazardPointer_t;

  /**
   * The hazard pointer object, used for memory management.
   */
  MPMCQueueNodeHazardPointer_t hazardPointer;


  /**
   * Atomic pointer to the head node of the queue
   */
  embb::base::Atomic< internal::LockFreeMPMCQueueNode<Type>* > head;

  /**
   * Atomic pointer to the tail node of the queue
   */
  embb::base::Atomic< internal::LockFreeMPMCQueueNode<Type>* > tail;

  /**
   * The callback function, used to cleanup non-hazardous pointers.
   * \see delete_pointer_callback
   */
  void DeletePointerCallback(internal::LockFreeMPMCQueueNode<Type>* to_delete);

 public:
  /**
   * Creates a queue with the specified capacity.
   *
   * \memory
   * Let \c t be the maximum number of threads and \c x be <tt>2.5*t+1</tt>.
   * Then, <tt>x*(3*t+1)</tt> elements of size <tt>sizeof(void*)</tt>, \c x
   * elements of size <tt>sizeof(Type)</tt>, and \c capacity+1 elements of size
   * <tt>sizeof(Type)</tt> are allocated.
   *
   * \notthreadsafe
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  LockFreeMPMCQueue(
    size_t capacity
    /**< [IN] Capacity of the queue */);

  /**
   * Destroys the queue.
   *
   * \notthreadsafe
   */
  ~LockFreeMPMCQueue();

  /**
   * Returns the capacity of the queue.
   *
   * \return Number of elements the queue can hold.
   *
   * \waitfree
   */
  size_t GetCapacity();

  /**
   * Tries to enqueue an element into the queue.
   *
   * \return \c true if the element could be enqueued, \c false if the queue is
   * full.
   *
   * \lockfree
   *
   * \note It might be possible to enqueue more elements into the queue than its
   * capacity permits.
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  bool TryEnqueue(
    Type const& element
    /**< [IN] Const reference to the element that shall be enqueued */);

  /**
   * Tries to dequeue an element from the queue.
   *
   * \return \c true if an element could be dequeued, \c false if the queue is
   * empty.
   *
   * \lockfree
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  bool TryDequeue(
    Type & element
    /**< [IN, OUT] Reference to the dequeued element.
                   Unchanged, if the operation
                   was not successful. */);
};
} // namespace containers
} // namespace embb

#include <embb/containers/internal/lock_free_mpmc_queue-inl.h>

#endif  // EMBB_CONTAINERS_LOCK_FREE_MPMC_QUEUE_H_
