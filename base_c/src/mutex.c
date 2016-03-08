/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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
#include <embb/base/c/thread.h>
#include <assert.h>

#include <embb/base/c/internal/unused.h>

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

int embb_mutex_init(embb_mutex_t* mutex, int type) {
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
  /* Critical sections in Windows are always recursive */
  InitializeCriticalSection(mutex);
  EMBB_UNUSED(type);
  return EMBB_SUCCESS;
}

int embb_mutex_lock(embb_mutex_t* mutex) {
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
  EnterCriticalSection(mutex);
  return EMBB_SUCCESS;
}

int embb_mutex_try_lock(embb_mutex_t* mutex) {
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
  BOOL success;
  success = TryEnterCriticalSection(mutex);
  if (success == FALSE) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_mutex_unlock(embb_mutex_t* mutex) {
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
  LeaveCriticalSection(mutex);
  return EMBB_SUCCESS;
}

void embb_mutex_destroy(embb_mutex_t* mutex) {
  assert(NULL != mutex);
  DeleteCriticalSection(mutex);
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

int embb_mutex_init(embb_mutex_t* mutex, int type) {
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
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
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
  int result = pthread_mutex_lock(mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_mutex_try_lock(embb_mutex_t* mutex) {
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
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
  if (NULL == mutex) {
    return EMBB_ERROR;
  }
  int result = pthread_mutex_unlock(mutex);
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

void embb_mutex_destroy(embb_mutex_t* mutex) {
  assert(NULL != mutex);
  pthread_mutex_destroy(mutex);
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */

int embb_spin_init(embb_spinlock_t* spinlock) {
  if (NULL == spinlock) {
    return EMBB_ERROR;
  }
  // For now, store the initial value. In the future will use atomic init
  // function (as soon as available).
  embb_atomic_store_int(&spinlock->atomic_spin_variable_, 0);
  return EMBB_SUCCESS;
}

int embb_spin_lock(embb_spinlock_t* spinlock) {
  if (NULL == spinlock) {
    return EMBB_ERROR;
  }
  int expected = 0;
  int spins = 1;

  // try to swap the
  while (0 == embb_atomic_compare_and_swap_int(
    &spinlock->atomic_spin_variable_, &expected, 1)) {
    if (0 == (spins & 1023)) {
      embb_thread_yield();
    }
    spins++;
    // reset expected, as CAS might change it...
    expected = 0;
  }
  return EMBB_SUCCESS;
}

int embb_spin_try_lock(embb_spinlock_t* spinlock,
  unsigned int max_number_spins) {
  if (NULL == spinlock) {
    return EMBB_ERROR;
  }
  if (max_number_spins == 0)
    return EMBB_BUSY;

  int expected = 0;
  while (0 == embb_atomic_compare_and_swap_int(
    &spinlock->atomic_spin_variable_,
    &expected, 1)) {
    max_number_spins--;
    if (0 == max_number_spins) {
      return EMBB_BUSY;
    }
    expected = 0;
  }

  return EMBB_SUCCESS;
}

int embb_spin_unlock(embb_spinlock_t* spinlock) {
  if (NULL == spinlock) {
    return EMBB_ERROR;
  }
  int expected = 1;
  return embb_atomic_compare_and_swap_int(&spinlock->atomic_spin_variable_,
    &expected, 0) ?
  EMBB_SUCCESS : EMBB_ERROR;
}

void embb_spin_destroy(embb_spinlock_t* spinlock) {
  assert(NULL != spinlock);
  // for now, doing nothing here... in future, will call the respective
  // destroy function for atomics...
  EMBB_UNUSED(spinlock);
}
