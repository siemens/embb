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

#include <embb/base/c/shared_mutex.h>
#include <assert.h>

#include <embb/base/c/internal/unused.h>

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

int embb_shared_mutex_init(embb_shared_mutex_t* shared_mutex) {
  InitializeSRWLock(shared_mutex);
  return EMBB_SUCCESS;
}

int embb_shared_mutex_lock(embb_shared_mutex_t* shared_mutex) {
  AcquireSRWLockExclusive(shared_mutex);
  return EMBB_SUCCESS;
}

int embb_shared_mutex_try_lock(embb_shared_mutex_t* shared_mutex) {
  BOOLEAN success;
  success = TryAcquireSRWLockExclusive(shared_mutex);
  if (success == FALSE) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_shared_mutex_unlock(embb_shared_mutex_t* shared_mutex) {
  ReleaseSRWLockExclusive(shared_mutex);
  return EMBB_SUCCESS;
}

int embb_shared_mutex_lock_shared(embb_shared_mutex_t* shared_mutex) {
  AcquireSRWLockShared(shared_mutex);
  return EMBB_SUCCESS;
}

int embb_shared_mutex_try_lock_shared(embb_shared_mutex_t* shared_mutex) {
  BOOLEAN success;
  success = TryAcquireSRWLockShared(shared_mutex);
  if (success == FALSE) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_shared_mutex_unlock_shared(embb_shared_mutex_t* shared_mutex) {
  ReleaseSRWLockShared(shared_mutex);
  return EMBB_SUCCESS;
}

void embb_shared_mutex_destroy(embb_shared_mutex_t* shared_mutex) {
  // Quoting MSDN: "SRW locks do not need to be explicitly destroyed".
  EMBB_UNUSED(shared_mutex);
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

int embb_shared_mutex_init(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_init(shared_mutex, NULL);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_shared_mutex_lock(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_wrlock(shared_mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_shared_mutex_try_lock(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_trywrlock(shared_mutex);
  if (result == 0) {
    return EMBB_SUCCESS;
  }
  if (result == EBUSY) {
    return EMBB_BUSY;
  }
  return EMBB_ERROR;
}

int embb_shared_mutex_unlock(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_unlock(shared_mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_shared_mutex_lock_shared(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_rdlock(shared_mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_shared_mutex_try_lock_shared(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_tryrdlock(shared_mutex);
  if (result == 0) {
    return EMBB_SUCCESS;
  }
  if (result == EBUSY) {
    return EMBB_BUSY;
  }
  return EMBB_ERROR;
}

int embb_shared_mutex_unlock_shared(embb_shared_mutex_t* shared_mutex) {
  int result = pthread_rwlock_unlock(shared_mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

void embb_shared_mutex_destroy(embb_shared_mutex_t* shared_mutex) {
  pthread_rwlock_destroy(shared_mutex);
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */
