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

#ifndef EMBB_BASE_INTERNAL_SHARED_MUTEX_INL_H_
#define EMBB_BASE_INTERNAL_SHARED_MUTEX_INL_H_

#include <assert.h>
#include <algorithm> // For std::swap()

namespace embb {
namespace base {

template<typename SharedMutex>
SharedLock<SharedMutex>::SharedLock()
    : shared_mutex_(NULL), locked_(false) {}

template<typename SharedMutex>
SharedLock<SharedMutex>::SharedLock(SharedMutex& shared_mutex)
    : shared_mutex_(&shared_mutex), locked_(false) {
  shared_mutex_->LockShared();
  locked_ = true;
}

template<typename SharedMutex>
SharedLock<SharedMutex>::SharedLock(SharedMutex& shared_mutex, DeferLockTag)
    : shared_mutex_(&shared_mutex), locked_(false) {}

template<typename SharedMutex>
SharedLock<SharedMutex>::SharedLock(SharedMutex& shared_mutex, TryLockTag)
    : shared_mutex_(&shared_mutex), locked_(shared_mutex_->TryLockShared()) {}

template<typename SharedMutex>
SharedLock<SharedMutex>::SharedLock(SharedMutex& shared_mutex, AdoptLockTag)
    : shared_mutex_(&shared_mutex), locked_(true) {}

template<typename SharedMutex>
SharedLock<SharedMutex>::~SharedLock() {
  if (OwnsLock()) {
    shared_mutex_->UnlockShared();
  }
}

template<typename SharedMutex>
void SharedLock<SharedMutex>::LockShared() {
  if (shared_mutex_ == NULL || locked_) {
    EMBB_THROW(ErrorException, "Mutex not set or locked");
  }
  shared_mutex_->LockShared();
  locked_ = true;
}

template<typename SharedMutex>
bool SharedLock<SharedMutex>::TryLockShared() {
  if (shared_mutex_ == NULL || locked_) {
    EMBB_THROW(ErrorException, "Mutex not set or locked");
  }
  locked_ = shared_mutex_->TryLockShared();
  return locked_;
}

template<typename SharedMutex>
void SharedLock<SharedMutex>::UnlockShared() {
  if (shared_mutex_ == NULL || !locked_) {
    EMBB_THROW(ErrorException, "Mutex not set or unlocked");
  }
  shared_mutex_->UnlockShared();
  locked_ = false;
}

template<typename SharedMutex>
void SharedLock<SharedMutex>::Swap(SharedLock& other) {
  std::swap(shared_mutex_, other.shared_mutex_);
  std::swap(locked_, other.locked_);
}

template<typename SharedMutex>
SharedMutex* SharedLock<SharedMutex>::Release() {
  SharedMutex* to_release = shared_mutex_;
  shared_mutex_ = NULL;
  locked_ = false;
  return to_release;
}

template<typename SharedMutex>
bool SharedLock<SharedMutex>::OwnsLock() const {
  assert(!(locked_ && (shared_mutex_ == NULL)));
  return locked_;
}

} // namespace base
} // namespace embb

#endif // EMBB_BASE_INTERNAL_SHARED_MUTEX_INL_H_
