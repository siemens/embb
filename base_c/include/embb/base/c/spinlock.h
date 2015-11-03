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

#ifndef EMBB_BASE_C_SPINLOCK_H_
#define EMBB_BASE_C_SPINLOCK_H_

/**
 * \defgroup C_BASE_SPINLOCK Spinlock
 *
 * Spinlock for thread synchronization
 *
 * Provides an abstraction from platform-specific spinlock implementations.
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
 * Opaque type representing a spinlock.
 */
typedef opaque_type embb_spinlock_t;

#else

/**
 * Spinlock type, treat as opaque.
 */
struct embb_spinlock_t;
#endif /* DOXYGEN */


/**
 * Initializes a spinlock
 *
 * \post \c spinlock is initialized
 * \return EMBB_SUCCESS if spinlock could be initialized \n
 *         EMBB_ERROR otherwise
 * \memory (Potentially) allocates dynamic memory
 * \notthreadsafe
 * \see embb_spinlock_destroy()
 */
int embb_spin_init(
  embb_spinlock_t* spinlock
  /**< [OUT] Pointer to spinlock */
  );

/**
 * Spins until the spinlock can be locked and locks it.
 *
 * \pre \c spinlock is initialized \n
 * \post If successful, \c spinlock is locked.
 * \return EMBB_SUCCESS if spinlock could be locked \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_spinlock_try_lock(), embb_mutex_unlock()
 */
int embb_spin_lock(
  embb_spinlock_t* spinlock
  /**< [IN/OUT] Pointer to spinlock */
  );

/**
 * Tries to lock the spinlock and returns if not successful.
 *
 * \pre \c spinlock is initialized
 * \post If successful, \c spinlock is locked
 *
 * \return EMBB_SUCCESS if spinlock could be locked \n
 *         EMBB_BUSY if spinlock could not be locked \n
 *         EMBB_ERROR if an error occurred
 * \threadsafe
 * \see embb_spin_lock(), embb_spin_unlock()
 */
int embb_spin_try_lock(
  embb_spinlock_t* spinlock,
  /**< [IN/OUT] Pointer to spinlock */
  unsigned int number_spins
  /**< [IN] Number of attempts the locking operation is repeated if
   *        unsuccessful */
  );

/**
 * Unlocks a locked spinlock.
 *
 * \pre \c spinlock has been locked by the current thread.
 * \post If successful, \c spinlock is unlocked.
 * \return EMBB_SUCCESS if the operation was successful \n
 *         EMBB_ERROR otherwise
 * \threadsafe
 * \see embb_spin_lock(), embb_spin_try_lock()
 */
int embb_spin_unlock(
  embb_spinlock_t* spinlock
  /**< [IN/OUT] Pointer to spinlock */
  );

/**
 * Destroys a spinlock and frees its resources.
 *
 * \pre \c spinlock has been initialized
 * \post \c spinlock is uninitialized
 * \notthreadsafe
 * \see embb_spin_init()
 */
void embb_spin_destroy(
  embb_spinlock_t* spinlock
  /**< [IN/OUT] Pointer to spinlock */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_SPINLOCK_H_ */
