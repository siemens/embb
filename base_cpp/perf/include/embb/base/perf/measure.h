#ifndef _EMBB_BASE_CPP_PERF_MEASURE_H
#define _EMBB_BASE_CPP_PERF_MEASURE_H

#include <embb/base/perf/duration.h>
#include <embb/base/perf/performance_metrics.h>

namespace embb {
namespace base {
namespace perf {

struct Measure { 
  Duration duration; 
  PerformanceMetrics metrics; 
}; 

} // namespace perf
} // namespace base
} // namespace embb

#endif // _EMBB_BASE_CPP_PERF_DURATION_H
