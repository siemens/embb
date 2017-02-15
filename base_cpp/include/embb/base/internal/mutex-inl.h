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

#ifndef EMBB_BASE_INTERNAL_MUTEX_INL_H_
#define EMBB_BASE_INTERNAL_MUTEX_INL_H_

#include <cassert>
#include <utility>   // swap C++ 11
#include <algorithm> // swap C++ 98

namespace embb {
namespace base {

template<typename Mutex>
UniqueLock<Mutex>::UniqueLock() : mutex_(NULL), locked_(false) {
}

template<typename Mutex>
UniqueLock<Mutex>::UniqueLock(Mutex& mutex)
    : mutex_(&mutex), locked_(false) {
  mutex_->Lock();
  locked_ = true;
}

template<typename Mutex>
UniqueLock<Mutex>::UniqueLock(Mutex& mutex, DeferLockTag)
    : mutex_(&mutex), locked_(false) {
}

template<typename Mutex>
UniqueLock<Mutex>::UniqueLock(Mutex& mutex, embb::base::TryLockTag)
    : mutex_(&mutex), locked_(false) {
  locked_ = mutex_->TryLock();
}

template<typename Mutex>
UniqueLock<Mutex>::UniqueLock(Mutex& mutex, AdoptLockTag)
    : mutex_(&mutex), locked_(true) {
}

template<typename Mutex>
UniqueLock<Mutex>::~UniqueLock() {
  if (OwnsLock()) {
    mutex_->Unlock();
  }
}

template<typename Mutex>
void UniqueLock<Mutex>::Lock() {
  if ((mutex_ == NULL) || locked_) {
    EMBB_THROW(ErrorException, "Mutex not set or locked");
  }
  mutex_->Lock();
  locked_ = true;
}

template<typename Mutex>
bool UniqueLock<Mutex>::TryLock() {
  if ((mutex_ == NULL) || locked_) {
    EMBB_THROW(ErrorException, "Mutex not set or locked");
  }
  locked_ = mutex_->TryLock();
  return locked_;
}

template<typename Mutex>
void UniqueLock<Mutex>::Unlock() {
  if ((mutex_ == NULL) || (!locked_)) {
    EMBB_THROW(ErrorException, "Mutex not set or unlocked");
  }
  locked_ = false;
  mutex_->Unlock();
}

template<typename Mutex>
void UniqueLock<Mutex>::Swap(UniqueLock<Mutex>& other) {
  std::swap(mutex_, other.mutex_);
  std::swap(locked_, other.locked_);
}

template<typename Mutex>
Mutex* UniqueLock<Mutex>::Release() {
  Mutex* toRelease = mutex_;
  mutex_ = NULL;
  locked_ = false;
  return toRelease;
}

template<typename Mutex>
bool UniqueLock<Mutex>::OwnsLock() const {
  assert(!(locked_ && (mutex_ == NULL)));
  return locked_;
}



} // namespace base
} // namespace embb

#endif  // EMBB_BASE_INTERNAL_MUTEX_INL_H_
