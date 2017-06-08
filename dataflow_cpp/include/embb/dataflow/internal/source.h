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

#ifndef EMBB_DATAFLOW_INTERNAL_SOURCE_H_
#define EMBB_DATAFLOW_INTERNAL_SOURCE_H_

#include <embb/base/atomic.h>

#include <embb/dataflow/internal/node.h>
#include <embb/dataflow/internal/outputs.h>
#include <embb/dataflow/internal/source_executor.h>
#include <embb/dataflow/internal/action.h>

namespace embb {
namespace dataflow {
namespace internal {

template <class Outputs> class Source;

template <
  typename O1, typename O2, typename O3, typename O4, typename O5>
class Source< Outputs<O1, O2, O3, O4, O5> >
  : public Node {
 public:
  typedef Outputs<O1, O2, O3, O4, O5> OutputsType;
  typedef SourceExecutor< OutputsType > ExecutorType;
  typedef typename ExecutorType::FunctionType FunctionType;

  Source(Scheduler * sched, FunctionType function)
    : executor_(function)
    , not_done_(true)
    , action_(NULL)
    , stop_(false)
    , running_(0)
    , next_clock_(0)
    , slices_(0) {
    SetScheduler(sched);
  }

  Source(Scheduler * sched, embb::mtapi::Job job)
    : executor_(job)
    , not_done_(true)
    , action_(NULL)
    , stop_(false)
    , running_(0)
    , next_clock_(0)
    , slices_(0) {
    SetScheduler(sched);
  }

  ~Source() {
    stop_ = true;
    while (running_ > 0) {
      sched_->YieldToScheduler();
    }
    if (NULL != action_) {
      embb::base::Allocation::Free(action_);
    }
  }

  virtual bool HasOutputs() const {
    return outputs_.Size() > 0;
  }

  virtual void Run(int clock) {
    if (stop_) {
      return;
    }
    ++running_;
    // check if this process is due
    if (next_clock_ == clock) {
      not_done_ = executor_.Execute(clock, outputs_);
      ++next_clock_;
    } else {
      sched_->YieldToScheduler();
      // redeploy if not
      const int idx = clock % slices_;
      action_[idx] = Action(this, clock);
      sched_->Start(action_[idx], policy_);
    }
    --running_;
  }

  virtual bool IsFullyConnected() const {
    return outputs_.IsFullyConnected();
  }

  virtual void Start(int clock) {
    const int idx = clock % slices_;
    action_[idx] = Action(this, clock);
    sched_->Start(action_[idx], policy_);
  }

  virtual bool Wait(int clock) {
    while (next_clock_ <= clock) {
      sched_->YieldToScheduler();
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
  T & operator >> (T & target) {
    GetOutput<0>() >> target.template GetInput<0>();
    return target;
  }

 private:
  OutputsType outputs_;
  ExecutorType executor_;
  embb::base::Atomic<bool> not_done_;
  Action * action_;
  embb::base::Atomic<bool> stop_;
  embb::base::Atomic<int> running_;
  embb::base::Atomic<int> next_clock_;
  int slices_;

  virtual void SetSlices(int slices) {
    if (0 < slices_) {
      embb::base::Allocation::Free(action_);
      action_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      action_ = reinterpret_cast<Action*>(
        embb::base::Allocation::Allocate(
          sizeof(Action)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        action_[ii] = Action();
      }
    }
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SOURCE_H_
