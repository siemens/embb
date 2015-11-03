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

#include <embb/base/c/rwlock.h>
#include <assert.h>

#include <embb/base/c/internal/unused.h>

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

int embb_rwlock_init(embb_rwlock_t* rwlock) {
  InitializeSRWLock(rwlock);
  return EMBB_SUCCESS;
}

int embb_rwlock_lock_read(embb_rwlock_t* rwlock) {
  AcquireSRWLockShared(rwlock);
  return EMBB_SUCCESS;
}

int embb_rwlock_lock_write(embb_rwlock_t* rwlock) {
  AcquireSRWLockExclusive(rwlock);
  return EMBB_SUCCESS;
}

int embb_rwlock_try_lock_read(embb_rwlock_t* rwlock) {
  BOOLEAN success;
  success = TryAcquireSRWLockShared(rwlock);
  if (success == FALSE) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_rwlock_try_lock_write(embb_rwlock_t* rwlock) {
  BOOLEAN success;
  success = TryAcquireSRWLockExclusive(rwlock);
  if (success == FALSE) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_rwlock_unlock_read(embb_rwlock_t* rwlock) {
  ReleaseSRWLockShared(rwlock);
  return EMBB_SUCCESS;
}

int embb_rwlock_unlock_write(embb_rwlock_t* rwlock) {
  ReleaseSRWLockExclusive(rwlock);
  return EMBB_SUCCESS;
}

void embb_rwlock_destroy(embb_rwlock_t* rwlock) {
  // Quoting MSDN: "SRW locks do not need to be explicitly destroyed".
  EMBB_UNUSED(rwlock);
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

int embb_rwlock_init(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_init(rwlock, NULL);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_rwlock_lock_read(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_rdlock(rwlock);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_rwlock_lock_write(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_wrlock(rwlock);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_rwlock_try_lock_read(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_tryrdlock(rwlock);
  if (result == 0) {
    return EMBB_SUCCESS;
  }
  if (result == EBUSY) {
    return EMBB_BUSY;
  }
  return EMBB_ERROR;
}

int embb_rwlock_try_lock_write(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_trywrlock(rwlock);
  if (result == 0) {
    return EMBB_SUCCESS;
  }
  if (result == EBUSY) {
    return EMBB_BUSY;
  }
  return EMBB_ERROR;
}

int embb_rwlock_unlock_read(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_unlock(rwlock);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_rwlock_unlock_write(embb_rwlock_t* rwlock) {
  int result = pthread_rwlock_unlock(rwlock);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

void embb_rwlock_destroy(embb_rwlock_t* rwlock) {
  pthread_rwlock_destroy(rwlock);
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */
