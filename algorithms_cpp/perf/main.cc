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

#include <partest/partest.h>

#include <for_each_perf.h>
#include <reduce_perf.h>
#include <scan_perf.h>
#include <count_perf.h>
#include <quick_sort_perf.h>
#include <merge_sort_perf.h>

#include <embb/base/perf/perf.h>
#include <embb/base/perf/speedup_test.h>

using embb::algorithms::perf::SerialForEach;
using embb::algorithms::perf::SerialReduce;
using embb::algorithms::perf::SerialScan;
using embb::algorithms::perf::SerialCount;
using embb::algorithms::perf::SerialScan;
using embb::algorithms::perf::SerialMergeSort;
using embb::algorithms::perf::SerialQuickSort;
using embb::algorithms::perf::ParallelForEach;
using embb::algorithms::perf::ParallelReduce;
using embb::algorithms::perf::ParallelScan;
using embb::algorithms::perf::ParallelCount;
using embb::algorithms::perf::ParallelMergeSort;
using embb::algorithms::perf::ParallelQuickSort;
using embb::base::perf::Timer;
using embb::base::perf::CallArgs;
using embb::base::perf::SpeedupTest;

#define COMMA ,

PT_PERF_MAIN("Algorithms") {
  PT_PERF_RUN(SpeedupTest< SerialForEach<float> COMMA ParallelForEach<float> >);
  PT_PERF_RUN(SpeedupTest< SerialReduce<float> COMMA ParallelReduce<float> >);
  PT_PERF_RUN(SpeedupTest< SerialScan<float> COMMA ParallelScan<float> >);
  PT_PERF_RUN(SpeedupTest< SerialCount<float> COMMA ParallelCount<float> >);
  PT_PERF_RUN(SpeedupTest< SerialMergeSort<float> COMMA ParallelMergeSort<float> >);
  PT_PERF_RUN(SpeedupTest< SerialQuickSort<float> COMMA ParallelQuickSort<float> >);
}
