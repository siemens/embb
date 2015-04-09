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

#ifndef EMBB_ALGORITHMS_PERF_FOR_EACH_PERF_INL_H_
#define EMBB_ALGORITHMS_PERF_FOR_EACH_PERF_INL_H_

#include <for_each_perf.h>
#include <embb/algorithms/for_each.h>
#include <index_iterator.h>
#include <embb/base/perf/call_args.h>
#include <embb/base/memory_allocation.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace algorithms {
namespace perf {

template<typename T>
SerialForEach<T>::SerialForEach(const embb::base::perf::CallArgs & args)
: cargs(args), op(args), vector_size(args.VectorSize()) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    v = static_cast<T *>(embb::base::Allocation::AllocateCacheAligned(
      vector_size * sizeof(T)));
    for (size_t i = 0; i < vector_size; i++) {
      v[i] = static_cast<T>(i);
    }
  } else {
    v = 0;
  }
}

template<typename T>
SerialForEach<T>::~SerialForEach() {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    embb::base::Allocation::FreeAligned(v);
  }
}

template<typename T>
void SerialForEach<T>::Run() {
  if (cargs.StressMode() == CallArgs::CPU_STRESS) {
    for (size_t i = 0; i < vector_size; i++) {
      T v = static_cast<T>(i);
      op(v);
    }
  } else if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    for (size_t i = 0; i < vector_size; i++) {
      op(v[i]);
    }
  }
}

template<typename T>
ParallelForEach<T>::ParallelForEach(const embb::base::perf::CallArgs & args)
: cargs(args), vector_size(args.VectorSize()) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    v = static_cast<T *>(embb::base::Allocation::AllocateCacheAligned(
      vector_size * sizeof(T)));
    for (size_t i = 0; i < vector_size; i++) {
      v[i] = static_cast<T>(i);
    }
  } else {
    v = 0;
  }
}

template<typename T>
ParallelForEach<T>::~ParallelForEach() {
  if (v != 0) {
    embb::base::Allocation::FreeAligned(v);
  }
}

template<typename T>
void ParallelForEach<T>::Run(unsigned int numThreads) {
  if (cargs.StressMode() == CallArgs::CPU_STRESS) {
    // Computing input values, no memory access
    ForEachFunctor<T> op(cargs);
    embb::algorithms::ForEach(
      // Using iterator returning index value to avoid
      // memory access
      IndexIterator<T>(0),
      IndexIterator<T>(static_cast<int>(vector_size)),
      op,
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads);
  } else if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    // Reading input values from memory
    ForEachFunctor<T> op(cargs);
    embb::algorithms::ForEach(
      v, v + vector_size,
      op,
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads);
  }
}

} // namespace perf
} // namespace algorithms
} // namespace embb

#endif /* EMBB_ALGORITHMS_PERF_FOR_EACH_PERF_INL_H_ */
