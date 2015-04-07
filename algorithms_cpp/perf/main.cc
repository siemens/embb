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

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <partest/partest.h>

#include <embb/base/core_set.h>
#include <embb/base/thread.h>
#include <embb/base/c/log.h>
#include <embb/base/c/internal/thread_index.h>
#include <embb/tasks/tasks.h>

#include <embb/base/perf/call_args.h>
#include <embb/base/perf/timer.h>

#include <for_each_perf.h>
#include <reduce_perf.h>
#include <scan_perf.h>
#include <count_perf.h>
#include <quick_sort_perf.h>
#include <merge_sort_perf.h>

using namespace embb::algorithms::perf;
using embb::base::perf::Timer;
using embb::base::perf::CallArgs;

void ReportResult(
  const std::string & name,
  unsigned int threads,
  const CallArgs & args,
  double elapsed,
  double speedup) {
  std::cout
    << args.VectorSize() << ","
    << args.ElementTypeName() << ","
    << args.LoadFactor() << ","
    << args.StressModeName() << ","
    << threads << ","
    << std::fixed << elapsed << ","
    << std::setprecision(3) << speedup
    << std::endl;
  std::ofstream file;
  std::string filename = "performance_tests_result.csv";
  file.open(filename.c_str(), ::std::ofstream::out | ::std::ofstream::app);
  file
    << name << ","
    << args.VectorSize() << ","
    << args.ElementTypeName() << ","
    << args.LoadFactor() << ","
    << args.StressModeName() << ","
    << threads << ","
    << std::fixed << elapsed << ","
    << std::setprecision(3) << speedup
    << std::endl;
}

template< typename TestSerial, typename TestParallel >
void RunPerformanceTest(
  const embb::base::perf::CallArgs & args,
  const std::string & name) {
  std::cout << "--- Running " << name << std::endl;
  // Initialize new test instances:
  TestParallel testParallel(args);
  // Parallel runs:  
  unsigned int threads = 1;
  // Base value to compute speedup; parallel execution
  // with 1 thread or serial execution.
  double baseDuration = 0;

  // Whether to use serial or parallel exeuction using 1
  // thread for speedup reference:
  if (args.ParallelBaseReference() == 0) {
    TestSerial testSerial(args);
    // Serial run:
    Timer t;
    testSerial.Run();
    double elapsed = t.Elapsed();
    ReportResult(
      name,
      0,
      args,
      elapsed,
      1.0);
    baseDuration = elapsed;
  }
  threads += args.ParallelBaseReference();
  // Run executions with incrementing number of threads:
  embb_internal_thread_index_set_max(args.MaxThreads());
  while(threads <= args.MaxThreads()) {
    // Set number of available threads to given limit: 
    // embb::base::Thread::SetThreadsMaxCount(threads);
    embb_internal_thread_index_reset();
    // Configure cores to be used by EMBB:
    embb::base::CoreSet cores(false);
    for (unsigned int coreId = 0; coreId < threads; ++coreId) {
      cores.Add(coreId);
    }
    embb::tasks::Node::Initialize(
      1, 1, cores,
      MTAPI_NODE_MAX_TASKS_DEFAULT * 8,
      MTAPI_NODE_MAX_GROUPS_DEFAULT * 8,
      MTAPI_NODE_MAX_QUEUES_DEFAULT * 8,
      MTAPI_NODE_QUEUE_LIMIT_DEFAULT * 8,
      MTAPI_NODE_MAX_PRIORITIES_DEFAULT);
    // Test setup:
    testParallel.Pre();
    // Initialize timer:
    Timer t;
    // Run the test body:
    testParallel.Run(threads);
    // Report duration:
    double elapsed = t.Elapsed();
    if (threads == 1) {
      baseDuration = elapsed;
    }
    ReportResult(
      name,
      threads,
      args,
      elapsed,
      static_cast<double>(baseDuration) / static_cast<double>(elapsed));
    // Test teardown:
    testParallel.Post();
    if (threads < 4) {
      ++threads;
    } else {
      threads += 4;
    }
    embb::tasks::Node::Finalize();
  }
}

template<typename EType>
void RunPerformanceTests(
  const CallArgs & args) {
  RunPerformanceTest<SerialForEach<EType>, ParallelForEach<EType> >(args, "ForEach");
  RunPerformanceTest<SerialReduce<EType>, ParallelReduce<EType> >(args, "Reduce");
  RunPerformanceTest<SerialScan<EType>, ParallelScan<EType> >(args, "Scan");
  RunPerformanceTest<SerialCount<EType>, ParallelCount<EType> >(args, "Count");
  RunPerformanceTest<SerialQuickSort<EType>, ParallelQuickSort<EType> >(args, "Quicksort");
  RunPerformanceTest<SerialMergeSort<EType>, ParallelMergeSort<EType> >(args, "Mergesort");
}

int main(int argc, char * argv[]) {
  // Parse command line arguments:
  embb::base::perf::CallArgs args;
  try {
    args.Parse(argc, argv);
  } catch (::std::runtime_error & re) {
    ::std::cerr << re.what() << ::std::endl;
  }
  if (args.ParallelBaseReference() == 1) {
    embb_log_set_log_level(EMBB_LOG_LEVEL_TRACE);
  }
  // Print test settings:
  args.Print(::std::cout);
  // Run tests:
  switch (args.ElementType()) {
  case CallArgs::FLOAT:
    RunPerformanceTests<float>(args);
    break;
  case CallArgs::DOUBLE:
    RunPerformanceTests<double>(args);
    break;
  case CallArgs::UNDEFINED_SCALAR_TYPE:
    break;
  }
  return 0;
}
