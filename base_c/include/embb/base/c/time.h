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

#ifndef EMBB_BASE_C_TIME_H_
#define EMBB_BASE_C_TIME_H_

/**
 * \addtogroup C_BASE_DURATIONTIME
 * \{
 *
 * \name Time
 * \{
 */

#include <embb/base/c/duration.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque type representing an absolute time point
 */
#ifdef DOXYGEN
typedef opaque_type embb_time_t;
#else
typedef struct embb_time_t {
  /**
   * Seconds part of time point
   */
  unsigned long long seconds;
  /**
   * Nanoseconds part of time point, smaller than one second
   */
  unsigned long nanoseconds;
} embb_time_t;
#endif /* else defined(DOXYGEN) */

/**
 * Sets time point to now.
 *
 * \notthreadsafe
 * \see embb_time_in()
 */
void embb_time_now(
  embb_time_t* time
  /**< [OUT] Pointer to time point */
  );

/**
 * Sets time point to now plus the given duration.
 *
 * \return EMBB_SUCCESS \n
 *         EMBB_UNDERFLOW if duration is smaller than implementation allows \n
 *         EMBB_OVERFLOW if time + duration is larger than implementation allows
 * \notthreadsafe
 * \see embb_time_now()
 */
int embb_time_in(
  embb_time_t* time,
  /**< [OUT] Pointer to time point */
  const embb_duration_t* duration
  /**< [IN] Pointer to duration */
  );

/**
 * Compares two time points.
 *
 * \pre \c lhs and \c rhs are not NULL and properly initialized.
 *
 * \return -1 if \c lhs < \c rhs \n
 *         0 if \c lhs == \c rhs \n
 *         1 if \c lhs > \c rhs
 * \notthreadsafe
 */
int embb_time_compare(
  const embb_time_t* lhs,
  /**< [IN] Pointer to left-hand side operand */
  const embb_time_t* rhs
  /**< [IN] Pointer to right-hand side operand */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 * \}
 */

#endif /* EMBB_BASE_C_TIME_H_ */
