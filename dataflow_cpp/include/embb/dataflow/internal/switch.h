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

#ifndef EMBB_DATAFLOW_INTERNAL_SWITCH_H_
#define EMBB_DATAFLOW_INTERNAL_SWITCH_H_

#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/node.h>
#include <embb/dataflow/internal/inputs.h>
#include <embb/dataflow/internal/outputs.h>

namespace embb {
namespace dataflow {
namespace internal {

template <typename Type>
class Switch
  : public Node
  , public ClockListener {
 public:
  typedef Inputs<bool, Type> InputsType;
  typedef Outputs<Type, Type> OutputsType;

  explicit Switch(Scheduler * sched) : inputs_() {
    inputs_.SetListener(this);
    SetScheduler(sched);
  }

  virtual bool HasInputs() const {
    return inputs_.Size() > 0;
  }

  virtual bool HasOutputs() const {
    return outputs_.Size() > 0;
  }

  virtual void Run(int clock) {
    if (inputs_.AreNoneBlank(clock)) {
      bool pred = GetInput<0>().GetValue(clock);
      Type val = GetInput<1>().GetValue(clock);
      if (pred) {
        // signal with value
        GetOutput<0>().Send(Signal<Type>(clock, val));
        // blank signal
        GetOutput<1>().Send(Signal<Type>(clock));
      } else {
        // blank signal
        GetOutput<0>().Send(Signal<Type>(clock));
        // signal with value
        GetOutput<1>().Send(Signal<Type>(clock, val));
      }
    } else {
      GetOutput<0>().Send(Signal<Type>(clock));
      GetOutput<1>().Send(Signal<Type>(clock));
    }
  }

  virtual bool IsFullyConnected() const {
    return inputs_.IsFullyConnected() && outputs_.IsFullyConnected();
  }

  virtual bool OnHasCycle(ClockListener const * node) const {
    ClockListener const * this_node = this;
    if (this_node == node) {
      return true;
    } else {
      return outputs_.HasCycle(node);
    }
  }

  InputsType & GetInputs() {
    return inputs_;
  }

  template <int Index>
  typename TypeAt<typename InputsType::Types, Index>::Result & GetInput() {
    return inputs_.template Get<Index>();
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

  virtual void OnClock(int clock) {
    //const int idx = clock % Slices;
    assert(inputs_.AreAtClock(clock));
    Run(clock);
  }

 private:
  InputsType inputs_;
  OutputsType outputs_;

  virtual void SetSlices(int slices) {
    inputs_.SetSlices(slices);
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SWITCH_H_
