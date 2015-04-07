
#include <embb/base/perf/timer.h>

#if defined(EMBB_BASE_CPP_PERF_TIMER_WIN32)

#include <embb/base/perf/internal/timestamp_counter_win32.h>

namespace embb {
namespace base {
namespace perf {
namespace internal {
namespace timer {

TimestampCounterWin32::counter_t TimestampCounterWin32::frequencyScaling = 0;

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#endif // EMBB_BASE_CPP_PERF_TIMER_WIN32
