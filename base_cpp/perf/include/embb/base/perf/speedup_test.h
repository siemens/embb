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
#include <embb/base/perf/perf_test_unit.h>

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
template<typename ParallelF, typename SerialF>
class SpeedupTest : public partest::TestCase {
 public:
  /**
   * Constructs SpeedupTest and creates test units.
   */
  explicit SpeedupTest(
      size_t max_thread_count = partest::TestSuite::GetDefaultNumThreads(),
      size_t iteration_count  = partest::TestSuite::GetDefaultNumIterations()) :
      partest::TestCase() {
    /* maximum one thread per available core */
    size_t threads = std::min<size_t>(
        max_thread_count,
        embb::base::CoreSet::CountAvailable());

    std::cout << "Test configuration ------------------------------------" << std::endl;
    std::cout << "  Num threads: " << threads << std::endl;
    std::cout << "  Iterations:  " << iteration_count << std::endl;

    /* create unit for serial version */
    ser_unit_ = &CreateUnit< PerfTestUnit<SerialF> >(0, iteration_count);
    /* create log2(threads)+1 units for parallel version */
    for (size_t i = 1; i <= threads; i = i * 2) {
      par_units_.push_back(
        &CreateUnit< PerfTestUnit<ParallelF> >(i, iteration_count));
    }
  }

  /**
   * Destructs SpeedupTest.
   */
  ~SpeedupTest() {
  }

  /**
   * Prints the durations of all units in comma separated format.
   */
  void PrintReport(std::ostream &ostr) {
    /* print sample row for sequential run (degree 0): */
    ostr << "0," 
         << std::fixed << std::setprecision(2) 
         << ser_unit_->GetDuration() << std::endl;
    /* print sample rows for parallel runs (degree > 0): */
    for (int i = 0; i < par_units_.size(); ++i) {
      ostr << std::fixed << par_units_[i]->GetThreadCount()
           << "," 
           << std::fixed << std::setprecision(2) 
           << par_units_[i]->GetDuration() 
           << std::endl;
    }
  }

 private:
  std::vector<PerfTestUnit<ParallelF> *> par_units_;
  PerfTestUnit<SerialF> *ser_unit_;

  /* prohibit copy and assignment */
  SpeedupTest(const SpeedupTest &other);
  SpeedupTest& operator=(const SpeedupTest &other);
};

} /* perf */
} /* base */
} /* embb */

#endif /* EMBB_BASE_PERF_SPEEDUP_TEST_H_ */
