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

#ifndef EMBB_DATAFLOW_INTERNAL_OUT_H_
#define EMBB_DATAFLOW_INTERNAL_OUT_H_

#include <vector>
#include <embb/dataflow/internal/signal.h>
#include <embb/dataflow/internal/in.h>

namespace embb {
namespace dataflow {
namespace internal {

class Scheduler;
class ClockListener;

template <typename Type>
class Out {
 public:
  typedef Signal<Type> SignalType;
  typedef In<Type> InType;

  Out() {
  }

  void Send(SignalType const & value) {
    for (size_t ii = 0; ii < targets_.size(); ii++) {
      targets_[ii]->Receive(value);
    }
  }

  void Connect(InType & input) {
    if (input.IsConnected()) {
      EMBB_THROW(embb::base::ErrorException,
        "Input is already connected.")
    } else {
      input.SetConnected();
      targets_.push_back(&input);
    }
  }

  void operator >> (InType & input) {
    Connect(input);
  }

  bool IsConnected() const {
    return targets_.size() > 0;
  }

  bool HasCycle(ClockListener const * node) const {
    bool result = false;
    for (size_t ii = 0; ii < targets_.size() && !result; ii++) {
      result = result || targets_[ii]->HasCycle(node);
    }
    return result;
  }

 private:
  std::vector< InType * > targets_;
};


} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_OUT_H_
