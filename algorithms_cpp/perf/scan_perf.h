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

#ifndef EMBB_ALGORITHMS_PERF_SCAN_PERF_H_
#define EMBB_ALGORITHMS_PERF_SCAN_PERF_H_

#include <embb/base/perf/call_args.h>
#include <vector>

namespace embb {
namespace algorithms {
namespace perf {


template<typename T>
class RamStressScanOp {
  size_t load_factor;
  const T * const in;
public:
   explicit RamStressScanOp(const CallArgs & args, const T inVector[]) :
     load_factor(args.LoadFactor()), in(inVector) { }
public:
  T operator()(const T & lhs, const T & rhs) {
    T value = lhs;
    // rhs is index. First iteration in range has
    // lhs = 0 (neutral element).
    // artificial complexity
    for (unsigned int k = 0; k < load_factor; k++) {
      // artificial complexity
      value += rhs;
    }
    return value;
  }
  RamStressScanOp(const RamStressScanOp & other)
    : load_factor(other.load_factor), in(other.in) { }
  RamStressScanOp & operator=(const RamStressScanOp & other) {
    if (*this != &other) {
      load_factor = other.load_factor;
      in = other.in;
    }
    return *this;
  }
};

template<typename T>
class CpuStressScanOp {
  size_t load_factor;
public:
  explicit CpuStressScanOp(const CallArgs & args) :
    load_factor(args.LoadFactor()) { }
public:
  T operator()(const T & lhs, const T &) {    
    T value = lhs;
    // Ignore rhs value, always add 1, corresponding
    // to input value in RAM stress operation
    for (unsigned int k = 0; k < load_factor; k++) {
      // artificial complexity
      value += static_cast<T>(1);
    }
    return value;
  }
};

template<typename T>
class SerialScan {
public:
  explicit SerialScan(const embb::base::perf::CallArgs & args);
  ~SerialScan();
  void Pre() { }
  void Run();
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  unsigned int load_factor;
  T * in;
  T * out;
  T result;
  /* prohibit copy and assignment */
  SerialScan(const SerialScan & other);
  SerialScan & operator=(const SerialScan & other);
};

template<typename T>
class ParallelScan {
public:
  explicit ParallelScan(const embb::base::perf::CallArgs & args);
  ~ParallelScan();
  void Pre();
  void Run(unsigned int numThreads);
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T * in;
  T * out;
  T result;
  /* prohibit copy and assignment */
  ParallelScan(const ParallelScan & other);
  ParallelScan & operator=(const ParallelScan & other);
};


} // namespace perf
} // namespace algorithms
} // namespace embb

#include <scan_perf-inl.h>

#endif /* EMBB_ALGORITHMS_PERF_SCAN_PERF_H_ */
