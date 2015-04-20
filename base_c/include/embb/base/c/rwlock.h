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

#ifndef EMBB_BASE_C_RWLOCK_H_
#define EMBB_BASE_C_RWLOCK_H_

/**
 * \defgroup C_BASE_RWLOCK Readers-Writers Lock
 *
 * Shared-read/exclusive-write lock for thread synchronization
 *
 * Provides an abstraction from platform-specific readers-writers lock
 * implementations.
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
 * Opaque type representing an rwlock.
 */
typedef opaque_type embb_rwlock_t;
#endif // DOXYGEN

/**
 * Initializes an rwlock
 *
 * \post \c rwlock is initialized
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if rwlock could be initialized \n
 *         EMBB_ERROR otherwise
 *
 * \memory (Potentially) allocates dynamic memory
 *
 * \notthreadsafe
 *
 * \see embb_rwlock_destroy()
 */
int embb_rwlock_init(embb_rwlock_t* rwlock);

/**
 * Waits until the rwlock can be locked for reading and locks it.
 *
 * \pre \c rwlock is initialized and not locked by the current thread.
 * \post If successful, \c rwlock is locked for reading.
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if rwlock could be locked for reading \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_rwlock_try_lock_read(), embb_rwlock_unlock_read()
 */
int embb_rwlock_lock_read(embb_rwlock_t* rwlock);

/**
 * Waits until the rwlock can be locked for writing and locks it.
 *
 * \pre \c rwlock is initialized and not locked by the current thread.
 * \post If successful, \c rwlock is locked for writing.
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if rwlock could be locked for writing \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_rwlock_try_lock_write(), embb_rwlock_unlock_write()
 */
int embb_rwlock_lock_write(embb_rwlock_t* rwlock);

/**
 * Tries to lock the rwlock for reading and returns immediately.
 *
 * \pre \c rwlock is initialized
 * \post If successful, \c rwlock is locked for reading
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if rwlock could be locked for reading \n
 *         EMBB_BUSY if rwlock could not be locked for reading \n
 *         EMBB_ERROR if an error occurred
 *
 * \threadsafe
 *
 * \see embb_rwlock_lock_read(), embb_rwlock_unlock_read()
 */
int embb_rwlock_try_lock_read(embb_rwlock_t* rwlock);

/**
 * Tries to lock the rwlock for writing and returns immediately.
 *
 * \pre \c rwlock is initialized
 * \post If successful, \c rwlock is locked for writing
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if rwlock could be locked for writing \n
 *         EMBB_BUSY if rwlock could not be locked for writing \n
 *         EMBB_ERROR if an error occurred
 *
 * \threadsafe
 *
 * \see embb_rwlock_lock_write(), embb_rwlock_unlock_write()
 */
int embb_rwlock_try_lock_write(embb_rwlock_t* rwlock);

/**
 * Unlocks an rwlock locked for reading.
 *
 * \pre \c rwlock has been locked for reading by the current thread.
 * \post If successful, \c rwlock is unlocked.
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if the operation was successful \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_rwlock_lock_read(), embb_rwlock_try_lock_read()
 */
int embb_rwlock_unlock_read(embb_rwlock_t* rwlock);

/**
 * Unlocks an rwlock locked for writing.
 *
 * \pre \c rwlock has been locked for writing by the current thread.
 * \post If successful, \c rwlock is unlocked.
 *
 * \param rwlock Pointer to rwlock
 * \return EMBB_SUCCESS if the operation was successful \n
 *         EMBB_ERROR otherwise
 *
 * \threadsafe
 *
 * \see embb_rwlock_lock_write(), embb_rwlock_try_lock_write()
 */
int embb_rwlock_unlock_write(embb_rwlock_t* rwlock);

/**
 * Destroys an rwlock and frees its resources.
 *
 * \pre \c rwlock has been initialized
 * \post \c rwlock is uninitialized
 *
 * \param rwlock Pointer to rwlock
 *
 * \notthreadsafe
 *
 * \see embb_rwlock_init()
 */
void embb_rwlock_destroy(embb_rwlock_t* rwlock);

#ifdef __cplusplus
} // Close extern "C"
#endif

/**
 * \}
 */

#endif // EMBB_BASE_C_RWLOCK_H_
