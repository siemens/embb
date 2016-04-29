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

#ifndef EMBB_CONTAINERS_BLOCKING_STACK_H_
#define EMBB_CONTAINERS_BLOCKING_STACK_H_

#include <stack>

#include <embb/containers/internal/blocking_push_and_pop_container.h>

namespace embb {
namespace containers {

/**
* Wrapper for the standard library stack.
*
* \tparam T Element type
*/
template< typename Type>
class BlockingStack : public BlockingPushAndPopContainer<Type> {
 private:
  /**
  * Internal stack from the standard library.
  */
  std::stack<Type> internalStack;
 public:
  /**
  * Enstacks an element in the internal stack.
  */
  void Push(
    const Type& element
    /**< [IN] Constant reference to element to push*/);

  void Pop(
    Type& element
    /**< [IN] Reference to popped element*/);

 protected:
   /*
   * Wrapper for the push method in the standard stack.
   */
  virtual void SpecializedPush(
    const Type& element
    /**< [IN] Constant reference to element to push.*/);

  /**
  * Wrapper for the pop method in the standard stack.
  */
  virtual void SpecializedPop(
    Type& element
    /**< [IN,OUT] Reference to element to the popped element*/);

  /**
  * Wrapper for the empty method in the standard stack.
  */
  virtual bool IsEmpty();

};
 
}  // namespace containers
}  // namespace embb

#include <embb/containers/internal/blocking_stack-inl.h>

#endif  // EMBB_CONTAINERS_BLOCKING_STACK_H_