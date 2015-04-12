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

#ifndef EMBB_BASE_PERF_CALL_ARGS_H_
#define EMBB_BASE_PERF_CALL_ARGS_H_

#include <embb/base/core_set.h>
#include <string>
#include <ostream>

namespace embb {
namespace base {
namespace perf {

class CallArgs {
 public:
  typedef enum {
    UNDEFINED_STRESS_TYPE = 0,
    RAM_STRESS,
    CPU_STRESS
  } StressType;

 public:
  inline CallArgs() :
    stress_type(CallArgs::RAM_STRESS),
    max_threads(embb::base::CoreSet::CountAvailable()),
    vector_size(10000000),
    load_factor(10),
    counter_scale(0) { 
  }

  inline CallArgs(const CallArgs & other) :
    stress_type(other.stress_type),
    max_threads(other.max_threads),
    vector_size(other.vector_size),
    load_factor(other.load_factor),
    counter_scale(other.counter_scale) {
  }

  inline CallArgs(int argc, char * argv[]) {
    Parse(argc, argv);
  }

  void Print(::std::ostream & os);

  void Parse(int argc, char * argv[]);

  inline size_t MaxThreads() const {
    return max_threads;
  }

  inline unsigned int CounterScale() const {
    return counter_scale;
  }

  inline StressType StressMode() const {
    return stress_type;
  }

  inline ::std::string StressModeName() const {
    return ((StressMode() == CPU_STRESS)
      ? "cpu"
      : "ram");
  }

  inline size_t VectorSize() const {
    return vector_size;
  }

  inline size_t LoadFactor() const {
    return load_factor;
  }
  
 private:
  StressType stress_type;
  size_t max_threads;
  size_t vector_size;
  size_t load_factor;
  unsigned int counter_scale;
};

} // namespace perf
} // namespace base
} // namespace embb

#endif
