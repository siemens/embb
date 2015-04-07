/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_PAPI_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_PAPI_H_

#include <embb/base/perf/timer.h>
#include <embb/base/perf/time_measure.h>
#include <embb/base/perf/internal/env.h>
#include <embb/base/perf/internal/timestamp.h>

#include <papi.h>

#include <stdint.h>
#include <iostream>
#include <stdexcept>

namespace embb {
namespace base {
namespace perf {
namespace internal {
namespace timer {

/**
 * @brief Timestamp counter (RDTSC or PMC) for POSIX platforms.
 */
template<TimeMeasure::MeasureMode TTimer>
class TimestampPAPI : public Timestamp
{
  
private: 

  Timestamp::counter_t value; 
  
  static int timer_mode; 

public: 

  static Timestamp::counter_t frequencyScaling; 

public: 

  /** 
   * @brief Initializes the PAPI library. 
   */
  static void Calibrate(unsigned int arg = 0) { 
    timer_mode = arg; 
    int retval = PAPI_library_init(PAPI_VER_CURRENT); 
    if (retval != PAPI_VER_CURRENT && retval > 0) {
      throw ::std::runtime_error("PAPI version mismatch");
    }
    else if (retval < 0) { 
      throw ::std::runtime_error("PAPI init failed");
    }
  } 

public:

  TimestampPAPI() {
    if (TTimer == TimeMeasure::Clock) {
      if (timer_mode == 0) { 
        value = static_cast<counter_t>(PAPI_get_real_usec());
      }
      else {
        value = static_cast<counter_t>(PAPI_get_virt_usec());
      }
    }
    else if (TTimer == TimeMeasure::Counter) {
      if (timer_mode == 0) { 
        value = static_cast<counter_t>(PAPI_get_real_cyc());
      }
      else {
        value = static_cast<counter_t>(PAPI_get_virt_cyc());
      }
    }
  }

  inline TimestampPAPI(const TimestampPAPI & other)
    : value(other.value)
  { }

  inline TimestampPAPI(const counter_t & counterValue)
    : value(counterValue)
  { }

  inline TimestampPAPI & operator=(const TimestampPAPI rhs) {
    if (this != &rhs) {
      value = rhs.value; 
    }
    return *this; 
  }

  inline const counter_t & Value() const {
    return value;
  }

  inline static double FrequencyScaling() {
    if (TTimer == TimeMeasure::Counter) { 
      return 996.0f; // clock speed on Wandboard
    }
    return 1.0f; 
  }

  inline static double FrequencyPrescale() {
    return 1.0f;
  }

  inline static const char * TimerName() {
    return "PAPI"; 
  }

};

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#include <embb/base/perf/internal/timestamp_papi-inl.h>

#endif // EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_PAPI_H_

