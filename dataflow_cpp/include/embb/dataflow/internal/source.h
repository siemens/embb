/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#ifndef EMBB_DATAFLOW_INTERNAL_SOURCE_H_
#define EMBB_DATAFLOW_INTERNAL_SOURCE_H_

#include <embb/dataflow/internal/node.h>
#include <embb/dataflow/internal/outputs.h>
#include <embb/dataflow/internal/source_executor.h>

namespace embb {
namespace dataflow {
namespace internal {

template <int Slices, class Outputs> class Source;

template <
  int Slices,
  typename O1, typename O2, typename O3, typename O4, typename O5>
class Source< Slices, Outputs<Slices, O1, O2, O3, O4, O5> >
  : public Node {
 public:
  typedef Outputs<Slices, O1, O2, O3, O4, O5> OutputsType;
  typedef SourceExecutor< OutputsType > ExecutorType;
  typedef typename ExecutorType::FunctionType FunctionType;

  explicit Source(FunctionType function)
    : executor_(function), not_done_(true) {
  }

  virtual bool HasOutputs() const {
    return outputs_.Size() > 0;
  }

  virtual void Run(int clock) {
    not_done_ = executor_.Execute(clock, outputs_);
  }

  virtual void Init(InitData * init_data) {
    SetScheduler(init_data->sched);
    executor_.Init(init_data, outputs_);
  }

  virtual bool Start(int clock) {
    if (not_done_) {
      Run(clock);
    }
    return not_done_;
  }

  OutputsType & GetOutputs() {
    return outputs_;
  }

  template <int Index>
  typename TypeAt<typename OutputsType::Types, Index>::Result & GetOutput() {
    return outputs_.template Get<Index>();
  }

  template <typename T>
  void operator >> (T & target) {
    GetOutput<0>() >> target.template GetInput<0>();
  }

 private:
  OutputsType outputs_;
  ExecutorType executor_;
  volatile bool not_done_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SOURCE_H_
