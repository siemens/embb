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

#include <embb/base/mutex.h>
#include <embb/base/c/mutex.h>

namespace embb {
namespace base {
namespace internal {
  MutexBase::MutexBase(int mutex_type) : mutex_() {
    embb_mutex_init(&mutex_, mutex_type);
  }

  MutexBase::~MutexBase() {
    embb_mutex_destroy(&mutex_);
  }

  void MutexBase::Lock() {
    embb_mutex_lock(&mutex_);
  }

  bool MutexBase::TryLock() {
    int result = embb_mutex_try_lock(&mutex_);
    return result == EMBB_SUCCESS;
  }

  void MutexBase::Unlock() {
    embb_mutex_unlock(&mutex_);
  }
} // namespace internal

Mutex::Mutex() : MutexBase(EMBB_MUTEX_PLAIN) {
}

RecursiveMutex::RecursiveMutex() : MutexBase(EMBB_MUTEX_RECURSIVE) {
}

SharedMutex::SharedMutex()
  : shared_mutex_() {
  int result = embb_shared_mutex_init(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while initializing mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}

SharedMutex::~SharedMutex() {
  embb_shared_mutex_destroy(&shared_mutex_);
}

void SharedMutex::Lock() {
  int result = embb_shared_mutex_lock(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while acquiring mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}

bool SharedMutex::TryLock() {
  int result = embb_shared_mutex_try_lock(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return true;
  case EMBB_BUSY:
    return false;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while acquiring mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}

void SharedMutex::Unlock() {
  int result = embb_shared_mutex_unlock(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while releasing mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}

void SharedMutex::LockShared() {
  int result = embb_shared_mutex_lock_shared(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while acquiring mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}

bool SharedMutex::TryLockShared() {
  int result = embb_shared_mutex_try_lock_shared(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return true;
  case EMBB_BUSY:
    return false;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while acquiring mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}

void SharedMutex::UnlockShared() {
  int result = embb_shared_mutex_unlock_shared(&shared_mutex_);

  switch (result) {
  case EMBB_SUCCESS:
    return;
  case EMBB_ERROR:
    EMBB_THROW(embb::base::ErrorException, "Error while releasing mutex.");
    break;
  default:
    EMBB_THROW(embb::base::ErrorException, "Unknown error.");
    break;
  }
}
} // namespace base
} // namespace embb
