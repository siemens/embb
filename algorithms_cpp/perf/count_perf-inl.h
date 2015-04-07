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

#ifndef EMBB_ALGORITHMS_PERF_COUNT_PERF_INL_H_
#define EMBB_ALGORITHMS_PERF_COUNT_PERF_INL_H_

#include <embb/base/perf/call_args.h>
#include <count_perf.h>
#include <embb/base/memory_allocation.h>
#include <embb/algorithms/count.h>
#include <index_iterator.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace algorithms {
namespace perf {

using embb::base::perf::CallArgs;

template<typename T>
SerialCount<T>::SerialCount(const embb::base::perf::CallArgs & args) :
cargs(args), vector_size(args.VectorSize()), result(0) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    a = (T *)Allocation::AllocateCacheAligned(
      vector_size * sizeof(T));
    for (size_t i = 0; i < vector_size; i++) {
      a[i] = static_cast<T>(i);
    }
  }
  else {
    a = 0;
  }
}

template<typename T>
SerialCount<T>::~SerialCount() {
  if (a != 0) {
    Allocation::FreeAligned(a);
  }
}

template<typename T>
void SerialCount<T>::Run() {
  T element = static_cast<T>(vector_size / 2);
  for (size_t i = 0; i != vector_size; ++i) {
    if (a[i] == element) {
      ++result;
    }
  }
}

template<typename T>
ParallelCount<T>::ParallelCount(const embb::base::perf::CallArgs & args) :
cargs(args), vector_size(args.VectorSize()), result(0) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    a = (T *)Allocation::AllocateCacheAligned(
      vector_size * sizeof(T));
    for (size_t i = 0; i < vector_size; i++) {
      a[i] = static_cast<T>(i);
    }
  }
  else {
    a = 0;
  }
}

template<typename T>
ParallelCount<T>::~ParallelCount() {
  if (a != 0) {
    Allocation::FreeAligned(a);
  }
}

template<typename T>
void ParallelCount<T>::Run(unsigned int numThreads) {
  T element = static_cast<T>(vector_size / 2);
  if (cargs.StressMode() == CallArgs::CPU_STRESS) {
    result = static_cast<size_t>(embb::algorithms::Count(
      // Using iterator returning index value to avoid 
      // memory access
      IndexIterator<T>(0),
      IndexIterator<T>(static_cast<int>(vector_size)),
      element,
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads));
  }
  else if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    result = static_cast<size_t>(embb::algorithms::Count(
      a, 
      a + vector_size,
      element,
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads));
  }
}

} // namespace perf
} // namespace algorithms
} // namespace embb

#endif /* EMBB_ALGORITHMS_PERF_COUNT_PERF_INL_H_ */
