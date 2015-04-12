
#include <embb/base/perf/call_args.h>
#include <embb/base/core_set.h>
#include <embb/base/perf/timer.h>
#include <string>
#include <ostream>
#include <cstdlib>

namespace embb {
namespace base {
namespace perf {

void CallArgs::Parse(int argc, char * argv[]) {
  // Set config from command line arguments:
  for (int paramIndex = 1; paramIndex < argc; paramIndex += 2) {
    // Max. number of threads to resolve speedup:
    if (std::string(argv[paramIndex]) == "-t") {
      size_t threads_param = static_cast<size_t>(
        atoi(argv[paramIndex + 1]));
      if (threads_param > 0 && threads_param < max_threads) {
        max_threads = threads_param;
      }
    }
    // Test vector size:
    if (std::string(argv[paramIndex]) == "-n") {
      size_t vsize_param = static_cast<size_t>(
        atoi(argv[paramIndex + 1]));
      if (vsize_param > 0) {
        vector_size = vsize_param;
      }
    }
    // Performance counter scaling:
    if (std::string(argv[paramIndex]) == "-f") {
      unsigned int scale_param = static_cast<unsigned int>(
        atoi(argv[paramIndex + 1]));
      if (scale_param > 0) {
        counter_scale = scale_param;
      }
    }
    // Stress type:
    if (std::string(argv[paramIndex]) == "-s") {
      stress_type = UNDEFINED_STRESS_TYPE;
      ::std::string type = argv[paramIndex + 1];
      if (type == "cpu") {
        stress_type = CPU_STRESS;
      }
      else if (type == "ram") {
        stress_type = RAM_STRESS;
      }
    }
    // Test load factor:
    if (std::string(argv[paramIndex]) == "-l") {
      load_factor = static_cast<size_t>(
        atoi(argv[paramIndex + 1]));
    }
    if (stress_type == UNDEFINED_STRESS_TYPE) {
      throw ::std::runtime_error(
        "Invalid setting for stress test type (-s ram|cpu)");
    }
  }
  // Calibrate performance time sampling:
  embb::base::perf::Timer::Calibrate(
    embb::base::perf::TimeMeasure::Counter,
    CounterScale());
}

void CallArgs::Print(std::ostream & os) {
  os << "Max. threads:    (-t) " << MaxThreads() << std::endl
     << "Vector size:     (-n) " << VectorSize() << std::endl
     << "Load factor:     (-l) " << LoadFactor() << std::endl
     << "Stress mode:     (-s) " << StressModeName() << std::endl
     << "Time sampling:   (-f) " << embb::base::perf::Timer::TimerName() 
     << std::endl;
}

} // namespace perf
} // namespace base
} // namespace embb
