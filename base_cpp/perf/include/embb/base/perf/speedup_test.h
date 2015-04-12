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

#ifndef EMBB_BASE_PERF_SPEEDUP_TEST_H_
#define EMBB_BASE_PERF_SPEEDUP_TEST_H_

#include <vector>
#include <iomanip>
#include <partest/partest.h>
#include <embb/base/perf/timer.h>
#include <embb/base/perf/call_args.h>
#include <embb/base/perf/parallel_perf_test_unit.h>
#include <embb/base/perf/serial_perf_test_unit.h>

namespace embb {
namespace base {
namespace perf {

/**
 * Speedup Test
 *
 * Runs a non-copyable Functor \c iteration_count times with 1...\c thread_count
 * working threads (log2 steps) and reports the duration.
 *
 * Executes another Functor without EMBB. This can be used to compare a serial
 * version of an algorithm with its parallel counterpart.
 *
 * \notthreadsafe
 * \ingroup CPP_BASE_PERF
 */
template<typename SerialF, typename ParallelF>
class SpeedupTest : public partest::TestCase {
 public:
  /**
   * Constructs SpeedupTest and creates test units.
   */
  explicit SpeedupTest(const embb::base::perf::CallArgs & params)
  : partest::TestCase(), params_(params) {
    // create unit for serial version
    ser_unit_ = &CreateUnit< SerialPerfTestUnit<SerialF>, CallArgs >(params_);
    // create unit for parallel version
    par_unit_ = &CreateUnit< ParallelPerfTestUnit<ParallelF>, CallArgs >(params_);
  }

  /**
   * Destructs SpeedupTest.
   */
  ~SpeedupTest() {
  }

  /**
   * Prints the durations of all units in comma separated format.
   */
  void PrintReport(std::ostream & ostr) {
    double base_serial_duration = ser_unit_->GetDuration();
    // print sample row for sequential run (degree 0):
    ostr << "0," 
         << std::fixed << std::setprecision(2) 
         << base_serial_duration << ","
         << std::fixed << 1.0 << ","
         << std::fixed << 1.0
         << std::endl;
    // print sample rows for parallel runs (degree > 0):
    std::vector < std::pair< unsigned int, double > > durations =
      par_unit_->GetDurations();
    double base_parallel_duration = durations[0].second;
    for (unsigned int i = 0; i < durations.size(); ++i) {
      ostr << std::fixed << durations[i].first
           << "," 
           << std::fixed << std::setprecision(2) 
           << durations[i].second
           << ","
           << std::fixed << base_serial_duration / durations[i].second
           << ","
           << std::fixed << base_parallel_duration / durations[i].second
           << std::endl;
    }
  }

 private:
  const CallArgs & params_;
  ParallelPerfTestUnit<ParallelF> * par_unit_;
  SerialPerfTestUnit<SerialF> * ser_unit_;

  /* prohibit copy and assignment */
  SpeedupTest(const SpeedupTest &other);
  SpeedupTest& operator=(const SpeedupTest &other);
};

}  // perf
}  // base
}  // embb

#endif /* EMBB_BASE_PERF_SPEEDUP_TEST_H_ */
