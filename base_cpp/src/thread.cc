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

#include <embb/base/thread.h>
#include <embb/base/exceptions.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/core_set.h>

namespace embb {
namespace base {

unsigned int Thread::GetThreadsMaxCount() {
  return embb_thread_get_max_count();
}

void Thread::SetThreadsMaxCount(unsigned int max_count) {
  embb_thread_set_max_count(max_count);
}

Thread::ID Thread::CurrentGetID() {
#if defined EMBB_PLATFORM_THREADING_WINTHREADS
  return Thread::ID(GetCurrentThreadId());
#else
  return Thread::ID(embb_thread_current().embb_internal_handle);
#endif
}

void Thread::CurrentYield() {
  embb_thread_yield();
}

void Thread::Join() {
  if (embb_thread_join(&rep_, NULL) == EMBB_ERROR) {
    EMBB_THROW(ErrorException, "Could not join thread.");
  }
}

Thread::ID Thread::GetID() {
#if defined EMBB_PLATFORM_THREADING_WINTHREADS
  return Thread::ID(GetCurrentThreadId());
#elif defined EMBB_PLATFORM_THREADING_POSIXTHREADS
  return Thread::ID(rep_.embb_internal_handle);
#endif
}

bool operator==(Thread::ID lhs, Thread::ID rhs) {
#if defined EMBB_PLATFORM_THREADING_WINTHREADS
  return lhs.id_ == rhs.id_;
#else
  return pthread_equal(lhs.id_, rhs.id_) != 0;
#endif
}

bool operator!=(Thread::ID lhs, Thread::ID rhs) {
#if defined EMBB_PLATFORM_THREADING_WINTHREADS
  return lhs.id_ != rhs.id_;
#else
  return pthread_equal(lhs.id_, rhs.id_) == 0;
#endif
}

} // namespace base
} // namespace embb
