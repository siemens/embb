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

#ifndef EMBB_DATAFLOW_INTERNAL_PROCESS_EXECUTOR_H_
#define EMBB_DATAFLOW_INTERNAL_PROCESS_EXECUTOR_H_

#include <embb/base/function.h>

#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/inputs.h>
#include <embb/dataflow/internal/outputs.h>

#include <embb/mtapi/mtapi.h>

namespace embb {
namespace dataflow {
namespace internal {

template <class Inputs, class Outputs>
class ProcessExecutor;

template <typename I1, typename O1>
class ProcessExecutor< Inputs<I1>, Outputs<O1> > {
 public:
  typedef embb::base::Function<void, I1 const &, O1 &> FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1> & inputs,
    Outputs<O1> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      function_(
        inputs.template Get<0>().GetValue(clock),
        o1);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1,
    O1 & o1) {
    struct {
      I1 i1_;
    } inputs = { i1 };
    struct {
      O1 o1_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
  }
};

template <typename I1, typename O1, typename O2>
class ProcessExecutor< Inputs<I1>, Outputs<O1, O2> > {
 public:
  typedef embb::base::Function<void, I1 const &, O1 &, O2 &> FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1> & inputs,
    Outputs<O1, O2> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      O2 o2;
      function_(
        inputs.template Get<0>().GetValue(clock),
        o1, o2);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
      outputs.template Get<1>().Send(Signal<O2>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1,
    O1 & o1, O2 & o2) {
    struct {
      I1 i1_;
    } inputs = { i1 };
    struct {
      O1 o1_;
      O2 o2_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
  }
};

template <typename I1, typename O1, typename O2, typename O3>
class ProcessExecutor< Inputs<I1>, Outputs<O1, O2, O3> > {
 public:
  typedef embb::base::Function<void, I1 const &, O1 &, O2 &, O3 &>
    FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1> & inputs,
    Outputs<O1, O2, O3> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      O2 o2;
      O3 o3;
      function_(
        inputs.template Get<0>().GetValue(clock),
        o1, o2, o3);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
      outputs.template Get<2>().Send(Signal<O3>(clock, o3));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
      outputs.template Get<1>().Send(Signal<O2>(clock));
      outputs.template Get<2>().Send(Signal<O3>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1,
    O1 & o1, O2 & o2, O3 & o3) {
    struct {
      I1 i1_;
    } inputs = { i1 };
    struct {
      O1 o1_;
      O2 o2_;
      O3 o3_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    o3 = outputs.o3_;
  }
};

template <typename I1, typename O1, typename O2, typename O3,
  typename O4>
class ProcessExecutor< Inputs<I1>, Outputs<O1, O2, O3, O4> > {
 public:
  typedef embb::base::Function<void, I1 const &, O1 &, O2 &, O3 &, O4 &>
    FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1> & inputs,
    Outputs<O1, O2, O3, O4> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      O2 o2;
      O3 o3;
      O4 o4;
      function_(
        inputs.template Get<0>().GetValue(clock),
        o1, o2, o3, o4);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
      outputs.template Get<2>().Send(Signal<O3>(clock, o3));
      outputs.template Get<3>().Send(Signal<O4>(clock, o4));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
      outputs.template Get<1>().Send(Signal<O2>(clock));
      outputs.template Get<2>().Send(Signal<O3>(clock));
      outputs.template Get<3>().Send(Signal<O4>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1,
    O1 & o1, O2 & o2, O3 & o3, O4 & o4) {
    struct {
      I1 i1_;
    } inputs = { i1 };
    struct {
      O1 o1_;
      O2 o2_;
      O3 o3_;
      O4 o4_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    o3 = outputs.o3_;
    o4 = outputs.o4_;
  }
};

template <typename I1, typename I2, typename O1>
class ProcessExecutor< Inputs<I1, I2>, Outputs<O1> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, O1 &>
    FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1, I2> & inputs,
    Outputs<O1> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      function_(
        inputs.template Get<0>().GetValue(clock),
        inputs.template Get<1>().GetValue(clock),
        o1);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1, I2 const & i2,
    O1 & o1) {
    struct {
      I1 i1_;
      I2 i2_;
    } inputs = { i1, i2 };
    struct {
      O1 o1_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
  }
};

template <typename I1, typename I2, typename O1, typename O2>
class ProcessExecutor< Inputs<I1, I2>, Outputs<O1, O2> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, O1 &, O2 &>
    FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1, I2> & inputs,
    Outputs<O1, O2> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      O2 o2;
      function_(
        inputs.template Get<0>().GetValue(clock),
        inputs.template Get<1>().GetValue(clock),
        o1, o2);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
      outputs.template Get<1>().Send(Signal<O2>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1, I2 const & i2,
    O1 & o1, O2 & o2) {
    struct {
      I1 i1_;
      I2 i2_;
    } inputs = { i1, i2 };
    struct {
      O1 o1_;
      O2 o2_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
  }
};

template <typename I1, typename I2, typename O1, typename O2,
  typename O3>
class ProcessExecutor< Inputs<I1, I2>, Outputs<O1, O2, O3> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, O1 &, O2 &, O3 &>
    FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1, I2> & inputs,
    Outputs<O1, O2, O3> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      O2 o2;
      O3 o3;
      function_(
        inputs.template Get<0>().GetValue(clock),
        inputs.template Get<1>().GetValue(clock),
        o1, o2, o3);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
      outputs.template Get<2>().Send(Signal<O3>(clock, o3));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
      outputs.template Get<1>().Send(Signal<O2>(clock));
      outputs.template Get<2>().Send(Signal<O3>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1, I2 const & i2,
    O1 & o1, O2 & o2, O3 & o3) {
    struct {
      I1 i1_;
      I2 i2_;
    } inputs = { i1, i2 };
    struct {
      O1 o1_;
      O2 o2_;
      O3 o3_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    o3 = outputs.o3_;
  }
};

template <typename I1, typename I2, typename I3, typename O1>
class ProcessExecutor< Inputs<I1, I2, I3>, Outputs<O1> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, I3 const &, O1 &>
    FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1, I2, I3> & inputs,
    Outputs<O1> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      function_(
        inputs.template Get<0>().GetValue(clock),
        inputs.template Get<1>().GetValue(clock),
        inputs.template Get<2>().GetValue(clock),
        o1);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1, I2 const & i2, I3 const & i3,
    O1 & o1) {
    struct {
      I1 i1_;
      I2 i2_;
      I3 i3_;
    } inputs = { i1, i2, i3 };
    struct {
      O1 o1_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
  }
};

template <typename I1, typename I2, typename I3, typename O1,
  typename O2>
class ProcessExecutor< Inputs<I1, I2, I3>, Outputs<O1, O2> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, I3 const &,
    O1 &, O2 &> FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1, I2, I3> & inputs,
    Outputs<O1, O2> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      O2 o2;
      function_(
        inputs.template Get<0>().GetValue(clock),
        inputs.template Get<1>().GetValue(clock),
        inputs.template Get<2>().GetValue(clock),
        o1, o2);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
      outputs.template Get<1>().Send(Signal<O2>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1, I2 const & i2, I3 const & i3,
    O1 & o1, O2 & o2) {
    struct {
      I1 i1_;
      I2 i2_;
      I3 i3_;
    } inputs = { i1, i2, i3 };
    struct {
      O1 o1_;
      O2 o2_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
  }
};

template <typename I1, typename I2, typename I3, typename I4,
  typename O1>
class ProcessExecutor< Inputs<I1, I2, I3, I4>, Outputs<O1> > {
 public:
  typedef embb::base::Function<void, I1 const &, I2 const &, I3 const &,
    I4 const &, O1 &> FunctionType;

  explicit ProcessExecutor(FunctionType func) : function_(func) {}
  explicit ProcessExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &ProcessExecutor::ExecuteJob);
  }

  void Execute(
    int clock,
    Inputs<I1, I2, I3, I4> & inputs,
    Outputs<O1> & outputs) {
    if (inputs.AreNoneBlank(clock)) {
      O1 o1;
      function_(
        inputs.template Get<0>().GetValue(clock),
        inputs.template Get<1>().GetValue(clock),
        inputs.template Get<2>().GetValue(clock),
        inputs.template Get<3>().GetValue(clock),
        o1);
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
    } else {
      outputs.template Get<0>().Send(Signal<O1>(clock));
    }
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  void ExecuteJob(
    I1 const & i1, I2 const & i2, I3 const & i3, I4 const & i4,
    O1 & o1) {
    struct {
      I1 i1_;
      I2 i2_;
      I3 i3_;
      I4 i4_;
    } inputs = { i1, i2, i3, i4 };
    struct {
      O1 o1_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task = node.Start(
      MTAPI_TASK_ID_NONE, job_.GetInternal(),
      &inputs, sizeof(inputs),
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_PROCESS_EXECUTOR_H_
