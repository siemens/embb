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

#include <embb/base/c/mutex.h>
#include <assert.h>

#include <embb/base/c/internal/unused.h>

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

int embb_mutex_init(embb_mutex_t* mutex, int type) {
  /* Critical sections in Windows are always recursive */
  InitializeCriticalSection(mutex);
  EMBB_UNUSED(type);
  return EMBB_SUCCESS;
}

int embb_mutex_lock(embb_mutex_t* mutex) {
  EnterCriticalSection(mutex);
  return EMBB_SUCCESS;
}

int embb_mutex_try_lock(embb_mutex_t* mutex) {
  BOOL success;
  success = TryEnterCriticalSection(mutex);
  if (success == FALSE) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_mutex_unlock(embb_mutex_t* mutex) {
  LeaveCriticalSection(mutex);
  return EMBB_SUCCESS;
}

void embb_mutex_destroy(embb_mutex_t* mutex) {
  DeleteCriticalSection(mutex);
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

int embb_mutex_init(embb_mutex_t* mutex, int type) {
  if (type == EMBB_MUTEX_PLAIN) {
    if (pthread_mutex_init(mutex, NULL) != 0) return EMBB_ERROR;
  } else {
    assert(type == EMBB_MUTEX_RECURSIVE);
    pthread_mutexattr_t attributes;
    if (pthread_mutexattr_init(&attributes) != 0) return EMBB_ERROR;
    if (pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE) != 0) {
      pthread_mutexattr_destroy(&attributes);
      return EMBB_ERROR;
    }
    if (pthread_mutex_init(mutex, &attributes) != 0) {
      pthread_mutexattr_destroy(&attributes);
      return EMBB_ERROR;
    }
    if (pthread_mutexattr_destroy(&attributes) != 0) return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_mutex_lock(embb_mutex_t* mutex) {
  int result = pthread_mutex_lock(mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_mutex_try_lock(embb_mutex_t* mutex) {
  int result = pthread_mutex_trylock(mutex);
  if (result == 0) {
    return EMBB_SUCCESS;
  }
  if (result == EBUSY) {
    return EMBB_BUSY;
  }
  return EMBB_ERROR;
}

int embb_mutex_unlock(embb_mutex_t* mutex) {
  int result = pthread_mutex_unlock(mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

void embb_mutex_destroy(embb_mutex_t* mutex) {
  pthread_mutex_destroy(mutex);
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */
