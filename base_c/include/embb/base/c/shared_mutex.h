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

#ifndef EMBB_BASE_C_SHARED_MUTEX_H_
#define EMBB_BASE_C_SHARED_MUTEX_H_

/**
 * \defgroup C_BASE_SHARED_MUTEX Shared mutex
 *
 * Shared mutexes for thread synchronization
 *
 * Provides an abstraction from platform-specific shared mutex implementations.
 * Shared mutexes provide two levels of access: exclusive access - only one
 * thread can own the mutex; shared access - several threads can share ownership
 * of the same mutex. Shared mutexes facilitate the "single writer, multiple
 * readers" scenarios.
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
 * Opaque type representing a shared mutex.
 */
typedef opaque_type embb_shared_mutex_t;
#endif // DOXYGEN

/**
 * Initializes a shared mutex
 *
 * \post \c shared_mutex is initialized
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if shared mutex could be initialized \n
 *         EMBB_ERROR otherwise
 *
 * \memory (Potentially) allocates dynamic memory
 *
 * \notthreadsafe
 *
 * \see embb_shared_mutex_destroy()
 */
int embb_shared_mutex_init(embb_shared_mutex_t* shared_mutex);

/**
 * Waits until the shared mutex can be locked for writing and locks it.
 *
 * \pre \c shared_mutex is initialized and not locked by the current thread.
 * \post If successful, \c shared_mutex is locked for writing.
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if shared_mutex could be locked for writing \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_shared_mutex_try_lock(), embb_shared_mutex_unlock()
 */
int embb_shared_mutex_lock(embb_shared_mutex_t* shared_mutex);

/**
 * Tries to lock the shared mutex for writing and returns immediately.
 *
 * \pre \c shared_mutex is initialized
 * \post If successful, \c shared_mutex is locked for writing
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if shared_mutex could be locked for writing \n
 *         EMBB_BUSY if shared_mutex could not be locked for writing \n
 *         EMBB_ERROR if an error occurred
 *
 * \threadsafe
 *
 * \see embb_shared_mutex_lock(), embb_shared_mutex_unlock()
 */
int embb_shared_mutex_try_lock(embb_shared_mutex_t* shared_mutex);

/**
 * Unlocks the shared mutex locked for writing.
 *
 * \pre \c shared_mutex has been locked for writing by the current thread.
 * \post If successful, \c shared_mutex is unlocked.
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if the operation was successful \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_shared_mutex_lock(), embb_shared_mutex_try_lock()
 */
int embb_shared_mutex_unlock(embb_shared_mutex_t* shared_mutex);

/**
 * Waits until the shared mutex can be locked for reading and locks it.
 *
 * \pre \c shared_mutex is initialized and not locked by the current thread.
 * \post If successful, \c shared_mutex is locked for reading.
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if shared_mutex could be locked for reading \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_shared_mutex_try_lock_shared(), embb_shared_mutex_unlock_shared()
 */
int embb_shared_mutex_lock_shared(embb_shared_mutex_t* shared_mutex);

/**
 * Tries to lock the shared mutex for reading and returns immediately.
 *
 * \pre \c shared_mutex is initialized
 * \post If successful, \c shared_mutex is locked for reading
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if shared_mutex could be locked for reading \n
 *         EMBB_BUSY if shared_mutex could not be locked for reading \n
 *         EMBB_ERROR if an error occurred
 *
 * \threadsafe
 *
 * \see embb_shared_mutex_lock_shared(), embb_shared_mutex_unlock_shared()
 */
int embb_shared_mutex_try_lock_shared(embb_shared_mutex_t* shared_mutex);

/**
 * Unlocks the shared mutex locked for reading.
 *
 * \pre \c shared_mutex has been locked for reading by the current thread.
 * \post If successful, \c shared_mutex is unlocked.
 *
 * \param shared_mutex Pointer to shared mutex
 * \return EMBB_SUCCESS if the operation was successful \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_shared_mutex_lock_shared(), embb_shared_mutex_try_lock_shared()
 */
int embb_shared_mutex_unlock_shared(embb_shared_mutex_t* shared_mutex);

/**
 * Destroys the shared mutex and frees its resources.
 *
 * \pre \c shared_mutex has been initialized
 * \post \c shared_mutex is uninitialized
 *
 * \param shared_mutex Pointer to shared mutex
 *
 * \notthreadsafe
 *
 * \see embb_shared_mutex_init()
 */
void embb_shared_mutex_destroy(embb_shared_mutex_t* shared_mutex);

#ifdef __cplusplus
} // Close extern "C"
#endif

/**
 * \}
 */

#endif // EMBB_BASE_C_SHARED_MUTEX_H_
