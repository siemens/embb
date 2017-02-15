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

#ifndef EMBB_DATAFLOW_INTERNAL_SIGNAL_H_
#define EMBB_DATAFLOW_INTERNAL_SIGNAL_H_

#include <embb/base/c/atomic.h>

namespace embb {
namespace dataflow {
namespace internal {

template <typename Type>
class Signal {
 public:
  Signal() : blank_(true), value_(), clock_(-1) {}
  Signal(int clock, Type const & value)
    : blank_(false), value_(value), clock_(clock) {}
  explicit Signal(int clock) : blank_(true), value_(), clock_(clock) {}
  Signal(Signal const & other)
    : blank_(other.blank_), value_(other.value_), clock_(other.clock_) {}
  void operator = (Signal const & rhs) {
    blank_ = rhs.blank_;
    value_ = rhs.value_;
    clock_ = rhs.clock_;
    embb_atomic_memory_barrier();
  }
  int GetClock() const { return clock_; }
  bool IsBlank() const { return blank_; }
  Type const & GetValue() const { return value_; }
  void Clear() {
    blank_ = true;
    clock_ = -1;
  }

 private:
  bool blank_;
  Type value_;
  int clock_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SIGNAL_H_
