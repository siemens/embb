
#include <embb/base/perf/timer.h>

#if defined(EMBB_BASE_CPP_PERF_TIMER_POSIX) || \
    defined(EMBB_BASE_CPP_PERF_TIMER_UX)

#include <embb/base/perf/internal/timestamp_counter_posix.h>

namespace embb {
namespace base {
namespace perf {
namespace internal {
namespace timer {

void TimestampCounterPosix::Calibrate(unsigned int freq) {
  frequencyScaling = freq == 0
                     ? 1900.0f 
                     : static_cast<double>(freq); 
}

Timestamp::counter_t TimestampCounterPosix::frequencyScaling = 1; 

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#endif  // EMBB_BASE_CPP_PERF_TIMER_POSIX || EMBB_BASE_CPP_PERF_TIMER_UX

