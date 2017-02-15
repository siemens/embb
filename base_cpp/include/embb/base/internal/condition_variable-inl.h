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

#ifndef EMBB_BASE_INTERNAL_CONDITION_VARIABLE_INL_H_
#define EMBB_BASE_INTERNAL_CONDITION_VARIABLE_INL_H_

namespace embb {
namespace base {

#include <embb/base/c/condition_variable.h>

template<typename Tick>
bool ConditionVariable::WaitFor(UniqueLock<Mutex>& lock,
                                const Duration<Tick>& duration) {
  int status = embb_condition_wait_for(&condition_var_, &(lock.mutex_->mutex_),
                                       &(duration.rep_));
  if (status == EMBB_ERROR) {
    EMBB_THROW(ErrorException, "Error in ConditionVariable::WaitFor");
  }
  if (status == EMBB_TIMEDOUT) return false;
  return true;
}

} // namespace base
} // namespace embb

#endif // EMBB_BASE_INTERNAL_CONDITION_VARIABLE_INL_H_
