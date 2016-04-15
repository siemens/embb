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

#ifndef EMBB_CONTAINERS_BLOCKING_CONTAINER_H_
#define EMBB_CONTAINERS_BLOCKING_CONTAINER_H_


#include <embb/base/base.h>


namespace embb {
namespace containers {

using namespace base;

/*
* Abstract class to use for the implementation of blocking datastructure.
* Child classes need to be wrappers for another container.
*
* \tparam T Element type of the container's data.
*/
template < typename Type >
class BlockingContainer {
 protected:
  /*
  * Mutex for thread synchronization.
  */
  Mutex mutex;

  /**
  * Condition variable for notifying threads that are waiting
  * for popping elements that new elements are available.
  */
  ConditionVariable condition;

  /**
  * Pure virtual method to be implemented in derived classes.
  * The method must wrap the pushing method of the 
  * underlying container.
  */
  virtual void SpecializedPush(
    const Type& element
    /**< [IN] Constant reference
    to element that shall be pushed. */) = 0;

  /**
  * Pure virtual method to be implemented in derived classes.
  * The method must wrap the popping method of the 
  * underlying container.
  */
  virtual void SpecializedPop(
    Type& element
    /**< [IN, OUT] Reference to popped element*/) = 0;

  /**
  * Pure virtual method to check if the underlying container
  * is empty.
  *
  * \return \c true if container is empty, \c false otherwise.
  */
  virtual bool IsEmpty() = 0;

  /**
  * Pushes an element and notifies threads that are waiting to
  * pop that new elements are available
  *
  *\note The calling threads acquire the mutex at the beginning
  * and release it at the end of the method to avoid concurrent
  * access to the structure.
  */
  void PushAndWakeUp(
    const Type&  element
    /**< [IN] Constant reference
    to element that shall be pushed */);

  /**
  * Pops an element from the container. If no elements are
  * available, the calling thread wait until it is notified
  * by the condition variable in PushAndWakeUp.
  *
  *\note The calling threads acquire the mutex at the beginning
  * and release it at the end of the method to avoid concurrent
  * access to the structure.
  */
  void BlockingPop(
    Type & element
    /**< [IN,OUT] Reference to popped element*/);

};

}
}

#include <embb/containers/internal/blocking_container-inl.h>

#endif  // EMBB_CONTAINERS_BLOCKING_CONTAINER_