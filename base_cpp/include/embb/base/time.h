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

#ifndef EMBB_BASE_TIME_H_
#define EMBB_BASE_TIME_H_

#include <embb/base/duration.h>
#include <embb/base/c/time.h>

namespace embb {
namespace base {

/**
 * Represents an absolute time point.
 *
 * \ingroup CPP_BASE_TIMEDURATION
 */
class Time {
 public:
  /**
   * Constructs an instance representing the current point of time.
   *
   * \notthreadsafe
   */
  Time();

  /**
   * Constructs an instance representing the current point of time plus
   * \c duration.
   *
   * \notthreadsafe
   * \see Duration
   * \tparam Tick Type of tick of the Duration
   */
  template<typename Tick>
  explicit Time(
    const Duration<Tick>& duration
    /**< [IN] %Duration added to the current point of time. */
    ) : rep_() {
    embb_time_in(&rep_, &(duration.rep_));
  }

 private:
  /**
   * Representation of the absolute time point.
   */
  embb_time_t rep_;

  /**
   * Needs to access the internal time representation.
   */
  friend class ConditionVariable;
};

} // namespace base
} // namespace embb

#endif  // EMBB_BASE_TIME_H_
