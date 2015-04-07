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

#ifndef EMBB_ALGORITHMS_CPP_PERF_COUNT_PERF_H_
#define EMBB_ALGORITHMS_CPP_PERF_COUNT_PERF_H_

#include <embb/base/perf/call_args.h>
#include <iterator>

namespace embb {
namespace algorithms {
namespace perf {

template<typename T>
class SerialCount {
public:
  explicit SerialCount(const embb::base::perf::CallArgs & args);
  ~SerialCount();
  void Pre() { }
  void Run();
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T * a;
  size_t result;
  /* prohibit copy and assignment */
  SerialCount(const SerialCount & other);
  SerialCount & operator=(const SerialCount & other);
};

template<typename T>
class ParallelCount {
public:
  explicit ParallelCount(const embb::base::perf::CallArgs & args);
  ~ParallelCount();
  void Pre() { }
  void Run(unsigned int numThreads);
  void Post() { }

private:
  const embb::base::perf::CallArgs & cargs;
  const size_t vector_size;
  T * a;
  size_t result;
  /* prohibit copy and assignment */
  ParallelCount(const ParallelCount & other);
  ParallelCount & operator=(const ParallelCount & other);
};

} // namespace perf
} // namespace algorithms
} // namespace embb

#include <count_perf-inl.h>

#endif /* EMBB_ALGORITHMS_CPP_PERF_COUNT_PERF_H_ */
