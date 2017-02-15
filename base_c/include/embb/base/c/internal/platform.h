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

#ifndef EMBB_BASE_C_INTERNAL_PLATFORM_H_
#define EMBB_BASE_C_INTERNAL_PLATFORM_H_

#include <embb/base/c/internal/config.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMBB_TIME_MAX_SECONDS ULLONG_MAX
#define EMBB_DURATION_MAX_SECONDS 60 * 60 * 24 * 7

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

#ifdef EMBB_PLATFORM_COMPILER_MSVC
// Suppress virtual functions but non-virtual constructor warning
// in windows headers
#pragma warning(push)
#pragma warning(disable : 4265)
#endif

#define NOMINMAX
#include <windows.h>

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop) // Reset warning 4640
#endif

struct embb_internal_thread_arg_t;

/**
 * Opaque handle for a thread.
 */
typedef struct embb_thread_t {
  HANDLE embb_internal_handle;
  struct embb_internal_thread_arg_t* embb_internal_arg;
} embb_thread_t;

typedef DWORD embb_thread_id_t;
typedef CRITICAL_SECTION embb_mutex_t;
typedef CONDITION_VARIABLE embb_condition_t;

#define EMBB_DURATION_MIN_NANOSECONDS 1000

#define EMBB_THREAD_SPECIFIC static __declspec(thread)

#elif defined EMBB_PLATFORM_THREADING_POSIXTHREADS

#include <pthread.h>
#include <errno.h>
#include <time.h>

struct embb_internal_thread_arg_t;

/**
 * Opaque handle for a thread.
 */
typedef struct embb_thread_t {
  pthread_t embb_internal_handle;
  struct embb_internal_thread_arg_t* embb_internal_arg;
} embb_thread_t;

typedef pthread_t embb_thread_id_t;
typedef pthread_mutex_t embb_mutex_t;
typedef pthread_cond_t embb_condition_t;

#define EMBB_DURATION_MIN_NANOSECONDS 1

#define EMBB_THREAD_SPECIFIC __thread

#else /* EMBB_PLATFORM_THREADING_POSIXTHREADS */

#error "No threading platform defined!"

#endif /* else */

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

#endif  // EMBB_BASE_C_INTERNAL_PLATFORM_H_
