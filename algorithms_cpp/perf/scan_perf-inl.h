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

#ifndef EMB_ALGORITHMS_PERF_SCAN_PERF_INL_H_
#define EMB_ALGORITHMS_PERF_SCAN_PERF_INL_H_

#include <scan_perf.h>
#include <embb/algorithms/scan.h>
#include <embb/base/perf/call_args.h>
#include <embb/base/memory_allocation.h>
#include <index_iterator.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace algorithms {
namespace perf {

using namespace embb::base;

template<typename T>
SerialScan<T>::SerialScan(const embb::base::perf::CallArgs & args) :
  cargs(args),
  vector_size(args.VectorSize()),
  load_factor(args.LoadFactor()) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    out = (T *) Allocation::AllocateCacheAligned(
      vector_size * sizeof(T));
    in  = (T *) Allocation::AllocateCacheAligned(
      vector_size * sizeof(T));
    for (size_t i = 0; i < vector_size; i++) {
      in[i] = static_cast<T>(1);
    }
  }
  else {
    out = 0;
    in  = 0;
  }
}

template<typename T>
SerialScan<T>::~SerialScan() {
  if (in != 0) {
    Allocation::FreeAligned(in);
  }
  if (out != 0) {
    Allocation::FreeAligned(out);
  }
}

template<typename T>
void SerialScan<T>::Run() {
  T total = 0;
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    for (unsigned int i = 0; i < vector_size; i++) {
      // artificial complexity
      for (unsigned int k = 0; k < load_factor; k++) {
        total += static_cast<T>(in[i]);
      }
      out[i] = total;
    }
    result = out[vector_size - 1];
  }
  else {
    for (unsigned int i = 0; i < vector_size; i++) {
      // artificial complexity
      for (unsigned int k = 0; k < load_factor; k++) {
        total += 1;
      }
      out[i] = total;
    }
    result = out[vector_size - 1];
  }
}

template<typename T>
ParallelScan<T>::ParallelScan(const embb::base::perf::CallArgs & args) :
  cargs(args), vector_size(args.VectorSize()) {
  if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    in = (T *) Allocation::AllocateCacheAligned(
      vector_size * sizeof(T));
    for (size_t i = 0; i < vector_size; i++) {
      in[i] = static_cast<T>(1);
    }
  }
  else {
    in  = 0;
  }
  out = (T *) Allocation::AllocateCacheAligned(
    vector_size * sizeof(T));
}

template<typename T>
ParallelScan<T>::~ParallelScan() {
  if (in != 0) {
    Allocation::FreeAligned(in);
  }
}

template<typename T>
void ParallelScan<T>::Run(unsigned int numThreads) {
  if (cargs.StressMode() == CallArgs::CPU_STRESS) {
    CpuStressScanOp<T> op(cargs);
    embb::algorithms::Scan(
      // Using iterator returning index value to avoid 
      // memory access
      IndexIterator<T>(0),
      IndexIterator<T>(static_cast<int>(vector_size)),
      out,
      static_cast<T>(0),            // neutral element
      op,                           // scan aggregation functor
      embb::algorithms::Identity(), // no transformation
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads
      );
    result = out[vector_size - 1];
  }
  else if (cargs.StressMode() == CallArgs::RAM_STRESS) {
    RamStressScanOp<T> op(cargs, in);
    embb::algorithms::Scan(
      in, in + vector_size, 
      out,
      static_cast<T>(0),            // neutral element
      op,                           // scan aggregation functor
      embb::algorithms::Identity(), // no transformation
      embb::tasks::ExecutionPolicy(),
      vector_size / numThreads
      );
    result = out[vector_size - 1];
  }
}

} // namespace perf
} // namespace algorithms
} // namespace embb

#endif /* EMB_ALGORITHMS_PERF_SCAN_PERF_INL_H_ */
