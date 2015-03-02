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

#include <embb/base/c/thread.h>
#include <embb/base/c/internal/thread_index.h>
#include <embb/base/c/internal/cmake_config.h>
#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/log.h>
#include <stdlib.h>
#include <assert.h>

unsigned int embb_thread_get_max_count() {
  return (unsigned int)(embb_internal_thread_index_max());
}

void embb_thread_set_max_count(unsigned int max) {
  embb_internal_thread_index_set_max(max);
}

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

/**
 * Used to wrap client thread start function and argument when calling internal
 * thread start function embb_internal_thread_start.
 */
typedef struct embb_internal_thread_arg_t {
  embb_thread_start_t func;
  void* arg;
} embb_internal_thread_arg_t;

/**
 * Used to offer a consistent thread start function signature. Windows threads
 * have a different signature than Pthreads and C11. This internal start
 * function for Windows threads just calls the client start function with the
 * given argument.
 */
DWORD WINAPI embb_internal_thread_start(LPVOID internalArg) {
  int result = ((embb_internal_thread_arg_t*)internalArg)->func(
             ((embb_internal_thread_arg_t*)internalArg)->arg);
#if !defined(__cplusplus)
  ExitThread((DWORD)result); /* In C, returning the result code doesn't work */
#else
  return (DWORD)result;
#endif
}

embb_thread_t embb_thread_current() {
  embb_thread_t thread;
  thread.embb_internal_handle = GetCurrentThread();
  thread.embb_internal_arg = NULL;
  return thread;
}

void embb_thread_yield() {
  SwitchToThread();
}

int embb_thread_create(embb_thread_t* thread, const embb_core_set_t* core_set,
                       embb_thread_start_t func, void *arg) {
  assert(thread != NULL);
  thread->embb_internal_arg = (embb_internal_thread_arg_t*)
                              embb_alloc(sizeof(embb_internal_thread_arg_t));
  if (thread->embb_internal_arg == NULL) return EMBB_NOMEM;
  thread->embb_internal_arg->func = func;
  thread->embb_internal_arg->arg = arg;

  thread->embb_internal_handle = CreateThread(
      0,                                  /* no security */
      0,                                  /* default stack size */
      embb_internal_thread_start,         /* entry function */
      (LPVOID)thread->embb_internal_arg,  /* parameters */
      0,                                  /* no creation arguments */
      0);                                 /* no system thread ID */
  if (thread->embb_internal_handle == NULL) {
    return EMBB_ERROR;
  }

  if (core_set != NULL) { /* Set thread affinity, if a core set is given */
    DWORD_PTR core_mask = 0;
    DWORD bit_mask = 1;
    assert(embb_core_count_available() < 64);
    for (unsigned int i = 0; i < embb_core_count_available(); i++) {
      if (embb_core_set_contains(core_set, i)) {
        core_mask |= bit_mask;
      }
      bit_mask <<= 1;
    }
    if (SetThreadAffinityMask(thread->embb_internal_handle, core_mask)
        == (DWORD_PTR)NULL) {
      return EMBB_ERROR;
    }
  }

  return EMBB_SUCCESS;
}

int embb_thread_join(embb_thread_t* thread, int* result_code) {
  BOOL success;
  DWORD result;
  result = WaitForSingleObject(thread->embb_internal_handle, INFINITE);
  embb_free(thread->embb_internal_arg);
  if (result != WAIT_OBJECT_0) {
    /* WAIT_OBJECT_0 indicates successful waiting */
    return EMBB_ERROR;
  }
  if (result_code != NULL) { /* != NULL means the client wants a result code */
    if (GetExitCodeThread(thread->embb_internal_handle, &result) != 0) {
      *result_code = (int)result;
    } else {
      *result_code = 0; /* Error on obtaining result code */
      return EMBB_ERROR;
    }
  }
  success = CloseHandle(thread->embb_internal_handle);
  if (success == FALSE) {
    return EMBB_ERROR;
  }
  /*return embb_internal_thread_counter_try_decrement();*/
  return EMBB_SUCCESS;
}

int embb_thread_equal(const embb_thread_t* lhs, const embb_thread_t* rhs) {
  embb_thread_id_t idLhs = GetThreadId(lhs->embb_internal_handle);
  embb_thread_id_t idRhs = GetThreadId(rhs->embb_internal_handle);
  if (idLhs == idRhs) {
    return 1;
  }
  return 0;
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

#ifdef EMBB_PLATFORM_HAS_GLIB_CPU
#include <sched.h>
#elif defined EMBB_PLATFORM_HAS_HEADER_CPUSET
#include <pthread_np.h>
#include <sys/param.h>
#include <sys/cpuset.h>
#endif

#ifdef EMBB_PLATFORM_HAS_HEADER_SYSINFO
#include <sys/sysinfo.h> /* Used to get number of processors */
#endif /* EMBB_PLATFORM_HAS_HEADER_SYSINFO */

/**
 * Used to wrap client thread start function and argument when calling internal
 * thread start function embb_internal_thread_start.
 */
typedef struct embb_internal_thread_arg_t {
  embb_thread_start_t func;
  void* arg;
  int result;
} embb_internal_thread_arg_t;

/**
 * Used to offer a consistent thread start function signature. POSIX threads
 * have a different signature than C11 threads. This internal start function
 * for POSIX threads just calls the client start function with the given
 * argument.
 */
void* embb_internal_thread_start(void* internalArg) {
  ((embb_internal_thread_arg_t*)internalArg)->result =
      ((embb_internal_thread_arg_t*)internalArg)->func(
      ((struct embb_internal_thread_arg_t*)internalArg)->arg);
  return NULL;
}

embb_thread_t embb_thread_current() {
  embb_thread_t thread;
  thread.embb_internal_handle = pthread_self();
  thread.embb_internal_arg = NULL;
  return thread;
}

void embb_thread_yield() {
  pthread_yield();
}

int embb_thread_create(embb_thread_t* thread, const embb_core_set_t* core_set,
                       embb_thread_start_t func, void* arg) {
  pthread_attr_t attr; /* Used to set thread affinities */
  int status = pthread_attr_init(&attr);
  if (status != 0) return EMBB_ERROR;
  if (core_set != NULL) {
#if defined(EMBB_PLATFORM_HAS_GLIB_CPU) || \
  defined(EMBB_PLATFORM_HAS_HEADER_CPUSET)
    assert(embb_core_count_available() < CPU_SETSIZE &&
      "Core sets are only supported up to CPU_SETSIZE processors!");
#ifdef EMBB_PLATFORM_HAS_GLIB_CPU
    cpu_set_t cpuset;
#else
    cpuset_t cpuset;
#endif
    CPU_ZERO(&cpuset); /* Disable all processors */
    for (unsigned int i = 0; i < embb_core_count_available(); i++) {
      if (embb_core_set_contains(core_set, i)) {
        CPU_SET(i, &cpuset);
      }
    }
    status = pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset);
    if (status != 0) return EMBB_ERROR;
#else
    embb_log_write("base_c", EMBB_LOG_LEVEL_WARNING, "Could not set thread "
                   "affinity, since no implementation available!\n");
#endif
  }

  /* Dynamic allocation of thread arguments. Freed on call of join. */
  thread->embb_internal_arg = (embb_internal_thread_arg_t*)
                              embb_alloc(sizeof(embb_internal_thread_arg_t));
  thread->embb_internal_arg->func = func;
  thread->embb_internal_arg->arg = arg;

  status = pthread_create(
      &(thread->embb_internal_handle),     /* pthread handle */
      &attr,                               /* additional attributes,
                                              e.g., affinities */
      embb_internal_thread_start,          /* thread start function */
      (void*)(thread->embb_internal_arg)); /* arguments to thread start func. */
  if (status != 0) return EMBB_ERROR;

  status = pthread_attr_destroy(&attr);
  if (status != 0) return EMBB_ERROR;
  return EMBB_SUCCESS;
}

int embb_thread_join(embb_thread_t* thread, int *result_code) {
  int status = 0;
  status = pthread_join(thread->embb_internal_handle, NULL);
  if (result_code != NULL) {
    *result_code = thread->embb_internal_arg->result;
  }
  embb_free(thread->embb_internal_arg);
  if (status != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_thread_equal(const embb_thread_t* lhs, const embb_thread_t* rhs) {
  return pthread_equal(lhs->embb_internal_handle, rhs->embb_internal_handle);
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */
