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

#ifndef EMBB_BASE_INTERNAL_DURATION_INL_H_
#define EMBB_BASE_INTERNAL_DURATION_INL_H_

#include <embb/base/internal/config.h>

namespace embb {
namespace base {

template<typename Tick>
const Duration<Tick>& Duration<Tick>::Zero() {
  static Duration<Tick> zero;
  return zero;
}

#ifdef EMBB_PLATFORM_COMPILER_MSVC
// Suppress non-thread-safe static initialization warning
// in Max() and Min()
#pragma warning(push)
#pragma warning(disable : 4640)
#endif

template<typename Tick>
const Duration<Tick>& Duration<Tick>::Max() {
  static Duration<Tick> maximum(Tick::Max());
  return maximum;
}

template<typename Tick>
const Duration<Tick>& Duration<Tick>::Min() {
  static Duration<Tick> minimum(Tick::Min());
  return minimum;
}

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop) // Reset warning 4640
#endif

template<typename Tick>
Duration<Tick>::Duration() /*: rep_(EMBB_DURATION_INIT)*/ {
  rep_.nanoseconds = 0;
  rep_.seconds = 0;
}

template<typename Tick>
Duration<Tick>::Duration(unsigned long long ticks) {
  /*: rep_(EMBB_DURATION_INIT) << does not work with vs2012*/
  rep_.nanoseconds = 0;
  rep_.seconds = 0;
  Tick::SetAndCheck(rep_, ticks);
}

template<typename Tick>
Duration<Tick>::Duration(const Duration<Tick>& to_copy) {
  /*: rep_(EMBB_DURATION_INIT)*/
  rep_.nanoseconds = 0;
  rep_.seconds = 0;
  Tick::SetAndCheck(rep_, Tick::Get(to_copy.rep_));
}

template<typename Tick>
Duration<Tick>& Duration<Tick>::operator=(const Duration<Tick>& to_assign) {
  Tick::SetAndCheck(rep_, Tick::Get(to_assign.rep_));
  return *this;
}

template<typename Tick>
unsigned long long Duration<Tick>::Count() const {
  return Tick::Get(rep_);
}

template<typename Tick>
Duration<Tick>& Duration<Tick>::operator+=(const Duration<Tick>& rhs) {
  int status = embb_duration_add(&rep_, &(rhs.rep_));
  Tick::CheckExceptions(status, "Add-assign duration");
  return *this;
}

template<typename Tick>
Duration<Tick>::Duration(const embb_duration_t& duration) : rep_() {
  int status = Tick::Set(rep_, 0);
  assert(status == EMBB_SUCCESS);
  status = embb_duration_add(&rep_, &duration);
  assert(status == EMBB_SUCCESS);
}

} // namespace base
} // namespace embb

#endif  // EMBB_BASE_INTERNAL_DURATION_INL_H_
