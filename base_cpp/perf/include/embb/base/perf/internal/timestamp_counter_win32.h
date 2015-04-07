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

#ifndef EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_COUNTER_WIN32_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_COUNTER_WIN32_H_

#if defined(EMBB_BASE_CPP_PERF_TIMER_WIN32)  

#include <embb/base/perf/timer.h>

// Prevent definition of Windows-specific macros 'min' and 'max': 
#define NOMINMAX 
#include <Windows.h>

namespace embb {
namespace base {
namespace perf {
namespace internal {
namespace timer {

/**
 * @brief Timestamp counter (TSC) for Windows platforms.
 */
class TimestampCounterWin32 : public Timestamp
{

private:

  typedef TimestampCounterWin32 self_t; 

  static counter_t frequencyScaling;

private:

  Timestamp::counter_t value;

public: 

  inline static void Calibrate(unsigned int = 0) {
    // Resolves ticks per second: 
    LARGE_INTEGER freq_li; 
    LONGLONG freq_value; 
    
    QueryPerformanceFrequency(&freq_li);
    freq_value = freq_li.QuadPart;

    self_t::frequencyScaling = static_cast<counter_t>(freq_value);
  }

public:

  inline TimestampCounterWin32() {
    LARGE_INTEGER count_now; 
    LONGLONG count_now_value; 
    QueryPerformanceCounter(&count_now);    
    count_now_value = count_now.QuadPart;
    value = static_cast<counter_t>(count_now_value);
  }

  inline TimestampCounterWin32(const self_t & other)
    : value(other.value)
  { }

  inline TimestampCounterWin32(const Timestamp::counter_t & counterValue)
    : value(counterValue)
  { }

  inline self_t & operator=(const self_t & rhs) {
    if (this != &rhs) {
      value = rhs.value; 
    }
    return *this; 
  }

  inline const Timestamp::counter_t & Value() const {
    return value;
  }

  inline static double FrequencyScaling() {
    if (self_t::frequencyScaling == 0) {
      Calibrate();
    }
    return static_cast<double>(self_t::frequencyScaling);
  }

  inline static double FrequencyPrescale() {
    // Adjusting frequency from seconds to microseconds scale:
    return 1000000.0f; 
  }

  inline static const char * TimerName() {
    return "Win32:QueryPerformanceCounter";
  }

};

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#endif // EMBB_BASE_CPP_BENCHMARK_INTERNAL_TIMESTAMP_WIN32_H_

#endif // _WIN32
