/*
  * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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
   
#ifndef EMBB_CONTAINERS_BLOCKING_PRIORITY_QUEUE_H_
#define EMBB_CONTAINERS_BLOCKING_PRIORITY_QUEUE_H_
   
#include <queue>
   
#include <embb/containers/blocking_container.h>
   
namespace embb {
namespace containers {
     
/**
* Wrapper for the standard library priority_queue.
*
* \tparam T Element type
*/
template< typename Type, 
          class Container = std::vector<Type>, 
          class Compare = std::less<typename Container::value_type>>
class BlockingPriorityQueue : public BlockingContainer<Type> {
 
 private:
  /**
   * Internal priority_queue from the standard library.
   */
  std::priority_queue<Type, Container,Compare> internalQueue;
 public:
  /**
   * Enqueues an element in the internal priority_queue.
   */
  void Enqueue(
    const Type& element
        /**< [IN] Constant reference to element to enqueue*/);
           
  void Dequeue(
    Type& element
        /**< [IN] Reference to dequeued element*/);
           
  protected:
  /*
   * Wrapper for the push method in the standard priority_queue.
   */
  virtual void SpecializedPush(
    const Type& element
        /**< [IN] Constant reference to element to push.*/);
               
  /**
   * Wrapper for the pop method in the standard priority_queue.
   */
  virtual void SpecializedPop(
    Type& element
        /**< [IN,OUT] Reference to element to the popped element*/);
               
  /**
   * Wrapper for the empty method in the standard priority_queue.
   */
  virtual bool IsEmpty();
                              
};
      
}  // namespace containers   
}  // namespace embb

#include <embb/containers/internal/blocking_priority_queue-inl.h>

#endif // EMBB_CONTAINERS_BLOCKING_PRIORITY_QUEUE