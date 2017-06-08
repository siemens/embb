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

#ifndef EMBB_DATAFLOW_INTERNAL_INPUTS_H_
#define EMBB_DATAFLOW_INTERNAL_INPUTS_H_

#include <embb/base/atomic.h>
#include <embb/dataflow/internal/tuple.h>
#include <embb/dataflow/internal/in.h>

namespace embb {
namespace dataflow {
namespace internal {

template <
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil>
class Inputs;

template <>
class Inputs<embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil>
  : public Tuple<embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil>
  , public ClockListener {
 public:
  void SetListener(ClockListener * /*notify*/) {
    // emtpy
  }

  bool AreNoneBlank(int /*clock*/) const {
    return false;
  }

  bool AreAtClock(int /*clock*/) const {
    return true;
  }

  virtual void OnClock(int /*clock*/) {
    // emtpy
  }

  bool IsFullyConnected() const {
    return true;
  }

  void SetSlices(int /*slices*/) {
    // emtpy
  }
};

template <typename T1>
class Inputs<T1, embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil>
  : public Tuple<In<T1>, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() : count_(NULL), slices_(0), listener_(NULL) {
    // empty
  }

  void SetSlices(int slices) {
    if (0 < slices_) {
      embb::base::Allocation::Free(count_);
      count_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      count_ = reinterpret_cast<embb::base::Atomic<int>*>(
        embb::base::Allocation::Allocate(
          sizeof(embb::base::Atomic<int>)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        new(count_+ii) embb::base::Atomic<int>(1);
      }
    }
    this->template Get<0>().SetSlices(slices_);
  }

  ~Inputs() {
    if (NULL != count_) {
      for (int ii = 0; ii < slices_; ii++) {
        count_[ii].~Atomic<int>();
      }
      embb::base::Allocation::Free(count_);
    }
  }

  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
  }

  bool AreNoneBlank(int clock) const {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank());
  }

  bool AreAtClock(int clock) const {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock);
  }

  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
  }

  virtual void OnClock(int clock) {
    const int idx = clock % slices_;
    assert(count_[idx] > 0);
    if (--count_[idx] == 0) {
      count_[idx] = 1;
      listener_->OnClock(clock);
    }
  }

  virtual bool OnHasCycle(ClockListener const * node) const {
    return listener_->OnHasCycle(node);
  }

  bool IsFullyConnected() const {
    return this->template Get<0>().IsConnected();
  }
 private:
  embb::base::Atomic<int> * count_;
  int slices_;
  ClockListener * listener_;
};

template <typename T1, typename T2>
class Inputs<T1, T2, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil>
  : public Tuple<In<T1>, In<T2>, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() : count_(NULL), slices_(0), listener_(NULL) {
    // empty
  }

  void SetSlices(int slices) {
    if (0 < slices_) {
      embb::base::Allocation::Free(count_);
      count_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      count_ = reinterpret_cast<embb::base::Atomic<int>*>(
        embb::base::Allocation::Allocate(
          sizeof(embb::base::Atomic<int>)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        new(count_ + ii) embb::base::Atomic<int>(2);
      }
    }
    this->template Get<0>().SetSlices(slices_);
    this->template Get<1>().SetSlices(slices_);
  }

  ~Inputs() {
    if (NULL != count_) {
      for (int ii = 0; ii < slices_; ii++) {
        count_[ii].~Atomic<int>();
      }
      embb::base::Allocation::Free(count_);
    }
  }

  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
  }

  bool AreNoneBlank(int clock) const {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank());
  }

  bool AreAtClock(int clock) const {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<1>().GetSignal(clock).GetClock() == clock);
  }

  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
    this->template Get<1>().Clear(clock);
  }

  virtual void OnClock(int clock) {
    const int idx = clock % slices_;
    assert(count_[idx] > 0);
    if (--count_[idx] == 0) {
      count_[idx] = 2;
      listener_->OnClock(clock);
    }
  }

  virtual bool OnHasCycle(ClockListener const * node) const {
    return listener_->OnHasCycle(node);
  }

  bool IsFullyConnected() const {
    return this->template Get<0>().IsConnected() &
      this->template Get<1>().IsConnected();
  }

 private:
  embb::base::Atomic<int> * count_;
  int slices_;
  ClockListener * listener_;
};

template <typename T1, typename T2, typename T3>
class Inputs<T1, T2, T3, embb::base::internal::Nil,
  embb::base::internal::Nil>
  : public Tuple<In<T1>, In<T2>, In<T3>,
    embb::base::internal::Nil, embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() : count_(NULL), slices_(0), listener_(NULL) {
    // empty
  }

  void SetSlices(int slices) {
    if (0 < slices_) {
      embb::base::Allocation::Free(count_);
      count_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      count_ = reinterpret_cast<embb::base::Atomic<int>*>(
        embb::base::Allocation::Allocate(
          sizeof(embb::base::Atomic<int>)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        new(count_ + ii) embb::base::Atomic<int>(3);
      }
    }
    this->template Get<0>().SetSlices(slices_);
    this->template Get<1>().SetSlices(slices_);
    this->template Get<2>().SetSlices(slices_);
  }

  ~Inputs() {
    if (NULL != count_) {
      for (int ii = 0; ii < slices_; ii++) {
        count_[ii].~Atomic<int>();
      }
      embb::base::Allocation::Free(count_);
    }
  }

  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
    this->template Get<2>().SetListener(this);
  }

  bool AreNoneBlank(int clock) const {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank() ||
      this->template Get<2>().GetSignal(clock).IsBlank());
  }

  bool AreAtClock(int clock) const {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<1>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<2>().GetSignal(clock).GetClock() == clock);
  }

  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
    this->template Get<1>().Clear(clock);
    this->template Get<2>().Clear(clock);
  }

  virtual void OnClock(int clock) {
    const int idx = clock % slices_;
    assert(count_[idx] > 0);
    if (--count_[idx] == 0) {
      count_[idx] = 3;
      listener_->OnClock(clock);
    }
  }

  virtual bool OnHasCycle(ClockListener const * node) const {
    return listener_->OnHasCycle(node);
  }

  bool IsFullyConnected() const {
    return this->template Get<0>().IsConnected() &
      this->template Get<1>().IsConnected() &
      this->template Get<2>().IsConnected();
  }

 private:
  embb::base::Atomic<int> * count_;
  int slices_;
  ClockListener * listener_;
};

template <typename T1, typename T2, typename T3, typename T4>
class Inputs<T1, T2, T3, T4, embb::base::internal::Nil>
  : public Tuple<In<T1>, In<T2>, In<T3>,
      In<T4>, embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() : count_(NULL), slices_(0), listener_(NULL) {
    // empty
  }

  void SetSlices(int slices) {
    if (0 < slices_) {
      embb::base::Allocation::Free(count_);
      count_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      count_ = reinterpret_cast<embb::base::Atomic<int>*>(
        embb::base::Allocation::Allocate(
          sizeof(embb::base::Atomic<int>)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        new(count_ + ii) embb::base::Atomic<int>(4);
      }
    }
    this->template Get<0>().SetSlices(slices_);
    this->template Get<1>().SetSlices(slices_);
    this->template Get<2>().SetSlices(slices_);
    this->template Get<3>().SetSlices(slices_);
  }

  ~Inputs() {
    if (NULL != count_) {
      for (int ii = 0; ii < slices_; ii++) {
        count_[ii].~Atomic<int>();
      }
      embb::base::Allocation::Free(count_);
    }
  }

  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
    this->template Get<2>().SetListener(this);
    this->template Get<3>().SetListener(this);
  }

  bool AreNoneBlank(int clock) const {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank() ||
      this->template Get<2>().GetSignal(clock).IsBlank() ||
      this->template Get<3>().GetSignal(clock).IsBlank());
  }

  bool AreAtClock(int clock) const {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<1>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<2>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<3>().GetSignal(clock).GetClock() == clock);
  }

  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
    this->template Get<1>().Clear(clock);
    this->template Get<2>().Clear(clock);
    this->template Get<3>().Clear(clock);
  }

  virtual void OnClock(int clock) {
    const int idx = clock % slices_;
    assert(count_[idx] > 0);
    if (--count_[idx] == 0) {
      count_[idx] = 4;
      listener_->OnClock(clock);
    }
  }

  virtual bool OnHasCycle(ClockListener const * node) const {
    return listener_->OnHasCycle(node);
  }

  bool IsFullyConnected() const {
    return this->template Get<0>().IsConnected() &
      this->template Get<1>().IsConnected() &
      this->template Get<2>().IsConnected() &
      this->template Get<3>().IsConnected();
  }

 private:
  embb::base::Atomic<int> * count_;
  int slices_;
  ClockListener * listener_;
};

template <typename T1, typename T2, typename T3, typename T4,
  typename T5>
class Inputs
  : public Tuple<In<T1>, In<T2>, In<T3>,
      In<T4>, In<T5> >
  , public ClockListener {
 public:
  Inputs() : count_(NULL), slices_(0), listener_(NULL) {
    // empty
  }

  void SetSlices(int slices) {
    if (0 < slices_) {
      embb::base::Allocation::Free(count_);
      count_ = NULL;
    }
    slices_ = slices;
    if (0 < slices_) {
      count_ = reinterpret_cast<embb::base::Atomic<int>*>(
        embb::base::Allocation::Allocate(
          sizeof(embb::base::Atomic<int>)*slices_));
      for (int ii = 0; ii < slices_; ii++) {
        new(count_ + ii) embb::base::Atomic<int>(5);
      }
    }
    this->template Get<0>().SetSlices(slices_);
    this->template Get<1>().SetSlices(slices_);
    this->template Get<2>().SetSlices(slices_);
    this->template Get<3>().SetSlices(slices_);
    this->template Get<4>().SetSlices(slices_);
  }

  ~Inputs() {
    if (NULL != count_) {
      for (int ii = 0; ii < slices_; ii++) {
        count_[ii].~Atomic<int>();
      }
      embb::base::Allocation::Free(count_);
    }
  }

  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
    this->template Get<2>().SetListener(this);
    this->template Get<3>().SetListener(this);
    this->template Get<4>().SetListener(this);
  }

  bool AreNoneBlank(int clock) const {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank() ||
      this->template Get<2>().GetSignal(clock).IsBlank() ||
      this->template Get<3>().GetSignal(clock).IsBlank() ||
      this->template Get<4>().GetSignal(clock).IsBlank());
  }

  bool AreAtClock(int clock) const {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<1>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<2>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<3>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<4>().GetSignal(clock).GetClock() == clock);
  }

  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
    this->template Get<1>().Clear(clock);
    this->template Get<2>().Clear(clock);
    this->template Get<3>().Clear(clock);
    this->template Get<4>().Clear(clock);
  }

  virtual void OnClock(int clock) {
    const int idx = clock % slices_;
    assert(count_[idx] > 0);
    if (--count_[idx] == 0) {
      count_[idx] = 5;
      listener_->OnClock(clock);
    }
  }

  virtual bool OnHasCycle(ClockListener const * node) const {
    return listener_->OnHasCycle(node);
  }

  bool IsFullyConnected() const {
    return this->template Get<0>().IsConnected() &&
      this->template Get<1>().IsConnected() &
      this->template Get<2>().IsConnected() &
      this->template Get<3>().IsConnected() &
      this->template Get<4>().IsConnected();
  }

 private:
  embb::base::Atomic<int> * count_;
  int slices_;
  ClockListener * listener_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_INPUTS_H_
