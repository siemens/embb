#ifndef EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_COUNTER_POSIX_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_COUNTER_POSIX_H_

#include <embb/base/perf/timer.h>
#include <embb/base/perf/internal/timestamp.h>

#include <stdint.h>
#include <iostream>

#include <embb/base/perf/internal/env.h>
#if defined(EMBB_BASE_CPP_PERF__ARCH_MIPS) || \
    defined(EMBB_BASE_CPP_PERF__ARCH_ARM)
#include <sys/time.h>
#endif // ARM <= V3 or MIPS

namespace embb {
namespace base {
namespace perf {
namespace internal {
namespace timer {

/**
 * @brief Timestamp counter (RDTSC or PMC) for POSIX platforms.
 */
class TimestampCounterPosix : public Timestamp
{
  
private: 

  Timestamp::counter_t value; 

public: 
  
  static Timestamp::counter_t frequencyScaling; 

  /**
   * @brief Serialized RDTSCP (x86, x64) or PMC,PMU (arm6+)
   * 
   * Prevents out-of-order execution to affect timestamps.  
   * 
   */  
  static inline uint64_t ArchCycleCount() {
#if defined(EMBB_BASE_CPP_PERF__ARCH_X64)
    uint64_t rax, rdx;
    uint32_t tsc_aux;
    __asm__ volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (tsc_aux) : : );
    return (rdx << 32) + rax;
#elif defined(EMBB_BASE_CPP_PERF__ARCH_I386)
    int64 ret;
    __asm__ volatile ("rdtsc" : "=A" (ret) );
    return ret;
#elif defined(EMBB_BASE_CPP_PERF__ARCH_ARMV6)
    uint32_t pmccntr;
    uint32_t pmuseren = 1;
    uint32_t pmcntenset;
    
    // Read the user mode perf monitor counter access permissions.
    __asm__ volatile ("mrc p15, 0, %0, c9, c14, 0" : "=r" (pmuseren));
    // Set permission flag: 
    pmuseren &= 0x01;  // Set E bit
    __asm__ volatile ("mcr p15, 0, %0, c9, c14, 0" : "=r" (pmuseren));
    if (pmuseren & 1) {  // Allows reading perfmon counters for user mode code.
      __asm__ volatile ("mrc p15, 0, %0, c9, c12, 1" : "=r" (pmcntenset));
      if (pmcntenset & 0x80000000ul) {  // Is it counting?
        __asm__ volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r" (pmccntr));
        // The counter is set up to count every 64th cycle
        return static_cast<int64_t>(pmccntr) * 64;  // Should optimize to << 6
      }
      else {
        return 1; 
      }
    }
    else {
      return 2; 
    }
#else // Fallback for architectures that do not provide any low-level counter: 
#pragma message "Fallback to generic performance counter implementation"
    struct timeval tv; 
    gettimeofday(&tv, NULL); 
    return static_cast<uint64_t>((tv.tv_sec + tv.tv_usec * 0.000001) * 
                                 FrequencyScaling());                                  
#endif
    // Undefined value if perfmon is unavailable: 
    return 0; 
  }

public: 

  /** 
   * @brief Calibrates counts per microscecond. 
   * 
   * Used for frequency scaling of RDSTD. 
   */
  static void Calibrate(unsigned int = 0); 

public:

  inline TimestampCounterPosix() {
    value = static_cast<counter_t>(ArchCycleCount());
  }

  inline TimestampCounterPosix(const TimestampCounterPosix & other)
    : value(other.value)
  { }

  inline TimestampCounterPosix(const counter_t & counterValue)
    : value(counterValue)
  { }

  inline TimestampCounterPosix & operator=(const TimestampCounterPosix rhs) {
    if (this != &rhs) {
      value = rhs.value; 
    }
    return *this; 
  }

  inline const counter_t & Value() const {
    return value;
  }

  inline static double FrequencyScaling() {
    return static_cast<double>(TimestampCounterPosix::frequencyScaling);
  }

  inline static double FrequencyPrescale() {
    return 1.0f;
  }

  inline static const char * TimerName() {
#if defined(EMBB_BASE_CPP_PERF__ARCH_X64)
    return "POSIX:X64:RDTSC"; 
#elif defined(EMBB_BASE_CPP_PERF__ARCH_I386)
    return "POSIX:X86:RDTSC"; 
#elif defined(EMBB_BASE_CPP_PERF__ARCH_ARMV6)
    return "POSIX:ARM:PMCNT"; 
#else 
    return "POSIX:GENERIC"; 
#endif
  }

};

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#endif // EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_COUNTER_POSIX_H_

