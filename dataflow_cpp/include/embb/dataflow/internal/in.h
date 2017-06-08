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

#ifndef EMBB_DATAFLOW_INTERNAL_IN_H_
#define EMBB_DATAFLOW_INTERNAL_IN_H_

#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
#include <vector>
#include <embb/base/mutex.h>
#endif

#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/clock_listener.h>

namespace embb {
namespace dataflow {
namespace internal {

class Scheduler;

template <typename>
class Out;

template <typename Type>
class In {
 public:
  typedef Signal<Type> SignalType;

  In() : values_(NULL), connected_(false), slices_(0), listener_(NULL) {}

  ~In() {
    if (NULL != values_) {
      for (int ii = 0; ii < slices_; ii++) {
        values_[ii].~SignalType();
      }
      embb::base::Allocation::Free(values_);
    }
  }

  SignalType const & GetSignal(int clock) const {
    return values_[clock % slices_];
  }

  Type GetValue(int clock) const {
    SignalType const & signal = GetSignal(clock);
    assert(!signal.IsBlank());
    return signal.GetValue();
  }

  bool IsConnected() const { return connected_; }
  void SetConnected() { connected_ = true; }

  bool HasCycle(ClockListener const * node) const {
    return listener_->OnHasCycle(node);
  }

  void SetSlices(int slices) {
    if (0 < slices_) {
      for (int ii = 0; ii < slices_; ii++) {
        values_[ii].~SignalType();
      }
      embb::base::Allocation::Free(values_);
      values_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      values_ = reinterpret_cast<SignalType*>(
        embb::base::Allocation::Allocate(
          sizeof(SignalType)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        new (&values_[ii]) SignalType();
      }
    }
  }

  void SetListener(ClockListener * listener) { listener_ = listener; }

  void Clear(int clock) {
    const int idx = clock % slices_;
    values_[idx].Clear();
  }

  friend class Out<Type>;

 private:
  SignalType * values_;
  bool connected_;
  int slices_;
  ClockListener * listener_;
#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
  embb::base::Spinlock lock_;
  std::vector<SignalType> history_;
#endif

  void Receive(SignalType const & value) {
    const int idx = value.GetClock() % slices_;
    assert(values_[idx].GetClock() < value.GetClock());
    values_[idx] = value;
    listener_->OnClock(value.GetClock());
#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
    lock_.Lock();
    history_.push_back(value);
    lock_.Unlock();
#endif
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_IN_H_
