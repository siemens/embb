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

#ifndef EMBB_BASE_PERF_SERIAL_PERF_TEST_UNIT_H_
#define EMBB_BASE_PERF_SERIAL_PERF_TEST_UNIT_H_

#include <cmath>
#include <vector>
#include <partest/partest.h>
#include <partest/test_unit.h>
#include <embb/base/perf/timer.h>
#include <embb/base/perf/call_args.h>
#include <embb/tasks/tasks.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/internal/thread_index.h>

#define THIS_DOMAIN_ID 1
#define THIS_NODE_ID 1

namespace embb {
namespace base {
namespace perf {

/**
 * \defgroup CPP_BASE_PERF Performance Tests
 *
 * Performance Test Framework
 *
 * \ingroup CPP_BASE
 */
/**
 * Performance Test Unit
 *
 * Base unit of any test (Speedup Test, Performance Test, ...). Takes a
 * non-copyable Functor as template argument and executes it \c iteration_count
 * times on \c thread_count worker threads.
 *
 * If \c thread_count equals 0, EMBB is not initialized and the Functor is
 * executed without EMBB support.
 *
 * \notthreadsafe
 * \ingroup CPP_BASE_PERF
 */
template<typename SerialF>
class SerialPerfTestUnit : public partest::TestUnit {
 public:
  /**
   * Constructs PerfTestUnit and sets up partest::TestUnit with Functor \c F.
   */
   explicit SerialPerfTestUnit(const CallArgs & params) :
    partest::TestUnit("SerialPerfTestUnit"),
    params_(params),
    duration_(0) {
    func_ = new SerialF(params_);
    Add(&SerialPerfTestUnit<SerialF>::Run, this);
  }

  /**
   * Destructs SerialPerfTestUnit
   */
  ~SerialPerfTestUnit() {
    delete func_;
  }

  /**
   * Durations of single runs of this unit in microseconds.
   * \return Vector of durations of single runs of this unit
   *         ordered by number of threads, in microseconds.
   */
  double GetDuration() const {
    return duration_;
  }

 private:
  /**
   * Run performance test
   */
  void Run() {
    func_->Pre();
    Tic();
    func_->Run();
    Toc();
    func_->Post();       
  }
  
  /**
   * Sets up EMBB and starts timer.
   */
  void Tic() {    
    // start timer
    timer_ = Timer();
  }

  /**
   * Stops timer and resets EMBB
   */
  void Toc() {
    // stop timer
    duration_ = timer_.Elapsed();
  }

  const CallArgs params_;
  double duration_;
  Timer timer_;
  SerialF * func_;

  // prohibit copy and assignment
  SerialPerfTestUnit(const SerialPerfTestUnit & other);
  SerialPerfTestUnit& operator=(const SerialPerfTestUnit & other);
};

}  // perf
}  // base
}  // embb

#endif  // EMBB_BASE_PERF_SERIAL_PERF_TEST_UNIT_H_
