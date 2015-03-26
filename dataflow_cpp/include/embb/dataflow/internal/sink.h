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

#ifndef EMBB_DATAFLOW_INTERNAL_SINK_H_
#define EMBB_DATAFLOW_INTERNAL_SINK_H_

#include <embb/dataflow/internal/node.h>
#include <embb/dataflow/internal/inputs.h>
#include <embb/dataflow/internal/sink_executor.h>
#include <embb/dataflow/internal/action.h>

namespace embb {
namespace dataflow {
namespace internal {

template <int Slices, class Inputs> class Sink;

template <
  int Slices,
  typename I1, typename I2, typename I3, typename I4, typename I5>
class Sink< Slices, Inputs<Slices, I1, I2, I3, I4, I5> >
  : public Node
  , public ClockListener {
 public:
  typedef Inputs<Slices, I1, I2, I3, I4, I5> InputsType;
  typedef SinkExecutor< InputsType > ExecutorType;
  typedef typename ExecutorType::FunctionType FunctionType;

  explicit Sink(FunctionType function)
    : executor_(function) {
    next_clock_ = 0;
    queued_clock_ = 0;
    queue_id_ = GetNextProcessID();
    inputs_.SetListener(this);
  }

  void SetListener(ClockListener * listener) {
    listener_ = listener;
  }

  virtual bool HasInputs() const {
    return inputs_.Size() > 0;
  }

  virtual void Run(int clock) {
    if (inputs_.AreNoneBlank(clock)) {
      executor_.Execute(clock, inputs_);
    }
    listener_->OnClock(clock);
  }

  virtual void Init(InitData * init_data) {
    SetListener(init_data->sink_listener);
    SetScheduler(init_data->sched);
    listener_->OnInit(init_data);
  }

  InputsType & GetInputs() {
    return inputs_;
  }

  template <int Index>
  typename TypeAt<typename InputsType::Types, Index>::Result & GetInput() {
    return inputs_.template Get<Index>();
  }

  virtual void OnClock(int clock) {
    if (!inputs_.AreAtClock(clock)) {
      EMBB_THROW(embb::base::ErrorException,
        "Some inputs are not at expected clock.")
    }

    bool retry = true;
    while (retry) {
      int clk = next_clock_;
      int clk_end = clk + Slices;
      int clk_res = clk;
      for (int ii = clk; ii < clk_end; ii++) {
        if (!inputs_.AreAtClock(ii)) {
          break;
        }
        clk_res++;
      }
      if (clk_res > clk) {
        if (next_clock_.CompareAndSwap(clk, clk_res)) {
          while (queued_clock_.Load() < clk) continue;
          for (int ii = clk; ii < clk_res; ii++) {
            const int idx = ii % Slices;
            action_[idx] = Action(this, ii);
            sched_->Enqueue(queue_id_, action_[idx]);
          }
          queued_clock_.Store(clk_res);
          retry = false;
        }
      } else {
        retry = false;
      }
    }
  }

  virtual void OnInit(InitData * init_data) {
    Init(init_data);
  }

 private:
  InputsType inputs_;
  ExecutorType executor_;
  Action action_[Slices];
  ClockListener * listener_;
  embb::base::Atomic<int> next_clock_;
  embb::base::Atomic<int> queued_clock_;
  int queue_id_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SINK_H_
