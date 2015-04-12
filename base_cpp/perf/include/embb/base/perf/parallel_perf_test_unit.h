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

#ifndef EMBB_BASE_PERF_PARALLEL_PERF_TEST_UNIT_H_
#define EMBB_BASE_PERF_PARALLEL_PERF_TEST_UNIT_H_

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
template<typename ParallelF>
class ParallelPerfTestUnit : public partest::TestUnit {
 public:
  /**
   * Constructs PerfTestUnit and sets up partest::TestUnit with Functor \c F.
   */
   explicit ParallelPerfTestUnit(const CallArgs & params) :
    partest::TestUnit("ParallelPerfTestUnit"),
    params_(params) {
    func_ = new ParallelF(params_);
    Add(&ParallelPerfTestUnit<ParallelF>::Run, this);
  }

  /**
   * Destructs ParallelPerfTestUnit
   */
   ~ParallelPerfTestUnit() {
    delete func_;
  }

  /**
   * Durations of single runs of this unit in microseconds.
   * \return Vector of durations of single runs of this unit
   *         ordered by number of threads, in microseconds.
   */
  const std::vector< std::pair<unsigned int, double> > & GetDurations() const {
    return durations_;
  }

 private:
   /**
    * Run performance test
    */
  void Run() {
    for (unsigned int num_threads = 1; num_threads <= params_.MaxThreads();) {
      func_->Pre();
      Tic();
      func_->Run(num_threads);
      Toc(num_threads);
      func_->Post();
      if (num_threads < 4) {
        num_threads++;
      } else {
        num_threads += 4;
      }
    }
  }
  
  /**
   * Sets up EMBB and starts timer.
   */
  void Tic() {
    // Set number of available threads to given limit:
    embb_internal_thread_index_reset();
    // Configure cores to be used by EMBB:
    embb::base::CoreSet cores(false);
    for (unsigned int coreId = 0;
         coreId < params_.MaxThreads();
         ++coreId) {
      cores.Add(coreId);
    }
    embb::tasks::Node::Initialize(
      THIS_DOMAIN_ID, THIS_NODE_ID,
      cores,
      MTAPI_NODE_MAX_TASKS_DEFAULT,
      MTAPI_NODE_MAX_GROUPS_DEFAULT,
      MTAPI_NODE_MAX_QUEUES_DEFAULT,
      MTAPI_NODE_QUEUE_LIMIT_DEFAULT,
      MTAPI_NODE_MAX_PRIORITIES_DEFAULT);
    // start timer
    timer_ = Timer();
  }

  /**
   * Stops timer and resets EMBB
   */
  void Toc(unsigned int num_threads) {
    // stop timer
    durations_.push_back(
      std::make_pair(num_threads, timer_.Elapsed()));
    embb::tasks::Node::Finalize();    
  }

  const CallArgs params_;
  std::vector< std::pair<unsigned int, double> > durations_;
  Timer timer_;
  ParallelF * func_;

  // prohibit copy and assignment
  ParallelPerfTestUnit(const ParallelPerfTestUnit &other);
  ParallelPerfTestUnit& operator=(const ParallelPerfTestUnit & other);
};

}  // perf
}  // base
}  // embb

#endif  // EMBB_BASE_PERF_PARALLEL_PERF_TEST_UNIT_H_
