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

#include <embb/base/mutex.h>
#include <embb/base/c/mutex.h>

namespace embb {
namespace base {
namespace internal {

MutexBase::MutexBase(int mutex_type) : mutex_() {
  int result = embb_mutex_init(&mutex_, mutex_type);
  if (EMBB_SUCCESS != result) {
    EMBB_THROW(ErrorException, "Could not initialize mutex.");
  }
}

MutexBase::~MutexBase() {
  embb_mutex_destroy(&mutex_);
}

void MutexBase::Lock() {
  int result = embb_mutex_lock(&mutex_);
  if (EMBB_SUCCESS != result) {
    EMBB_THROW(ErrorException, "Could not lock mutex.");
  }
}

bool MutexBase::TryLock() {
  int result = embb_mutex_try_lock(&mutex_);
  return result == EMBB_SUCCESS;
}

void MutexBase::Unlock() {
  int result = embb_mutex_unlock(&mutex_);
  if (EMBB_SUCCESS != result) {
    EMBB_THROW(ErrorException, "Could not unlock mutex.");
  }
}

} // namespace internal

Mutex::Mutex() : MutexBase(EMBB_MUTEX_PLAIN) {
}

RecursiveMutex::RecursiveMutex() : MutexBase(EMBB_MUTEX_RECURSIVE) {
}

Spinlock::Spinlock() {
  embb_spin_init(&spinlock_);
}

Spinlock::~Spinlock() {
  embb_spin_destroy(&spinlock_);
}

void Spinlock::Lock() {
  int status = embb_spin_lock(&spinlock_);

  // Currently, embb_spin_lock will always return EMBB_SUCCESS. However,
  // This might change.
  if (status != EMBB_SUCCESS) {
    EMBB_THROW(ErrorException, "Error while locking spinlock");
  }
}

bool Spinlock::TryLock(unsigned int number_spins) {
  int status = embb_spin_try_lock(&spinlock_, number_spins);

  if (status == EMBB_BUSY) {
    return false;
  } else if (status != EMBB_SUCCESS) {
    EMBB_THROW(ErrorException, "Error while try-locking spinlock");
  }

  return true;
}

void Spinlock::Unlock() {
  int status = embb_spin_unlock(&spinlock_);

  if (status != EMBB_SUCCESS) {
    EMBB_THROW(ErrorException, "Error while unlocking spinlock");
  }
}
} // namespace base
} // namespace embb
