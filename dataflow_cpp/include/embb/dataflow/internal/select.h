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

#ifndef EMBB_DATAFLOW_INTERNAL_SELECT_H_
#define EMBB_DATAFLOW_INTERNAL_SELECT_H_

#include <embb/dataflow/internal/action.h>
#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/node.h>
#include <embb/dataflow/internal/inputs.h>
#include <embb/dataflow/internal/outputs.h>

namespace embb {
namespace dataflow {
namespace internal {

template <int Slices, typename Type>
class Select
  : public Node
  , public ClockListener {
 public:
  typedef Inputs<Slices, bool, Type, Type> InputsType;
  typedef Outputs<Slices, Type> OutputsType;

  Select() {
    inputs_.SetListener(this);
  }

  virtual bool HasInputs() const {
    return inputs_.Size() > 0;
  }

  virtual bool HasOutputs() const {
    return outputs_.Size() > 0;
  }

  virtual void Run(int clock) {
    if (GetInput<0>().GetSignal(clock).IsBlank()) {
      GetOutput<0>().Send(Signal<Type>(clock));
    } else {
      bool pred = GetInput<0>().GetValue(clock);
      Type val;
      if (pred) {
        if (GetInput<1>().GetSignal(clock).IsBlank()) {
          GetOutput<0>().Send(Signal<Type>(clock));
        } else {
          val = GetInput<1>().GetValue(clock);
          GetOutput<0>().Send(Signal<Type>(clock, val));
        }
      } else {
        if (GetInput<2>().GetSignal(clock).IsBlank()) {
          GetOutput<0>().Send(Signal<Type>(clock));
        } else {
          val = GetInput<2>().GetValue(clock);
          GetOutput<0>().Send(Signal<Type>(clock, val));
        }
      }
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

  virtual void OnClock(int clock) {
    //const int idx = clock % Slices;
    if (!inputs_.AreAtClock(clock))
      EMBB_THROW(embb::base::ErrorException,
        "Some inputs are not at expected clock.")
    Run(clock);
  }

 private:
  InputsType inputs_;
  OutputsType outputs_;
  Action action_[Slices];
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SELECT_H_
