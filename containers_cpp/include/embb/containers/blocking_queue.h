/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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

#ifndef EMBB_CONTAINERS_BLOCKING_QUEUE_H_
#define EMBB_CONTAINERS_BLOCKING_QUEUE_H_

#include <queue>
#include <embb/containers/internal/blocking_push_and_pop_container.h>

namespace embb {
namespace containers {

/**
 * Blocking queue.
 *
 * \concept{CPP_CONCEPTS_QUEUE}
 *
 * \ingroup CPP_CONTAINERS_QUEUES
 *
 * \see WaitFreeSPSCQueue, LockFreeMPMCQueue, BlockingPriorityQueue
 *
 * \tparam Type Element type
 */
template< typename Type>
class BlockingQueue : public BlockingPushAndPopContainer<Type> {
 private:
  /**
   * Internal queue from the standard library.
   */
  std::queue<Type> internalQueue;

  /**
   * Wrapper for push_back method in the standard library queue.
   * Implements the corresponding pure virtual method
   * in the super class.
   */
  void SpecializedPush(const Type& element);
  
  /**
   * Wrapper for pop_front method in the standard library queue.
   * Implements the corresponding pure virtual method
   * in the super class.
   */
  void SpecializedPop(Type& element);

  /** 
   * Wrapper for the empty method in the standard queue.
   * Implements the corresponding pure virtual method
   * in the super class.
   */
  bool IsEmpty();

 public:
  /**
   * Enqueues an element in the priority queue.
   */
  void Enqueue(
    const Type& element
    /**< [IN] Constant reference to element to enqueue*/);

  /**
   * Dequeues the next element from the priority queue.
   * What element will be dequeued is determined by the Compare
   * template parameter. By default, the next returned element is
   * the one with the largest key.
   */
  void Dequeue(
    Type& element
    /**< [IN, OUT] Reference to dequeued element*/);

};
 
}  // namespace containers
}  // namespace embb

#include <embb/containers/internal/blocking_queue-inl.h>

#endif  // EMBB_CONTAINERS_BLOCKING_QUEUE_H_