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

#ifndef EMBB_BASE_C_CONDITION_VARIABLE_H_
#define EMBB_BASE_C_CONDITION_VARIABLE_H_

/**
 * \defgroup C_BASE_CONDITION Condition Variable
 * Condition variables for thread synchronization.
 *
 * Provides an abstraction from platform-specific condition variable
 * implementations. Condition variables can be waited for with timeouts using
 * relative durations and absolute time points.
 *
 * \ingroup C_BASE
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <embb/base/c/internal/platform.h>
#include <embb/base/c/time.h>
#include <embb/base/c/errors.h>

#ifdef DOXYGEN
/**
 * Opaque type representing a condition variable.
 */
typedef opaque_type embb_condition_t;
#endif /* DOXYGEN */

/**
 * Initializes a condition variable.
 *
 * \memory Potentially allocates dynamic memory
 * \pre \c condition_var is not initialized
 * \post If successful, \c condition_var is initialized
 * \return EMBB_SUCCESS if successful \n
 *         EMBB_ERROR otherwise
 * \notthreadsafe
 * \see embb_condition_destroy()
 */
int embb_condition_init(
  embb_condition_t* condition_var
  /**< [OUT] Pointer to condition variable */
  );

/**
 * Wakes up one thread waiting for \c condition_var.
 *
 * \pre \c condition_var is initialized
 * \return EMBB_SUCCESS if signaling was successful \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_condition_notify_all(), embb_condition_wait(),
 *      embb_condition_wait_until(), embb_condition_wait_for()
 */
int embb_condition_notify_one(
  embb_condition_t* condition_var
  /**< [IN/OUT] Pointer to condition variable */
  );

/**
 * Wakes up all threads waiting for \c condition_var.
 *
 * \pre \c condition_var is initialized
 * \return EMBB_SUCCESS if broadcast was successful \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_condition_notify_one(), embb_condition_wait(),
 *      embb_condition_wait_until(), embb_condition_wait_for()
 */
int embb_condition_notify_all(
  embb_condition_t* condition_var
  /**< [IN/OUT] Pointer to condition variable */
  );

/**
 * Unlocks \c mutex and waits until the thread is woken up.
 *
 * \pre \c condition_var is initialized and \c mutex is locked by calling thread
 * \post If successful, \c mutex is locked by the calling thread
 * \return EMBB_SUCCESS if successful \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_condition_notify_one(), embb_condition_notify_all(),
 *      embb_condition_wait_until(), embb_condition_wait_for()
 * \note It is strongly recommended checking the condition in a loop in order
 *       to deal with spurious wakeups and situations where another thread has
 *       locked the mutex between notification and wakeup.
 */
int embb_condition_wait(
  embb_condition_t* condition_var,
  /**< [IN/OUT] Pointer to condition variable */
  embb_mutex_t* mutex
  /**< [IN/OUT] Pointer to mutex */
  );

/**
 * Unlocks \c mutex and waits until the thread is woken up or \c time has
 * passed.
 *
 * \pre \c condition_var is initialized and \c mutex is locked by calling thread
 * \post If successful, \c mutex is locked by the calling thread
 * \return EMBB_SUCCESS if successful \n
 *         EMBB_TIMEDOUT if mutex could not be locked until the specified point
 *         of time \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_condition_notify_one(), embb_condition_notify_all(),
 *      embb_condition_wait(), embb_condition_wait_for()
 * \note It is strongly recommended checking the condition in a loop in order
 *       to deal with spurious wakeups and situations where another thread has
 *       locked the mutex between notification and wakeup.
 */
int embb_condition_wait_until(
  embb_condition_t* condition_var,
  /**< [IN/OUT] Pointer to condition variable */
  embb_mutex_t* mutex,
  /**< [IN/OUT] Pointer to mutex */
  const embb_time_t* time
  /**< [IN] Point of time until the thread waits */
  );

/**
 * Unlocks \c mutex and waits until the thread is woken up or \c duration
 * has passed.
 *
 * \pre \c condition_var is initialized and \c mutex is locked by calling thread
 * \post If successful, \c mutex is locked by the calling thread
 * \return EMBB_SUCCESS if successful \n
 *         EMBB_TIMEDOUT if mutex could not be locked within the specified time
 *         span \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_condition_notify_one(), embb_condition_notify_all(),
 *      embb_condition_wait(), embb_condition_wait_until()
 * \note It is strongly recommended checking the condition in a loop in order
 *       to deal with spurious wakeups and situations where another thread has
 *       locked the mutex between notification and wakeup.
 */
int embb_condition_wait_for(
  embb_condition_t* condition_var,
  /**< [IN/OUT] Pointer to condition variable */
  embb_mutex_t* mutex,
  /**< [IN/OUT] Pointer to mutex */
  const embb_duration_t* duration
  /**< [IN] Duration in microseconds the thread waits */
  );

/**
 * Destroys \c condition_var and frees used memory.
 *
 * \pre \c condition_var is initialized and no thread is waiting for it using
 *      embb_condition_wait(), embb_condition_wait_for(), or
 *      embb_condition_wait_until().
 * \post \c condition_var is uninitialized
 * \return EMBB_SUCCESS if destruction of condition variable was successful \n
 *         EMBB_ERROR otherwise
 * \notthreadsafe
 * \see embb_condition_init()
 */
int embb_condition_destroy(
  embb_condition_t* condition_var
  /**< [IN] Pointer to condition variable */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_CONDITION_VARIABLE_H_ */
