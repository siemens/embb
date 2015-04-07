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

#ifndef EMBB_ALGORITHMS_PERF_REDUCE_PERF_H_
#define EMBB_ALGORITHMS_PERF_REDUCE_PERF_H_

#include <embb/base/perf/call_args.h>

namespace embb {
namespace algorithms {
namespace perf {

template<typename T>
class TransformOp {
  T step_size;
  size_t load_factor;
public:
  explicit TransformOp(T stepSize, const embb::base::perf::CallArgs & args) :
    step_size(stepSize),
    load_factor(args.LoadFactor()) { }
  TransformOp(const TransformOp & other) :
    step_size(other.step_size),
    load_factor(other.load_factor) { }
  TransformOp & operator=(const TransformOp & other) {
    step_size = other.step_size;
    load_factor = other.load_factor;
  }
  T operator()(T val) const {
    T x = 0;
    // Simulate more complex operation depending on
    // load factor. Default load factor is 1.
    for (size_t i = 0; i < load_factor * 10000; ++i) {
      x = (val + static_cast<T>(0.5)) * step_size * i;
      x = static_cast<T>(4.0 / (1.0 + x * x / load_factor));
    }
    return x;
  }
};

template<typename T>
class SerialReduce {
public:
  explicit SerialReduce(
    const embb::base::perf::CallArgs & args);
  ~SerialReduce();
  void Pre() { }
  void Run();
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T *v;
  T result;

  /* prohibit copy and assignment */
  SerialReduce(const SerialReduce &other);
  SerialReduce& operator=(const SerialReduce &other);
};

template<typename T>
class ParallelReduce {
public:
  explicit ParallelReduce(
    const embb::base::perf::CallArgs & args);
  ~ParallelReduce();
  void Pre() { }
  void Run(unsigned int numThreads);
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T *v;
  T result;
  
  /* prohibit copy and assignment */
  ParallelReduce(const ParallelReduce &other);
  ParallelReduce& operator=(const ParallelReduce &other);
};

}
}
}

#include <reduce_perf-inl.h>

#endif /* EMBB_ALGORITHMS_PERF_REDUCE_PERF_H_ */
