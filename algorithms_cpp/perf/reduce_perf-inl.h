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

#ifndef EMBB_ALGORITHMS_PERF_REDUCE_PERF_TBB_INL_H_
#define EMBB_ALGORITHMS_PERF_REDUCE_PERF_TBB_INL_H_

#include <embb/base/perf/call_args.h>
#include <reduce_perf.h>
#include <embb/base/memory_allocation.h>
#include <embb/algorithms/reduce.h>
#include <index_iterator.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace algorithms {
namespace perf {

using embb::base::perf::CallArgs;

template<typename T>
SerialReduce<T>::SerialReduce(
  const embb::base::perf::CallArgs & args) :
  cargs(args),
  vector_size(args.VectorSize()) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    v = static_cast<T*>(
      embb::base::Allocation::AllocateCacheAligned(
      vector_size * sizeof(T)));
    for (size_t i = 0; i < vector_size; i++) {
      v[i] = (T)i;
    }
  }
  else {
    v = 0;
  }
}

template<typename T>
SerialReduce<T>::~SerialReduce() {
  if (v != 0) {
    embb::base::Allocation::FreeAligned(v);
  }
}

template<typename T>
void SerialReduce<T>::Run() {
  result = 0;
  if (cargs.StressMode() == CallArgs::CPU_STRESS) {
    TransformOp<T> op(static_cast<T>(1.0) / vector_size, cargs);
    for (size_t i = 0; i < vector_size; i++) {
      result += op((T)i);
    }
    result /= static_cast<T>(vector_size);
  }
  else if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    TransformOp<T> op(static_cast<T>(1.0) / vector_size, cargs);
    for (size_t i = 0; i < vector_size; i++) {
      result += op(v[i]);
    }
    result /= static_cast<T>(vector_size);
  }
}

template<typename T>
ParallelReduce<T>::ParallelReduce(
  const embb::base::perf::CallArgs & args) :
  cargs(args),
  vector_size(args.VectorSize()) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    v = static_cast<T*>(
      embb::base::Allocation::AllocateCacheAligned(
      vector_size * sizeof(T)));
    for (size_t i = 0; i < vector_size; i++) {
      v[i] = (T)i;
    }
  }
  else {
    v = 0;
  }
}

template<typename T>
ParallelReduce<T>::~ParallelReduce() {
  if (v != 0) {
    embb::base::Allocation::FreeAligned(v);
  }
}

template<typename T>
void ParallelReduce<T>::Run(unsigned int numThreads) {
  TransformOp<T> op(static_cast<T>(1) / vector_size, cargs);
  if (cargs.StressMode() == CallArgs::CPU_STRESS) {
    result = embb::algorithms::Reduce(
      // Using iterator returning index value to avoid 
      // memory access
      IndexIterator<T>(0),
      IndexIterator<T>(static_cast<int>(vector_size)),
      static_cast<T>(0),     // neutral element
      ::std::plus<T>(),      // reduce op
      op,                    // transform op
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads
      );
  }
  else if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    result = embb::algorithms::Reduce(
      v, v + vector_size,    // input
      static_cast<T>(0),     // neutral element
      ::std::plus<T>(),      // reduce op
      op,                    // transform op
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads
      );
  }
  result /= static_cast<T>(vector_size);
}

} // namespace perf
} // namespace algorithms
} // namespace embb

#endif
