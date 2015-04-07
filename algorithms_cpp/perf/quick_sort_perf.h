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

#ifndef EMBB_ALGORITHMS_PERF_QUICK_SORT_PERF_H_
#define EMBB_ALGORITHMS_PERF_QUICK_SORT_PERF_H_

#include <embb/base/perf/call_args.h>
#include <cmath>

namespace embb {
namespace algorithms {
namespace perf {

using embb::base::perf::CallArgs;

template<typename T>
class SerialQuickSort {
public:
  explicit SerialQuickSort(const embb::base::perf::CallArgs & args);
  ~SerialQuickSort();
  void Pre() { }
  void Run();
  void Post() { }
private:
  static int Greater(const void *a, const void *b);
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T * v;
  /* prohibit copy and assignment */
  SerialQuickSort(const SerialQuickSort & other);
  SerialQuickSort & operator=(const SerialQuickSort & other);
};

template<typename T>
class ParallelQuickSort {
public:
  explicit ParallelQuickSort(const embb::base::perf::CallArgs & args);
  ~ParallelQuickSort();
  void Pre() { }
  void Run(unsigned int numThreads);
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T * v;
  /* prohibit copy and assignment */
  ParallelQuickSort(const ParallelQuickSort & other);
  ParallelQuickSort & operator=(const ParallelQuickSort & other);
};

} // namespace perf
} // namespace algorithms
} // namespace embb

#include <quick_sort_perf-inl.h>

#endif  // EMBB_ALGORITHMS_PERF_QUICK_SORT_PERF_H_

