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

#ifndef EMBB_BASE_C_DURATION_H_
#define EMBB_BASE_C_DURATION_H_

#include <embb/base/c/errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup C_BASE_DURATIONTIME Duration and Time
 *
 * Relative time durations and absolute time points
 *
 * \ingroup C_BASE
 * \{
 *
 * \name Duration
 * \{
 */

/**
 * Opaque type representing a relative time duration.
 */
#ifdef DOXYGEN
typedef opaque_type embb_duration_t;
#else
typedef struct embb_duration_t {
  /**
   * Seconds part of duration
   */
  unsigned long long seconds;
  /**
   * Nanoseconds part of duration, smaller than a second
   */
  unsigned long nanoseconds;
} embb_duration_t;
#endif /* else defined(DOXYGEN) */

/**
 * Macro for initializing a duration with zero length at definition.
 */
#ifdef DOXYGEN
#define EMBB_DURATION_INIT
#else
#define EMBB_DURATION_INIT {0, 0}
#endif /* defined(DOXYGEN) */

/**
 * Returns duration with maximum ticks representable by implementation.
 *
 * \return Pointer to duration with maximum value
 * \notthreadsafe
 * \see embb_duration_min()
 */
const embb_duration_t* embb_duration_max();

/**
 * Returns duration with minimum ticks representable by implementation.
 *
 * \return Pointer to duration with minimum value
 * \notthreadsafe
 * \see embb_duration_max()
 */
const embb_duration_t* embb_duration_min();

/**
 * Returns duration of length zero.
 *
 * \return Pointer to duration of length zero
 * \notthreadsafe
 */
const embb_duration_t* embb_duration_zero();

/**
 * Set duration from nanosecond ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if given nanosecond interval is too small to be
 *                        represented by implementation \n
 *         EMBB_OVERFLOW if given nanosecond interval is too large to be
 *                       represented by implementation
 * \notthreadsafe
 */
int embb_duration_set_nanoseconds(
  embb_duration_t* duration,
  /**< [OUT] Pointer to duration */
  unsigned long long nanoseconds
  /**< [IN] Nanosecond ticks */
  );

/**
 * Sets duration from microsecond ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if given microsecond interval is too small to be
 *                        represented by implementation \n
 *         EMBB_OVERFLOW if given microsecond interval is too large to be
 *                       represented by implementation
 * \notthreadsafe
 */
int embb_duration_set_microseconds(
  embb_duration_t* duration,
  /**< [OUT] Pointer to duration */
  unsigned long long microseconds
  /**< [IN] Microsecond ticks */
  );

/**
 * Sets duration from millisecond ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if given millisecond interval is too small to be
 *                        represented by implementation \n
 *         EMBB_OVERFLOW if given millisecond interval is too large to be
 *                       represented by implementation \n
 * \notthreadsafe
 */
int embb_duration_set_milliseconds(
  embb_duration_t* duration,
  /**< [OUT] Pointer to duration */
  unsigned long long milliseconds
  /**< [IN] Millisecond ticks */
  );

/**
 * Sets duration from second ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if given second interval is too small to be
 *                        represented by implementation \n
 *         EMBB_OVERFLOW if given second interval is too large to be
 *                       represented by implementation
 * \notthreadsafe
 */
int embb_duration_set_seconds(
  embb_duration_t* duration,
  /**< [OUT] Pointer to duration */
  unsigned long long seconds
  /**< [IN] Second ticks */
  );

/**
 * Adds two durations.
 *
 * Computest the sum of \c rhs and \c lhs and stores the result in \c lhs.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_OVERFLOW if sum is greater than embb_duration_max()
 * \notthreadsafe
 */
int embb_duration_add(
  embb_duration_t* lhs,
  /**< [IN/OUT] Left-hand side operand, overwritten by result of addition */
  const embb_duration_t* rhs
  /**< [IN] Right-hand side operand of addition */
  );

/**
 * Converts duration to nanosecond ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if duration contains fractions less than a
 *         nanosecond \n
 *         EMBB_OVERFLOW if duration is not representable by tick type
 * \notthreadsafe
 */
int embb_duration_as_nanoseconds(
  const embb_duration_t* duration,
  /**< [IN] Pointer to duration */
  unsigned long long* nanoseconds
  /**< [OUT] Pointer to nanosecond ticks of duration */
  );

/**
 * Converts duration to microsecond ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if duration contains fractions less than a
 *         microsecond \n
 *         EMBB_OVERFLOW if duration is not representable by tick type
 * \notthreadsafe
 */
int embb_duration_as_microseconds(
  const embb_duration_t* duration,
  /**< [IN] Pointer to duration */
  unsigned long long* microseconds
  /**< [OUT] Pointer to microsecond ticks of duration */
  );

/**
 * Converts duration to millisecond ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if duration contains fractions less than a
 *         millisecond \n
 *         EMBB_OVERFLOW if duration is not representable by tick type
 * \notthreadsafe
 */
int embb_duration_as_milliseconds(
  const embb_duration_t* duration,
  /**< [IN] Pointer to duration */
  unsigned long long* milliseconds
  /**< [OUT] Pointer to millisecond ticks of duration */
  );

/**
 * Converts duration to second ticks.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if duration contains fractions less than a
 *         second \n
 *         EMBB_OVERFLOW if duration is not representable by tick type
 * \notthreadsafe
 */
int embb_duration_as_seconds(
  const embb_duration_t* duration,
  /**< [IN] Pointer to duration */
  unsigned long long* seconds
  /**< [OUT] Pointer to second ticks of duration */
  );

/**
 * Compares two durations.
 *
 * \pre \c lhs and \c rhs are not NULL and properly initialized.
 *
 * \return -1 if \c lhs < \c rhs \n
 *         0 if \c lhs == \c rhs \n
 *         1 if \c lhs > \c rhs
 * \notthreadsafe
 */
int embb_duration_compare(
  const embb_duration_t* lhs,
  /**< [IN] Pointer to left-hand side operand */
  const embb_duration_t* rhs
  /**< [IN] Pointer to right-hand side operand */
  );

/**
 * \}
 * \}
 */

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

#endif /* EMBB_BASE_C_DURATION_H_ */
