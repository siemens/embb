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

#ifndef EMBB_BASE_PERF_PERFORMANCE_TEST_H_
#define EMBB_BASE_PERF_PERFORMANCE_TEST_H_

#include <partest/partest.h>
#include <embb/base/perf/perf_test_unit.h>

namespace embb {
namespace base {
namespace perf {

/**
 * Performance Test
 *
 * Runs a non-copyable Functor \c iteration_count times with \c thread_count
 * working threads and reports the duration.
 *
 * \notthreadsafe
 * \ingroup CPP_BASE_PERF
 */
template<typename ParallelF>
class PerformanceTest : public partest::TestCase {
 public:
  /**
   * Constructs PerformanceTest.
   */
  explicit PerformanceTest(
      size_t thread_count = partest::TestSuite::GetDefaultNumThreads(),
      size_t iteration_count = partest::TestSuite::GetDefaultNumIterations()) :
      partest::TestCase() {
    /* maximum one thread per available core */
    size_t threads = std::min<size_t>(thread_count,
                                      embb::base::CoreSet::CountAvailable());

    unit = &CreateUnit< PerfTestUnit<ParallelF> >(threads, iteration_count);
  }

  /**
   * Destructs PerformanceTest.
   */
  ~PerformanceTest() {
    PrintReport(std::cout);
  }

  /**
   * Prints the durations of all units in comma separated format.
   */
  void PrintReport(std::ostream &ostr) const {
    /* print execution duration */
    ostr << "P" << unit->GetThreadCount << std::endl << unit->GetDuration()
         << std::endl;
  }

 private:
  PerfTestUnit<ParallelF> *unit;

  /* prohibit copy and assignment */
  PerformanceTest(const PerformanceTest &other);
  PerformanceTest& operator=(const PerformanceTest &other);
};

} /* perf */
} /* base */
} /* embb */

#endif /* EMBB_BASE_PERF_PERFORMANCE_TEST_H_ */
