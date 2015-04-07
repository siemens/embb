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

#ifndef EMBB_BASE_CPP_PERF_TIMER_H_
#define EMBB_BASE_CPP_PERF_TIMER_H_

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS
// #define EMBB_BASE_CPP_PERF_TIMER_PAPI
#endif

#include <embb/base/perf/time_measure.h>
#include <embb/base/perf/internal/env.h>
#include <embb/base/perf/internal/timestamp.h>

#include <embb/base/c/internal/config.h>

#include <climits>

#if defined(EMBB_BASE_CPP_PERF_TIMER_PAPI)
#  include <embb/base/perf/internal/timestamp_papi.h>
#endif
#if defined(EMBB_PLATFORM_THREADING_WINTHREADS)
#  include <embb/base/perf/internal/timestamp_counter_win32.h>
#  include <embb/base/perf/internal/timestamp_clock_win32.h>
#elif defined(EMBB_PLATFORM_THREADING_POSIXTHREADS)
#  include <embb/base/perf/internal/timestamp_counter_posix.h>
#  include <embb/base/perf/internal/timestamp_clock_posix.h>
#endif

namespace embb {
namespace base {
namespace perf {

class Timer {
 public: 
  typedef Timestamp::counter_t timestamp_t;

 private:
  typedef Timer self_t;
  static embb::base::perf::TimeMeasure::MeasureMode Type;
  timestamp_t timestampStart;

 private: 
#if defined(EMBB_BASE_CPP_PERF_TIMER_PAPI)
// PAPI support, use optimized performance measurements from PAPI 
  typedef embb::base::perf::internal::timer::TimestampPAPI<TimeMeasure::Clock>
    TimestampClockBased;
  typedef embb::base::perf::internal::timer::TimestampPAPI<TimeMeasure::Counter>
    TimestampCounterBased;
#else  // No PAPI

# if defined(EMBB_BASE_CPP_PERF_TIMER_WIN32)
  // Windows: 
  typedef embb::base::perf::internal::timer::TimestampCounterWin32 
    TimestampCounterBased;
  typedef embb::base::perf::internal::timer::TimestampClockWin32 
    TimestampClockBased;
# else 
  // POSIX: 
  typedef embb::base::perf::internal::timer::TimestampCounterPosix 
    TimestampCounterBased;
  typedef embb::base::perf::internal::timer::TimestampClockPosix 
    TimestampClockBased;
# endif  // WIN32 / POSIX

#endif  // No PAPI

 public:
  inline Timer() {
    timestampStart = Timer::Now(); 
  }

  inline Timer(const self_t & other) : timestampStart(other.timestampStart)
  { }

  inline Timer & operator=(const self_t & other) {
    if (this != &other) {
      timestampStart = other.timestampStart;
    }
    return *this;
  }

  /**
   * @brief Microeconds elapsed since instantiation of this Timer object.
   */
  inline double Elapsed() const {
    timestamp_t now;

    if (Timer::Type == TimeMeasure::Counter) {
      TimestampCounterBased timestamp;
      now = timestamp.Value();
      return (static_cast<double>(now - timestampStart) *
        static_cast<double>(TimestampCounterBased::FrequencyPrescale())) /
        static_cast<double>(TimestampCounterBased::FrequencyScaling());
    }
    if (Timer::Type == TimeMeasure::Clock) {
      TimestampClockBased timestamp;
      now = timestamp.Value();
      return (static_cast<double>(now - timestampStart) *
        static_cast<double>(TimestampClockBased::FrequencyPrescale())) /
        static_cast<double>(TimestampClockBased::FrequencyScaling());
    }
    return 0.0f; 
  }

  /**
   * Returns timestamp from instantiation of this Timer.
   */
  inline const timestamp_t & Start() const {
    return timestampStart;
  }

  /**
   * Microseconds elapsed since given timestamp.
   */
  inline static double ElapsedSince(timestamp_t timestamp) {
    if (Timer::Type == TimeMeasure::Counter) {
      TimestampCounterBased now;
      return (static_cast<double>(now.Value() - timestamp) *
        static_cast<double>(TimestampCounterBased::FrequencyPrescale())) /
        static_cast<double>(TimestampCounterBased::FrequencyScaling());
    }
    if (Timer::Type == TimeMeasure::Clock) {
      TimestampClockBased now;
      return (static_cast<double>(now.Value() - timestamp) *
        static_cast<double>(TimestampClockBased::FrequencyPrescale())) /
        static_cast<double>(TimestampClockBased::FrequencyScaling());
    }
    return 0.0f; 
  }

  /**
   * Produces current timestamp.
   */
  inline static timestamp_t Now() {
    if (Timer::Type == TimeMeasure::Counter) {
      TimestampCounterBased timestamp;
      return timestamp.Value();
    }
    if (Timer::Type == TimeMeasure::Clock) {
      TimestampClockBased timestamp;
      return timestamp.Value();
    }
    return 0; 
  }

  /**
   * Convert interval of two timestamp values to mircoseconds.
   */
  inline static double FromInterval(
    const timestamp_t & start,
    const timestamp_t & end) 
  {
    if (Timer::Type == TimeMeasure::Counter) {
      return (static_cast<double>(end - start) *
        static_cast<double>(TimestampCounterBased::FrequencyPrescale())) /
        static_cast<double>(TimestampCounterBased::FrequencyScaling());
    }
    if (Timer::Type == TimeMeasure::Clock) {
      return (static_cast<double>(end - start) *
        static_cast<double>(TimestampClockBased::FrequencyPrescale())) /
        static_cast<double>(TimestampClockBased::FrequencyScaling());
    }
    return -1.0f; 
  }

  /**
   * Convert interval of two timestamp values to mircoseconds.
   */
  inline static double FromInterval(
    const double & start,
    const double & end)
  {
    if (Timer::Type == TimeMeasure::Counter) {
      return ((end - start) *
        TimestampCounterBased::FrequencyPrescale() /
        TimestampCounterBased::FrequencyScaling());
    }
    if (Timer::Type == TimeMeasure::Clock) {
      return ((end - start) *
        TimestampClockBased::FrequencyPrescale() /
        TimestampClockBased::FrequencyScaling());
    }
    return -1.0f; 
  }

  inline static void Calibrate(
      TimeMeasure::MeasureMode mode, 
      unsigned int freq = 0) {
    Timer::Type = mode; 
    if (Timer::Type == TimeMeasure::Counter) {
      Timer::TimestampCounterBased::Calibrate(freq);
    }
    else if (Timer::Type == TimeMeasure::Clock) {
      Timer::TimestampClockBased::Calibrate(freq);
    }
  }

  inline static const char * TimerName() {
    if (Timer::Type == TimeMeasure::Counter) {
      return TimestampCounterBased::TimerName();
    }
    if (Timer::Type == TimeMeasure::Clock) {
      return TimestampClockBased::TimerName();
    }
    return "Undefined";
  }

  inline static Timestamp::counter_t TimestampInfinity() {
    if (Timer::Type == TimeMeasure::Counter) {
      return TimestampCounterBased::TimestampInfinity();
    }
    if (Timer::Type == TimeMeasure::Clock) {
      return TimestampClockBased::TimestampInfinity();
    }
    return LLONG_MAX; 
  }

  inline static Timestamp::counter_t TimestampNegInfinity() {
    if (Timer::Type == TimeMeasure::Counter) {
      return Timer::TimestampCounterBased::TimestampNegInfinity();
    }
    if (Timer::Type == TimeMeasure::Clock) {
      return Timer::TimestampClockBased::TimestampNegInfinity();
    }
    return 0; 
  }

  inline static double FrequencyScaling() {
    if (Timer::Type == TimeMeasure::Counter) {
      return Timer::TimestampCounterBased::FrequencyScaling(); 
    }
    if (Timer::Type == TimeMeasure::Clock) {
      return Timer::TimestampClockBased::FrequencyScaling(); 
    }
    return 1.0f; 
  }
};

} // namespace perf
} // namespace base
} // namespace embb

#endif // EMBB_BASE_CPP_PERF_TIMER_H_
