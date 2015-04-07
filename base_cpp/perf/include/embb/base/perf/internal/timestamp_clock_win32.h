#ifndef EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_CLOCK_WIN32_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_CLOCK_WIN32_H_

#include <embb/base/perf/timer.h>

#if defined(EMBB_BASE_CPP_PERF_TIMER_WIN32)  

#if (defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8)
#define EMBB_BASE_CPP_PERF_TIMER_WIN32_SYSTEM_TIME_PRECISE
#endif

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
class TimestampClockWin32 : public Timestamp
{

private:

  typedef TimestampClockWin32 self_t;

private:

  counter_t value;

public:

  inline static void Calibrate(unsigned int = 0) { }

public:

  inline TimestampClockWin32() {
    FILETIME  tm;
#if defined(EMBB_BASE_CPP_PERF_TIMER_WIN32_SYSTEM_TIME_PRECISE)
    // Windows 8, Windows Server 2012 and later
    GetSystemTimeAsFileTime(&tm);
 // Should work according to docs, but doesn't: 
 // GetSystemTimePreciseAsFileTime(&tm);
#else
    // Windows 2000 and later
    GetSystemTimeAsFileTime(&tm);
#endif
    value = (static_cast<counter_t>(tm.dwHighDateTime) << 32) |
             static_cast<counter_t>(tm.dwLowDateTime);
  }

  inline TimestampClockWin32(const self_t & other)
    : value(other.value)
  { }

  inline TimestampClockWin32(const counter_t & counterValue)
    : value(counterValue)
  { }

  inline self_t & operator=(const self_t & rhs) {
    if (this != &rhs) {
      value = rhs.value;
    }
    return *this;
  }

  inline const counter_t & Value() const {
    return value;
  }

  inline static double FrequencyScaling() {
    return 1.0f;
  }

  inline static double FrequencyPrescale() {
    // Adjust from millisecond to microsecond scale: 
    return 0.1;
  }

  inline static const char * TimerName() {
#if defined(EMBB_BASE_CPP_PERF_TIMER_WIN32_SYSTEM_TIME_PRECISE)
    return "Win32:GetSystemTimePrecise";
#else
    return "Win32:GetSystemTime";
#endif
  }

};

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#endif // defined(EMBB_BASE_CPP_PERF_TIMER_WIN32)  

#endif // EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_CLOCK_WIN32_H_
