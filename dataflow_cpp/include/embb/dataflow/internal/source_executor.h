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

#ifndef EMBB_DATAFLOW_INTERNAL_SOURCE_EXECUTOR_H_
#define EMBB_DATAFLOW_INTERNAL_SOURCE_EXECUTOR_H_

#include <embb/base/function.h>

#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/outputs.h>

#include <embb/mtapi/mtapi.h>

namespace embb {
namespace dataflow {
namespace internal {

template <class OUTPUTS>
class SourceExecutor;

template <typename O1>
class SourceExecutor< Outputs<O1> > {
 public:
  typedef embb::base::Function<bool, O1 &> FunctionType;

  explicit SourceExecutor(FunctionType func) : function_(func) {}
  explicit SourceExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &SourceExecutor::ExecuteJob);
  }

  bool Execute(
    int clock,
    Outputs<O1> & outputs) {
    O1 o1;
    bool result = function_(o1);
    if (result) {
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
    }
    return result;
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  bool ExecuteJob(O1 & o1) {
    struct {
      mtapi_boolean_t result;
      O1 o1_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task =
      node.Start(MTAPI_TASK_ID_NONE, job_.GetInternal(),
      MTAPI_NULL, 0,
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    return outputs.result != MTAPI_FALSE;
  }
};

template <typename O1, typename O2>
class SourceExecutor< Outputs<O1, O2> > {
 public:
  typedef embb::base::Function<bool, O1 &, O2 &> FunctionType;

  explicit SourceExecutor(FunctionType func) : function_(func) {}
  explicit SourceExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &SourceExecutor::ExecuteJob);
  }

  bool Execute(
    int clock,
    Outputs<O1, O2> & outputs) {
    O1 o1;
    O2 o2;
    bool result = function_(o1, o2);
    if (result) {
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
    }
    return result;
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  bool ExecuteJob(O1 & o1, O2 & o2) {
    struct {
      mtapi_boolean_t result;
      O1 o1_;
      O2 o2_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task =
      node.Start(MTAPI_TASK_ID_NONE, job_.GetInternal(),
      MTAPI_NULL, 0,
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    return outputs.result != MTAPI_FALSE;
  }
};

template <typename O1, typename O2, typename O3>
class SourceExecutor< Outputs<O1, O2, O3> > {
 public:
  typedef embb::base::Function<bool, O1 &, O2 &, O3 &> FunctionType;

  explicit SourceExecutor(FunctionType func) : function_(func) {}
  explicit SourceExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &SourceExecutor::ExecuteJob);
  }

  bool Execute(
    int clock,
    Outputs<O1, O2, O3> & outputs) {
    O1 o1;
    O2 o2;
    O3 o3;
    bool result = function_(o1, o2, o3);
    if (result) {
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
      outputs.template Get<2>().Send(Signal<O3>(clock, o3));
    }
    return result;
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  bool ExecuteJob(O1 & o1, O2 & o2, O3 & o3) {
    struct {
      mtapi_boolean_t result;
      O1 o1_;
      O2 o2_;
      O3 o3_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task =
      node.Start(MTAPI_TASK_ID_NONE, job_.GetInternal(),
      MTAPI_NULL, 0,
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    o3 = outputs.o3_;
    return outputs.result != MTAPI_FALSE;
  }
};

template <typename O1, typename O2, typename O3, typename O4>
class SourceExecutor< Outputs<O1, O2, O3, O4> > {
 public:
  typedef embb::base::Function<bool, O1 &, O2 &, O3 &, O4 &> FunctionType;

  explicit SourceExecutor(FunctionType func) : function_(func) {}
  explicit SourceExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &SourceExecutor::ExecuteJob);
  }

  bool Execute(
    int clock,
    Outputs<O1, O2, O3, O4> & outputs) {
    O1 o1;
    O2 o2;
    O3 o3;
    O4 o4;
    bool result = function_(o1, o2, o3, o4);
    if (result) {
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
      outputs.template Get<2>().Send(Signal<O3>(clock, o3));
      outputs.template Get<3>().Send(Signal<O4>(clock, o4));
    }
    return result;
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  bool ExecuteJob(O1 & o1, O2 & o2, O3 & o3, O4 & o4) {
    struct {
      mtapi_boolean_t result;
      O1 o1_;
      O2 o2_;
      O3 o3_;
      O4 o4_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task =
      node.Start(MTAPI_TASK_ID_NONE, job_.GetInternal(),
      MTAPI_NULL, 0,
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    o3 = outputs.o3_;
    o4 = outputs.o4_;
    return outputs.result != MTAPI_FALSE;
  }
};

template <typename O1, typename O2, typename O3, typename O4,
  typename O5>
class SourceExecutor< Outputs<O1, O2, O3, O4, O5> > {
 public:
  typedef embb::base::Function<bool, O1 &, O2 &, O3 &, O4 &, O5 &> FunctionType;

  explicit SourceExecutor(FunctionType func) : function_(func) {}
  explicit SourceExecutor(embb::mtapi::Job job)
    : job_(job) {
    function_ = FunctionType(*this, &SourceExecutor::ExecuteJob);
  }

  bool Execute(
    int clock,
    Outputs<O1, O2, O3, O4, O5> & outputs) {
    O1 o1;
    O2 o2;
    O3 o3;
    O4 o4;
    O5 o5;
    bool result = function_(o1, o2, o3, o4, o5);
    if (result) {
      outputs.template Get<0>().Send(Signal<O1>(clock, o1));
      outputs.template Get<1>().Send(Signal<O2>(clock, o2));
      outputs.template Get<2>().Send(Signal<O3>(clock, o3));
      outputs.template Get<3>().Send(Signal<O4>(clock, o4));
      outputs.template Get<4>().Send(Signal<O5>(clock, o5));
    }
    return result;
  }

 private:
  FunctionType function_;
  embb::mtapi::Job job_;

  bool ExecuteJob(O1 & o1, O2 & o2, O3 & o3, O4 & o4, O5 & o5) {
    struct {
      mtapi_boolean_t result;
      O1 o1_;
      O2 o2_;
      O3 o3_;
      O4 o4_;
      O5 o5_;
    } outputs;
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    embb::mtapi::Task task =
      node.Start(MTAPI_TASK_ID_NONE, job_.GetInternal(),
      MTAPI_NULL, 0,
      &outputs, sizeof(outputs),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
    task.Wait();
    o1 = outputs.o1_;
    o2 = outputs.o2_;
    o3 = outputs.o3_;
    o4 = outputs.o4_;
    o5 = outputs.o5_;
    return outputs.result != MTAPI_FALSE;
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SOURCE_EXECUTOR_H_
