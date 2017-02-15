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

#ifndef EMBB_CONTAINERS_WAIT_FREE_SPSC_QUEUE_H_
#define EMBB_CONTAINERS_WAIT_FREE_SPSC_QUEUE_H_

#include <embb/base/atomic.h>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
/**
 * \defgroup CPP_CONCEPTS_QUEUE Queue Concept
 * Concept for thread-safe queues
 *
 * \ingroup CPP_CONCEPT
 * \{
 * \par Description
 * A queue is an abstract data type holding a collection of elements of some
 * predetermined type. A queue provides two operations: \c TryEnqueue and
 * \c TryDequeue. \c TryEnqueue tries to add an element to the collection, and
 * \c TryDequeue tries to remove an element from the collection. A queue has
 * per-thread FIFO (First-In, First-out) semantics, i.e., if one thread enqueues
 * two elements and another thread dequeues these elements, then they appear in
 * the same order. The capacity \c cap of a queue defines the number of elements
 * it can store (depending on the implementation, a queue might store more than
 * \c cap elements, since for thread-safe memory management, more memory than
 * necessary for holding \c cap elements has to be provided).
 *
 * \par Requirements
 * - Let \c Queue be the queue class
 * - Let \c Type be the element type of the queue
 * - Let \c capacity be a value of type \c size_t
 * - Let \c element be a reference to an element of type \c Type
 *
 * \par Valid Expressions
 * <table>
 *   <tr>
 *     <th>Expression</th>
 *     <th>Return type</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} Queue<Type>(capacity) \endcode</td>
 *     <td>Nothing</td>
 *     <td>
 *      Constructs a queue with minimal capacity \c capacity that holds elements of
 *      type \c T.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} TryEnqueue(element) \endcode</td>
 *     <td>\code{.cpp} bool \endcode</td>
 *     <td>
 *      Tries to enqueue \c element into the queue. Returns \c false if the
 *      queue is full, otherwise \c true.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} TryDequeue(element) \endcode</td>
 *     <td>\code{.cpp} bool \endcode</td>
 *     <td>
 *      Tries to dequeue an element from the queue. Returns \c false if the
 *      queue is empty, otherwise \c true. In the latter case, the dequeued
 *      element is stored in \c element which must be passed by reference.
 *     </td>
 *   </tr>
 * </table>
 *
 * \}
 */

/**
 * \defgroup CPP_CONTAINERS_QUEUES Queues
 * Concurrent queues
 *
 * \see CPP_CONCEPTS_QUEUE
 *
 * \ingroup CPP_CONTAINERS
 *
 */
namespace embb {
namespace containers {
/**
 * Wait-free queue for a single producer and a single consumer.
 *
 * \concept{CPP_CONCEPTS_QUEUE}
 *
 * \ingroup CPP_CONTAINERS_QUEUES
 *
 * \see LockFreeMPMCQueue
 *
 * \tparam Type Type of the queue elements
 * \tparam Allocator Allocator type for allocating queue elements.
 */
template<typename Type, class Allocator = embb::base::Allocator< Type > >
class WaitFreeSPSCQueue {
 private:
  /**
   * Allocator for allocating queue elements
   */
  Allocator allocator;

  /**
   * Capacity of the queue
   */
  size_t capacity;

  /**
   * Array holding the queue elements
   */
  Type* queue_array;

  /**
   * Index of the head in the \c queue_array
   */
  embb::base::Atomic<size_t> head_index;

  /**
   * Index of the tail in the \c queue_array
   */
  embb::base::Atomic<size_t> tail_index;

  /**
   * Align capacity to the next smallest power of two
   */
  static size_t AlignCapacityToPowerOfTwo(size_t capacity);

 public:
  /**
   * Creates a queue with at least the specified capacity.
   *
   * \memory Allocates \c 2^k elements of type \c Type, where \c k is the
   * smallest number such that <tt>capacity <= 2^k</tt> holds.
   *
   * \notthreadsafe
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  WaitFreeSPSCQueue(
    size_t capacity
    /**< [IN] Capacity of the queue */
  );

  /**
   * Destroys the queue.
   *
   * \notthreadsafe
   */
  ~WaitFreeSPSCQueue();

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
   * \waitfree
   *
   * \note Concurrently enqueueing elements by multiple producers leads to
   * undefined behavior.
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  bool TryEnqueue(
    Type const & element
    /**< [IN] Const reference to the element that shall be enqueued */
  );

  /**
   * Tries to dequeue an element from the queue.
   *
   * \return \c true if an element could be dequeued, \c false if the queue is
   * empty.
   *
   * \waitfree
   *
   * \note Concurrently dequeueing elements by multiple consumers leads to
   * undefined behavior.
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  bool TryDequeue(
    Type & element
    /**< [IN,OUT] Reference to the dequeued element. Unchanged, if the
                  operation was not successful. */
  );
};
} // namespace containers
} // namespace embb

#include<embb/containers/internal/wait_free_spsc_queue-inl.h>

#endif  // EMBB_CONTAINERS_WAIT_FREE_SPSC_QUEUE_H_
