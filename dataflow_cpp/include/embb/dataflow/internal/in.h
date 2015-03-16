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

#ifndef EMBB_DATAFLOW_INTERNAL_IN_H_
#define EMBB_DATAFLOW_INTERNAL_IN_H_

#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
#include <vector>
#include <embb/dataflow/internal/spinlock.h>
#endif

#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/clock_listener.h>

namespace embb {
namespace dataflow {
namespace internal {

class Scheduler;

template <typename, int>
class Out;

template <typename Type, int Slices>
class In {
 public:
  typedef Signal<Type> SignalType;

  In() : connected_(false) {}

  SignalType const & GetSignal(int clock) const {
    return values_[clock % Slices];
  }

  Type GetValue(int clock) const {
    SignalType const & signal = GetSignal(clock);
    if (signal.IsBlank())
      EMBB_THROW(embb::base::ErrorException,
        "Signal is blank, cannot get a value.")
    return signal.GetValue();
  }

  bool IsConnected() const { return connected_; }
  void SetConnected() { connected_ = true; }

  void SetListener(ClockListener * listener) { listener_ = listener; }

  void Clear(int clock) {
    const int idx = clock % Slices;
    values_[idx].Clear();
  }

  friend class Out<Type, Slices>;

 private:
  SignalType values_[Slices];
  ClockListener * listener_;
  bool connected_;
#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
  SpinLock lock_;
  std::vector<SignalType> history_;
#endif

  void Receive(SignalType const & value) {
    const int idx = value.GetClock() % Slices;
    if (values_[idx].GetClock() >= value.GetClock())
      EMBB_THROW(embb::base::ErrorException,
        "Received signal does not increase clock.");
    values_[idx] = value;
    listener_->OnClock(value.GetClock());
#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
    lock_.Lock();
    history_.push_back(value);
    lock_.Unlock();
#endif
  }

  void ReceiveInit(InitData * init_data) {
    listener_->OnInit(init_data);
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_IN_H_
