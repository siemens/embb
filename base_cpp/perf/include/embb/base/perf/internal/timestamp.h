#ifndef EMBB_BASE_CPP_PERF_INTERNAL_TIMESTAMP_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_TIMESTAMP_H_

#include <embb/base/perf/internal/env.h>
#include <limits.h>

namespace embb {
namespace base {
namespace perf {

class Timestamp {
 public:
#if defined(EMBB_BASE_CPP_PERF__ARCH_X64)
  typedef unsigned long long counter_t;
#else
  typedef unsigned long long counter_t;
#endif

 public:
  virtual const counter_t & Value() const = 0;

  static double FrequencyScaling();
  static double FrequencyPrescale();
  static const char * VariantName();
  inline static counter_t TimestampInfinity() {
    return LLONG_MAX;
  }
  inline static counter_t TimestampNegInfinity() {
    return 0;
  }
};

} // namespace perf
} // namespace base
} // namespace embb

#endif //  EMBB_BASE_CPP_PERF_INTERNAL_TIMESTAMP_H_

