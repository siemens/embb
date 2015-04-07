#ifndef _EMBB_BASE_CPP_PERF_DURATION_H
#define _EMBB_BASE_CPP_PERF_DURATION_H

#include <embb/base/perf/timer.h>

namespace embb {
namespace base {
namespace perf {

struct Duration {
  Timer::timestamp_t Start;
  Timer::timestamp_t End;
};

} // namespace perf
} // namespace base
} // namespace embb

#endif // _EMBB_BASE_CPP_PERF_DURATION_H
