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

#ifndef EMBB_DATAFLOW_INTERNAL_SINK_EXECUTOR_H_
#define EMBB_DATAFLOW_INTERNAL_SINK_EXECUTOR_H_

#include <embb/base/function.h>

#include <embb/dataflow/internal/inputs.h>

namespace embb {
namespace dataflow {
namespace internal {

template <class Inputs>
class SinkExecutor;

template <int Slices, typename I1>
class SinkExecutor< Inputs<Slices, I1> > {
 public:
  typedef embb::base::Function<void, I1 const &> FunctionType;

  explicit SinkExecutor(FunctionType func) : function_(func) {}

  void Execute(
    int clock,
    Inputs<Slices, I1> & inputs) {
    function_(
      inputs.template Get<0>().GetValue(clock));
  }

 private:
  FunctionType function_;
};

template <int Slices, typename I1, typename I2>
class SinkExecutor< Inputs<Slices, I1, I2> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &> FunctionType;

  explicit SinkExecutor(FunctionType func) : function_(func) {}

  void Execute(
    int clock,
    Inputs<Slices, I1, I2> & inputs) {
    function_(
      inputs.template Get<0>().GetValue(clock),
      inputs.template Get<1>().GetValue(clock));
  }

 private:
  FunctionType function_;
};

template <int Slices, typename I1, typename I2, typename I3>
class SinkExecutor< Inputs<Slices, I1, I2, I3> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, I3 const &>
    FunctionType;

  explicit SinkExecutor(FunctionType func) : function_(func) {}

  void Execute(
    int clock,
    Inputs<Slices, I1, I2, I3> & inputs) {
    function_(
      inputs.template Get<0>().GetValue(clock),
      inputs.template Get<1>().GetValue(clock),
      inputs.template Get<2>().GetValue(clock));
  }

 private:
  FunctionType function_;
};

template <int Slices, typename I1, typename I2, typename I3, typename I4>
class SinkExecutor< Inputs<Slices, I1, I2, I3, I4> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, I3 const &,
    I4 const &> FunctionType;

  explicit SinkExecutor(FunctionType func) : function_(func) {}

  void Execute(
    int clock,
    Inputs<Slices, I1, I2, I3, I4> & inputs) {
    function_(
      inputs.template Get<0>().GetValue(clock),
      inputs.template Get<1>().GetValue(clock),
      inputs.template Get<2>().GetValue(clock),
      inputs.template Get<3>().GetValue(clock));
  }

 private:
  FunctionType function_;
};

template <int Slices, typename I1, typename I2, typename I3, typename I4,
  typename I5>
class SinkExecutor< Inputs<Slices, I1, I2, I3, I4, I5> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, I3 const &,
    I4 const &, I5 const &> FunctionType;

  explicit SinkExecutor(FunctionType func) : function_(func) {}

  void Execute(
    int clock,
    Inputs<Slices, I1, I2, I3, I4, I5> & inputs) {
    function_(
      inputs.template Get<0>().GetValue(clock),
      inputs.template Get<1>().GetValue(clock),
      inputs.template Get<2>().GetValue(clock),
      inputs.template Get<3>().GetValue(clock),
      inputs.template Get<4>().GetValue(clock));
  }

 private:
  FunctionType function_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SINK_EXECUTOR_H_
