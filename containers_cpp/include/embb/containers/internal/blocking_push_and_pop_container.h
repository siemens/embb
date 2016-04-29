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

#ifndef EMBB_CONTAINERS_BLOCKING_PUSH_AND_POP_CONTAINER_H_
#define EMBB_CONTAINERS_BLOCKING_PUSH_AND_POP_CONTAINER_H_


#include <embb/base/base.h>

namespace embb {
namespace containers {

// Abstract class, provides a synchronization mechanism for 
// data structures that support push/pop-like methods (e.g. stacks, queues)
template < typename Type >
class BlockingPushAndPopContainer {

  typedef embb::base::Mutex Mutex;

 protected:

  Mutex mutex;

  embb::base::ConditionVariable condition;

  virtual void SpecializedPush(const Type& element) = 0;

  virtual void SpecializedPop(Type& element) = 0;

  virtual bool IsEmpty() = 0;

  void PushAndWakeUp(const Type&  element) {
  embb::base::LockGuard<Mutex> lock(mutex);

  SpecializedPush(element);

  condition.NotifyOne();
  }

  void BlockingPop( Type & element) {
  embb::base::UniqueLock<Mutex> lock(mutex);

  while (IsEmpty()) {
    condition.Wait(lock);
  }

  SpecializedPop(element);
  }

};

}
}

#endif  // EMBB_CONTAINERS_BLOCKING_PUSH_AND_POP_CONTAINER_