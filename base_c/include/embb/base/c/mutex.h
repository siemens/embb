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

#ifndef EMBB_BASE_C_MUTEX_H_
#define EMBB_BASE_C_MUTEX_H_

/**
 * \defgroup C_BASE_MUTEX Mutex
 *
 * Mutexes for thread synchronization
 *
 * Provides an abstraction from platform-specific mutex implementations.
 * Plain and recursive mutexes are available, where the plain version can
 * only be locked once by the same thread.
 *
 * \ingroup C_BASE
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <embb/base/c/internal/platform.h>
#include <embb/base/c/errors.h>

#ifdef DOXYGEN
/**
 * Opaque type representing a mutex.
 */
typedef opaque_type embb_mutex_t;
#endif /* DOXYGEN */

/**
 * Types of mutexes to be used in embb_mutex_init()
 */
enum {
  EMBB_MUTEX_PLAIN,
  /**< Mutex can be locked only once by the same thread. */
  EMBB_MUTEX_RECURSIVE
  /**< Mutex can be locked recursively by the same thread. */
};

/**
 * Initializes a mutex
 *
 * \post \c mutex is initialized
 * \return EMBB_SUCCESS if mutex could be initialized \n
 *         EMBB_ERROR otherwise
 * \memory (Potentially) allocates dynamic memory
 * \notthreadsafe
 * \see embb_mutex_destroy()
 */
int embb_mutex_init(
  embb_mutex_t* mutex,
  /**< [OUT] Pointer to mutex */
  int type
  /**< [IN] EMBB_MUTEX_PLAIN or EMBB_MUTEX_RECURSIVE. There is no guarantee
       that a mutex is non-recursive if the plain type is given. */
  );

/**
 * Waits until the mutex can be locked and locks it.
 *
 * \pre \c mutex is initialized \n
 *      If the mutex type is plain, \c mutex must not be locked by the current
 *      thread.
 * \post If successful, \c mutex is locked.
 * \return EMBB_SUCCESS if mutex could be locked \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_mutex_try_lock(), embb_mutex_unlock()
 */
int embb_mutex_lock(
  embb_mutex_t* mutex
  /**< [IN/OUT] Pointer to mutex */
  );

/**
 * Tries to lock the mutex and returns immediately.
 *
 * \pre \c mutex is initialized
 * \post If successful, \c mutex is locked
 *
 * \return EMBB_SUCCESS if mutex could be locked \n
 *         EMBB_BUSY if mutex could not be locked \n
 *         EMBB_ERROR if an error occurred
 * \threadsafe
 * \see embb_mutex_lock(), embb_mutex_unlock()
 */
int embb_mutex_try_lock(
  embb_mutex_t* mutex
  /**< [IN/OUT] Pointer to mutex */
  );

/**
 * Unlocks a locked mutex.
 *
 * \pre \c mutex has been locked by the current thread.
 * \post If successful and when the given mutex type is plain, \c mutex is
 *       unlocked. If its type is recursive, \c mutex is only unlocked if the
 *       number of successful unlocks has reached the number of successful locks
 *       done by the current thread.
 * \return EMBB_SUCCESS if the operation was successful \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_mutex_lock(), embb_mutex_try_lock()
 */
int embb_mutex_unlock(
  embb_mutex_t* mutex
  /**< [IN/OUT] Pointer to mutex */
  );

/**
 * Destroys a mutex and frees its resources.
 *
 * \pre \c mutex has been initialized
 * \post \c mutex is uninitialized
 * \notthreadsafe
 * \see embb_mutex_init()
 */
void embb_mutex_destroy(
  embb_mutex_t* mutex
  /**< [IN/OUT] Pointer to mutex */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_MUTEX_H_ */
