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

#ifndef EMBB_BASE_C_COUNTER_H_
#define EMBB_BASE_C_COUNTER_H_

#include <embb/base/c/atomic.h>

/**
 * \defgroup C_BASE_COUNTER Counter
 * Thread-safe counter
 * \ingroup C_BASE
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque type representing a thread-safe counter.
 */
#ifdef DOXYGEN
typedef opaque_type embb_counter_t;
#else
typedef struct embb_counter_t {
  embb_atomic_unsigned_int value;
} embb_counter_t;
#endif /* else defined(DOXYGEN) */

/**
 * Initializes \c counter and sets it to zero.
 *
 * \return EMBB_SUCCESS if counter could be initialized \n
 *         EMBB_ERROR otherwise
 *
 * \waitfree
 */
int embb_counter_init(
  embb_counter_t* counter
  /**< [OUT] Pointer to counter */
  );

/**
 * Returns the current value of \c counter.
 *
 * \pre \c counter is not NULL.
 *
 * \return Current value 
 *
 * \waitfree
 */
unsigned int embb_counter_get(
  embb_counter_t* counter
  /**< [IN] Pointer to counter */
  );

/**
 * Increments \c counter and returns the old value.
 *
 * \pre \c counter is not NULL.
 *
 * \return Old, non-incremented value
 * \waitfree
 */
unsigned int embb_counter_increment(
  embb_counter_t* counter
  /**< [IN,OUT] Pointer to counter */
  );

/**
 * Decrements \c counter and returns the old value.
 *
 * \pre \c counter is not NULL.
 *
 * \return Old, non-decremented value
 * \waitfree
 */
unsigned int embb_counter_decrement(
  embb_counter_t* counter
  /**< [IN,OUT] Pointer to counter */
  );

/**
 * Resets an initialized counter to 0.
 *
 * \pre \c counter is initialized and not NULL.
 * \waitfree
 */
void embb_counter_reset(
  embb_counter_t* counter
  /**< [IN,OUT] Pointer to counter */
  );

/**
 * Destroys an initialized counter.
 *
 * \pre \c counter is initialized and not NULL.
 * \post \c counter is invalid and cannot be used anymore
 * \waitfree
 */
void embb_counter_destroy(
  embb_counter_t* counter
  /**< [OUT] Pointer to counter */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_COUNTER_H_ */
