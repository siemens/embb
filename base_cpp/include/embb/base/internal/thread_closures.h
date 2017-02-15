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

#ifndef EMBB_BASE_INTERNAL_THREAD_CLOSURES_H_
#define EMBB_BASE_INTERNAL_THREAD_CLOSURES_H_

#include <embb/base/memory_allocation.h>

namespace embb {
namespace base {
namespace internal {

/**
 * Thread closure for thread start function with no arguments.
 *
 * Provides a thread start function from which a priorly stored function object 
 * is called.
 */
template<typename Function>
struct ThreadClosure {
  Function function_;

  static int ThreadStart(void* arg) {
    ThreadClosure *self = static_cast<ThreadClosure*>(arg);
    self->function_();
    Allocation::Delete(self);
    return 0;
  }

  explicit ThreadClosure(const Function& function) : function_(function) {}
};

/**
 * Thread closure for thread start function with one argument.
 *
 * Provides a thread start function from which a priorly stored function object
 * is called.
 */
template<typename Function, typename Arg1>
struct ThreadClosureArg1 {
  Function function_;
  Arg1 arg1_;

  static int ThreadStart(void* arg) {
    ThreadClosureArg1 *self = static_cast<ThreadClosureArg1*>(arg);
    self->function_(self->arg1_);
    Allocation::Delete(self);
    return 0;
  }

  ThreadClosureArg1(const Function& function, const Arg1& arg1)
  : function_(function), arg1_(arg1) {}
};

/**
 * Thread closure for thread start function with two arguments.
 *
 * Provides a thread start function from which a priorly stored function object
 * is called.
 */
template<typename Function, typename Arg1, typename Arg2>
struct ThreadClosureArg2 {
  Function function_;
  Arg1 arg1_;
  Arg2 arg2_;

  static int ThreadStart(void* arg) {
    ThreadClosureArg2 *self = static_cast<ThreadClosureArg2*>(arg);
    self->function_(self->arg1_, self->arg2_);
    Allocation::Delete(self);
    return 0;
  }

  ThreadClosureArg2(
    const Function& function, const Arg1& arg1, const Arg2& arg2)
  : function_(function), arg1_(arg1), arg2_(arg2) {}
};

} // namespace internal
} // namespace base
} // namespace embb

#endif  // EMBB_BASE_INTERNAL_THREAD_CLOSURES_H_
