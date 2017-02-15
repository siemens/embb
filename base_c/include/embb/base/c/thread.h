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

#ifndef EMBB_BASE_C_THREAD_H_
#define EMBB_BASE_C_THREAD_H_

/**
 * \defgroup C_BASE_THREADS Thread
 *
 * Threads supporting thread-to-core affinities.
 *
 * Provides an abstraction from platform-specific threading implementations to
 * create, manage, and join threads of execution. Support for thread-to-core
 * affinities is given on thread creation by using the core set functionality.
 *
 * \ingroup C_BASE
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <embb/base/c/internal/platform.h>
#include <embb/base/c/time.h>
#include <embb/base/c/core_set.h>
#include <embb/base/c/errors.h>

#ifdef DOXYGEN
/**
 * Opaque type representing a thread of execution.
 */
typedef opaque_type embb_thread_t;
#endif /* DOXYGEN */

/**
 * Thread priority type.
 */
typedef enum {
  EMBB_THREAD_PRIORITY_IDLE,
  EMBB_THREAD_PRIORITY_LOWEST,
  EMBB_THREAD_PRIORITY_BELOW_NORMAL,
  EMBB_THREAD_PRIORITY_NORMAL,
  EMBB_THREAD_PRIORITY_ABOVE_NORMAL,
  EMBB_THREAD_PRIORITY_HIGHEST,
  EMBB_THREAD_PRIORITY_TIME_CRITICAL
} embb_thread_priority_t;

/**
 * Thread start function pointer type.
 *
 * The return value can be used to return a user-defined exit code when the
 * thread is joined.
 */
typedef int(*embb_thread_start_t)(void*);

/**
 * Returns the maximum number of threads handled by EMB<sup>2</sup>.
 *
 * The maximum thread number concerns all threads in a program using
 * EMB<sup>2</sup> functionalities or data structures, regardless of whether
 * a thread is started by EMB<sup>2</sup> or other threading libraries.
 * Each thread that makes use of EMB<sup>2</sup> at least once consumes one
 * entry in the internal tables. The entry is permanently consumed during a
 * program run, even if the thread does not exist any longer. If more threads
 * than the maximum thread count access EMB<sup>2</sup>, undefined behavior or
 * abortion of program execution can occur.
 *
 * \return Maximum number of threads
 *
 * \lockfree
 *
 * \see embb_thread_set_max_count()
 */
unsigned int embb_thread_get_max_count();

/**
 * Sets maximum number of threads handled by EMBB.
 *
 * It needs to be set before any EMB<sup>2</sup> functionalities are used or
 * data structures are defined, unless the default value is sufficient.
 *
 * \notthreadsafe
 * \see embb_thread_get_max_count()
 */
void embb_thread_set_max_count(
  unsigned int max
  /**< [IN] Maximum number of threads */
  );

/**
 * Returns the calling thread (that is, this thread).
 *
 * The returned handle is only valid for the thread calling the function.
 *
 * \return Calling thread
 *
 * \threadsafe
 */
embb_thread_t embb_thread_current();

/**
 * Reschedule the current thread for later execution.
 *
 * This is only a request, the realization depends on the implementation and the
 * scheduler employed by the operating system.
 *
 * \threadsafe
 */
void embb_thread_yield();

/**
 * Creates and runs a thread.
 *
 * \pre The given thread is not running and has not yet been successfully
 *      joined.
 * \post On success, the given thread has started to run.
 * \return EMBB_SUCCESS if the thread could be created. \n
 *         EMBB_NOMEM if there was insufficient amount of memory \n
 *         EMBB_ERROR otherwise.
 * \memory Dynamically allocates a small constant amount of memory to store the
 *         function and argument pointers. This memory is freed when the thread
 *         is joined.
 * \notthreadsafe
 * \see embb_thread_join()
 */
int embb_thread_create(
  embb_thread_t* thread,
  /**< [OUT] Thread to be run */
  const embb_core_set_t* core_set,
  /**< [IN] Set of cores on which the thread shall be executed. Can be NULL to
            indicate automatic thread scheduling by the OS. */
  embb_thread_start_t function,
  /**< [IN] Function which is executed by the thread when started. Has to be of
            type embb_thread_start_t. */
  void* arg
  /**< [IN/OUT] Argument to thread start function. Can be NULL. */
  );

/**
 * Creates and runs a thread.
 *
 * \pre The given thread is not running and has not yet been successfully
 *      joined.
 * \post On success, the given thread has started to run.
 * \return EMBB_SUCCESS if the thread could be created. \n
 *         EMBB_NOMEM if there was insufficient amount of memory \n
 *         EMBB_ERROR otherwise.
 * \memory Dynamically allocates a small constant amount of memory to store the
 *         function and argument pointers. This memory is freed when the thread
 *         is joined.
 * \notthreadsafe
 * \see embb_thread_join()
 */
int embb_thread_create_with_priority(
  embb_thread_t* thread,
  /**< [OUT] Thread to be run */
  const embb_core_set_t* core_set,
  /**< [IN] Set of cores on which the thread shall be executed. Can be NULL to
            indicate automatic thread scheduling by the OS. */
  embb_thread_priority_t priority,
  /**< [IN] Priority to run the thread at. */
  embb_thread_start_t function,
  /**< [IN] Function which is executed by the thread when started. Has to be of
            type embb_thread_start_t. */
  void* arg
  /**< [IN/OUT] Argument to thread start function. Can be NULL. */
  );

/**
 * Waits until the given thread has finished execution.
 *
 * \pre The given thread has been successfully created using
        embb_thread_create().
 * \post If successful, the thread has finished execution and all memory
 *       associated to the thread has been freed.
 * \return EMBB_SUCCESS if thread was joined \n
 *         EMBB_ERROR otherwise
 * \notthreadsafe
 * \see embb_thread_create()
 */
int embb_thread_join(
  embb_thread_t* thread,
  /**< [IN/OUT] Thread to be joined */
  int* result_code
  /**< [OUT] Memory location (or NULL) for thread result code */
  );

/**
 * Compares two threads represented by their handles for equality.
 *
 * \return Non-zero, if equal \n
 *         0, otherwise
 * \notthreadsafe
 */
int embb_thread_equal(
  const embb_thread_t* lhs,
  /**< [IN] First thread (left-hand side of equality sign) */
  const embb_thread_t* rhs
  /**< [IN] Second thread (right-hand side of equality sign) */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_THREAD_H_ */
