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

#ifndef EMBB_ALGORITHMS_PERF_FOR_EACH_PERF_H_
#define EMBB_ALGORITHMS_PERF_FOR_EACH_PERF_H_

#include <embb/base/perf/call_args.h>
#include <cmath>

namespace embb {
namespace algorithms {
namespace perf {

/**
 * Operation performed in each loop iteration.
 */
template<typename T>
class ForEachOp {
public:
  explicit ForEachOp(const embb::base::perf::CallArgs & args) :
    load_factor(args.LoadFactor()) { }
  void operator()(T & val) const {
    T x = val;
    for (unsigned int i = 0; i < load_factor; i++) {
      x = 2 * x * x;
      x = sqrt(x);
      x = x / sqrt(static_cast<T>(2));
    }
    val = x;
  }
private:
  size_t load_factor;
};

template<typename T>
class ForEachFunctor {
public:
  ForEachFunctor(const embb::base::perf::CallArgs & args) :
    cargs(args), op(args) { }
  void operator()(T & value) const {
    op(value);
  }
  ForEachFunctor(const ForEachFunctor & other) :
    cargs(other.cargs), op(other.op) { }
  ForEachFunctor & operator=(const ForEachFunctor & other) {
    if (&other != *this) {
      cargs = other.cargs;
      op = other.op;
    }
    return *this;
  }
private:
  const embb::base::perf::CallArgs & cargs;
  ForEachOp<T> op;
};

template<typename T>
class SerialForEach {
public:
  explicit SerialForEach(const embb::base::perf::CallArgs & args);
  ~SerialForEach();
  void Pre() { }
  void Run();
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  ForEachOp<T> op;
  const size_t vector_size;
  T * v;
  /* prohibit copy and assignment */
  SerialForEach(const SerialForEach & other);
  SerialForEach & operator=(const SerialForEach & other);
};

template<typename T>
class ParallelForEach {
public:
  explicit ParallelForEach(const embb::base::perf::CallArgs & args);
  ~ParallelForEach();
  void Pre();
  void Run(unsigned int numThreads);
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T * v;
  /* prohibit copy and assignment */
  ParallelForEach(const ParallelForEach & other);
  ParallelForEach & operator=(const ParallelForEach & other);
};

} // namespace perf
} // namespace algorithms
} // namespace embb

#include <for_each_perf-inl.h>

#endif /* EMBB_ALGORITHMS_PERF_FOR_PERF_H_ */

