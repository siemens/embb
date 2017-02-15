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

#ifndef EMBB_BASE_INTERNAL_THREAD_INL_H_
#define EMBB_BASE_INTERNAL_THREAD_INL_H_

#include <embb/base/exceptions.h>
#include <embb/base/internal/thread_closures.h>
#include <embb/base/memory_allocation.h>
#include <iostream>

namespace embb {
namespace base {

template<typename Function>
Thread::Thread(Function function) : rep_() {
  internal::ThreadClosure<Function>* closure =
    Allocation::New<internal::ThreadClosure<Function> >(function);
  int result = embb_thread_create(
    &rep_,
    NULL,
    internal::ThreadClosure<Function>::ThreadStart,
    static_cast<void*>(closure));
  CheckThreadCreationErrors(result, closure);
}

template<typename Function>
Thread::Thread(CoreSet& core_set, Function function) : rep_() {
  typedef internal::ThreadClosure<Function> Closure;
  Closure* closure = Allocation::New<Closure>(function);
  int result = embb_thread_create(
    &rep_,
    &core_set.rep_,
    internal::ThreadClosure<Function>::ThreadStart,
    static_cast<void*>(closure));
  CheckThreadCreationErrors(result, closure);
}

template<typename Function>
Thread::Thread(
  CoreSet& core_set,
  embb_thread_priority_t priority,
  Function function) : rep_() {
  typedef internal::ThreadClosure<Function> Closure;
  Closure* closure = Allocation::New<Closure>(function);
  int result = embb_thread_create_with_priority(
    &rep_,
    &core_set.rep_,
    priority,
    internal::ThreadClosure<Function>::ThreadStart,
    static_cast<void*>(closure));
  CheckThreadCreationErrors(result, closure);
}

template<typename Function, typename Arg1>
Thread::Thread(Function function, Arg1 arg1) : rep_() {
  typedef internal::ThreadClosureArg1<Function, Arg1> Closure;
  Closure* closure = Allocation::New<Closure>(function, arg1);
  int result =  embb_thread_create(
    &rep_,
    NULL,
    internal::ThreadClosureArg1<Function, Arg1>::ThreadStart,
    static_cast<void*>(closure));
  CheckThreadCreationErrors(result, closure);
}

template<typename Function, typename Arg1, typename Arg2>
Thread::Thread(Function function, Arg1 arg1, Arg2 arg2) : rep_() {
  typedef internal::ThreadClosureArg2<Function, Arg1, Arg2> Closure;
  Closure* closure = Allocation::New<Closure>(function, arg1, arg2);
  int result = embb_thread_create(
    &rep_,
    NULL,
    internal::ThreadClosureArg2<Function, Arg1, Arg2>::ThreadStart,
    static_cast<void*>(closure));
  CheckThreadCreationErrors(result, closure);
}

template<typename ThreadClosure>
void Thread::CheckThreadCreationErrors(int result, ThreadClosure* closure) {
  if (result != EMBB_SUCCESS) {
    if (closure != NULL) {
      Allocation::Delete(closure);
    }
    const char* message = "Could not create thread.";
    if (result == EMBB_NOMEM) {
      EMBB_THROW(NoMemoryException, message);
    }
    EMBB_THROW(ErrorException, message);
  }
}

template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
  operator<<(std::basic_ostream<CharT, Traits>& os, Thread::ID id) {
  return os << id.id_;
}

} // namespace base
} // namespace embb

#endif  // EMBB_BASE_INTERNAL_THREAD_INL_H_
