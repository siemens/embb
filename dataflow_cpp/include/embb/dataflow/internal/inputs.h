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

#ifndef EMBB_DATAFLOW_INTERNAL_INPUTS_H_
#define EMBB_DATAFLOW_INTERNAL_INPUTS_H_

#include <embb/base/atomic.h>
#include <embb/dataflow/internal/tuple.h>
#include <embb/dataflow/internal/in.h>

namespace embb {
namespace dataflow {
namespace internal {

template <
  int,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil>
class Inputs;

template <int Slices>
class Inputs<Slices, embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil>
  : public Tuple<embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil>
  , public ClockListener {
 public:
  void SetListener(ClockListener * /*notify*/) {}
  bool AreNoneBlank(int /*clock*/) { return false; }
  bool AreAtClock(int /*clock*/) { return true; }
  virtual void OnClock(int /*clock*/) {}
  virtual void OnInit(InitData * /*init_data*/) {}
};

template <int Slices, typename T1>
class Inputs<Slices, T1, embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil>
  : public Tuple<In<T1, Slices>, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() {
    for (int ii = 0; ii < Slices; ii++)
      count_[ii] = 1;
    test_count_ = 1;
  }
  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
  }
  bool AreNoneBlank(int clock) {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank());
  }
  bool AreAtClock(int clock) {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock);
  }
  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
  }
  virtual void OnClock(int clock) {
    const int idx = clock % Slices;
    if (count_[idx] == 0) {
      EMBB_THROW(embb::base::ErrorException,
        "All inputs already fired for this clock.");
    }
    if (--count_[idx] == 0) {
      count_[idx] = 1;
      listener_->OnClock(clock);
    }
  }
  virtual void OnInit(InitData * init_data) {
    if (--test_count_ == 0) {
      listener_->OnInit(init_data);
    }
  }
 private:
  embb::base::Atomic<int> count_[Slices];
  int test_count_;
  ClockListener * listener_;
};

template <int Slices, typename T1, typename T2>
class Inputs<Slices, T1, T2, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil>
  : public Tuple<In<T1, Slices>, In<T2, Slices>, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() {
    for (int ii = 0; ii < Slices; ii++)
      count_[ii] = 2;
    test_count_ = 2;
  }
  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
  }
  bool AreNoneBlank(int clock) {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank());
  }
  bool AreAtClock(int clock) {
    return
      (this->template Get<0>().GetSignal(clock).GetClock() == clock) &&
      (this->template Get<1>().GetSignal(clock).GetClock() == clock);
  }
  void Clear(int clock) {
    this->template Get<0>().Clear(clock);
    this->template Get<1>().Clear(clock);
  }
  virtual void OnClock(int clock) {
    const int idx = clock % Slices;
    if (count_[idx] == 0) {
      EMBB_THROW(embb::base::ErrorException,
        "All inputs already fired for this clock.");
    }
    if (--count_[idx] == 0) {
      count_[idx] = 2;
      listener_->OnClock(clock);
    }
  }
  virtual void OnInit(InitData * init_data) {
    if (--test_count_ == 0) {
      listener_->OnInit(init_data);
    }
  }
 private:
  embb::base::Atomic<int> count_[Slices];
  int test_count_;
  ClockListener * listener_;
};

template <int Slices, typename T1, typename T2, typename T3>
class Inputs<Slices, T1, T2, T3, embb::base::internal::Nil,
  embb::base::internal::Nil>
  : public Tuple<In<T1, Slices>, In<T2, Slices>, In<T3, Slices>,
    embb::base::internal::Nil, embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() {
    for (int ii = 0; ii < Slices; ii++)
      count_[ii] = 3;
    test_count_ = 3;
  }
  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
    this->template Get<2>().SetListener(this);
  }
  bool AreNoneBlank(int clock) {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank() ||
      this->template Get<2>().GetSignal(clock).IsBlank());
  }
  bool AreAtClock(int clock) {
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
    const int idx = clock % Slices;
    if (count_[idx] == 0) {
      EMBB_THROW(embb::base::ErrorException,
        "All inputs already fired for this clock.");
    }
    if (--count_[idx] == 0) {
      count_[idx] = 3;
      listener_->OnClock(clock);
    }
  }
  virtual void OnInit(InitData * init_data) {
    if (--test_count_ == 0) {
      listener_->OnInit(init_data);
    }
  }
 private:
  embb::base::Atomic<int> count_[Slices];
  int test_count_;
  ClockListener * listener_;
};

template <int Slices, typename T1, typename T2, typename T3, typename T4>
class Inputs<Slices, T1, T2, T3, T4, embb::base::internal::Nil>
  : public Tuple<In<T1, Slices>, In<T2, Slices>, In<T3, Slices>,
      In<T4, Slices>, embb::base::internal::Nil>
  , public ClockListener {
 public:
  Inputs() {
    for (int ii = 0; ii < Slices; ii++)
      count_[ii] = 4;
    test_count_ = 4;
  }
  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
    this->template Get<2>().SetListener(this);
    this->template Get<3>().SetListener(this);
  }
  bool AreNoneBlank(int clock) {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank() ||
      this->template Get<2>().GetSignal(clock).IsBlank() ||
      this->template Get<3>().GetSignal(clock).IsBlank());
  }
  bool AreAtClock(int clock) {
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
    const int idx = clock % Slices;
    if (count_[idx] == 0) {
      EMBB_THROW(embb::base::ErrorException,
        "All inputs already fired for this clock.");
    }
    if (--count_[idx] == 0) {
      count_[idx] = 4;
      listener_->OnClock(clock);
    }
  }
  virtual void OnInit(InitData * init_data) {
    if (--test_count_ == 0) {
      listener_->OnInit(init_data);
    }
  }
 private:
  embb::base::Atomic<int> count_[Slices];
  int test_count_;
  ClockListener * listener_;
};

template <int Slices, typename T1, typename T2, typename T3, typename T4,
  typename T5>
class Inputs
  : public Tuple<In<T1, Slices>, In<T2, Slices>, In<T3, Slices>,
      In<T4, Slices>, In<T5, Slices> >
  , public ClockListener {
 public:
  Inputs() {
    for (int ii = 0; ii < Slices; ii++)
      count_[ii] = 5;
    test_count_ = 5;
  }
  void SetListener(ClockListener * listener) {
    listener_ = listener;
    this->template Get<0>().SetListener(this);
    this->template Get<1>().SetListener(this);
    this->template Get<2>().SetListener(this);
    this->template Get<3>().SetListener(this);
    this->template Get<4>().SetListener(this);
  }
  bool AreNoneBlank(int clock) {
    return !(
      this->template Get<0>().GetSignal(clock).IsBlank() ||
      this->template Get<1>().GetSignal(clock).IsBlank() ||
      this->template Get<2>().GetSignal(clock).IsBlank() ||
      this->template Get<3>().GetSignal(clock).IsBlank() ||
      this->template Get<4>().GetSignal(clock).IsBlank());
  }
  bool AreAtClock(int clock) {
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
    const int idx = clock % Slices;
    if (count_[idx] == 0) {
      EMBB_THROW(embb::base::ErrorException,
        "All inputs already fired for this clock.");
    }
    if (--count_[idx] == 0) {
      count_[idx] = 5;
      listener_->OnClock(clock);
    }
  }
  virtual void OnInit(InitData * init_data) {
    if (--test_count_ == 0) {
      listener_->OnInit(init_data);
    }
  }
 private:
  embb::base::Atomic<int> count_[Slices];
  int test_count_;
  ClockListener * listener_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_INPUTS_H_
