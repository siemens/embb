/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
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

#ifndef EMBB_ALGORITHMS_INTERNAL_PREDICATE_JOB_FUNCTOR_H_
#define EMBB_ALGORITHMS_INTERNAL_PREDICATE_JOB_FUNCTOR_H_

#include <embb/mtapi/mtapi.h>

namespace embb {
namespace algorithms {
namespace internal {

template<typename ElementType>
class PredicateJobFunctor {
 public:
  PredicateJobFunctor(embb::mtapi::Job comparison,
    embb::mtapi::ExecutionPolicy const & policy)
    : comparison_(comparison) {
    attr_.SetPolicy(policy);
  }

  bool operator () (ElementType const & val) const {
    struct {
      ElementType in;
    } inputs;
    inputs.in = val;
    struct {
      bool out;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, comparison_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      &attr_.GetInternal());
    task.Wait(MTAPI_INFINITE);
    return outputs.out;
  }

 private:
  embb::mtapi::Job comparison_;
  embb::mtapi::TaskAttributes attr_;
};

}  // namespace internal
}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_PREDICATE_JOB_FUNCTOR_H_
