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

#include <embb/base/c/internal/unused.h>
#include <embb/base/duration.h>
#include <embb/base/c/duration.h>
#include <embb/base/exceptions.h>
#include <embb/base/c/internal/platform.h>
#include <limits>
#include <cassert>

namespace embb {
namespace base {

void internal::Tick::CheckExceptions(int status, const char* msg) {
  switch (status) {
  case EMBB_SUCCESS: return;
  case EMBB_OVERFLOW: EMBB_THROW(OverflowException, msg);
  case EMBB_UNDERFLOW: EMBB_THROW(UnderflowException, msg);
  default: EMBB_THROW(ErrorException, msg);
  }
}

int internal::Seconds::Set(embb_duration_t& duration,
                           unsigned long long ticks) {
  return embb_duration_set_seconds(&duration, ticks);
}

void internal::Seconds::SetAndCheck(embb_duration_t& duration,
                                   unsigned long long ticks) {
  int status = Set(duration, ticks);
  CheckExceptions(status, "Setting duration from seconds");
}

unsigned long long internal::Seconds::Get(const embb_duration_t& duration) {
  unsigned long long ticks = 0;
  int status = embb_duration_as_seconds(&duration, &ticks);
  assert(status == EMBB_SUCCESS);
  EMBB_UNUSED_IN_RELEASE(status);
  return ticks;
}

unsigned long long internal::Seconds::Min() {
  return 1;
}

unsigned long long internal::Seconds::Max() {
  return EMBB_DURATION_MAX_SECONDS;
}

int internal::Milliseconds::Set(embb_duration_t& duration,
                                unsigned long long ticks) {
  return embb_duration_set_milliseconds(&duration, ticks);
}

void internal::Milliseconds::SetAndCheck(
  embb_duration_t& duration, unsigned long long ticks) {
  int status = Set(duration, ticks);
  CheckExceptions(status, "Setting duration from milliseconds");
}

unsigned long long internal::Milliseconds::Get(
    const embb_duration_t& duration) {
  unsigned long long ticks = 0;
  int status = embb_duration_as_milliseconds(&duration, &ticks);
  assert(status == EMBB_SUCCESS);
  EMBB_UNUSED_IN_RELEASE(status);
  return ticks;
}

unsigned long long internal::Milliseconds::Min() {
#if EMBB_DURATION_MIN_NANOSECONDS > 1000000
  assert(EMBB_DURATION_MIN_NANOSECONDS % 1000000 == 0);
  return EMBB_DURATION_MIN_NANOSECONDS / 1000000;
#endif
  return 1;
}

unsigned long long internal::Milliseconds::Max() {
#if EMBB_DURATION_MAX_SECONDS < ULLONG_MAX / 1000
  return ULLONG_MAX;
#else
  return EMBB_DURATION_MAX_SECONDS * 1000;
#endif
}

int internal::Microseconds::Set(embb_duration_t& duration,
                                unsigned long long ticks) {
  return embb_duration_set_microseconds(&duration, ticks);
}

void internal::Microseconds::SetAndCheck(
  embb_duration_t& duration, unsigned long long ticks) {
  int status = Set(duration, ticks);
  CheckExceptions(status, "Setting duration from microseconds");
}

unsigned long long internal::Microseconds::Get(
    const embb_duration_t& duration) {
  unsigned long long ticks = 0;

  int status = embb_duration_as_microseconds(&duration, &ticks);

  assert(status == EMBB_SUCCESS);
  EMBB_UNUSED_IN_RELEASE(status);
  return ticks;
}

unsigned long long internal::Microseconds::Min() {
#if EMBB_DURATION_MIN_NANOSECONDS > 1000
  assert(EMBB_DURATION_MIN_NANOSECONDS % 1000 == 0);
  return EMBB_DURATION_MIN_NANOSECONDS / 1000;
#endif
  return 1;
}

unsigned long long internal::Microseconds::Max() {
#if EMBB_DURATION_MAX_SECONDS < ULLONG_MAX / 1000000
  return ULLONG_MAX;
#else
  return EMBB_DURATION_MAX_SECONDS * 1000000ULL;
#endif
}

int internal::Nanoseconds::Set(embb_duration_t& duration,
                               unsigned long long ticks) {
  return embb_duration_set_nanoseconds(&duration, ticks);
}

void internal::Nanoseconds::SetAndCheck(
  embb_duration_t& duration, unsigned long long ticks) {
  int status = Set(duration, ticks);
  CheckExceptions(status, "Setting duration from microseconds");
}

unsigned long long internal::Nanoseconds::Get(const embb_duration_t& duration) {
  unsigned long long ticks = 0;
  int status = embb_duration_as_nanoseconds(&duration, &ticks);
  assert(status == EMBB_SUCCESS);
  EMBB_UNUSED_IN_RELEASE(status);
  return ticks;
}

unsigned long long internal::Nanoseconds::Min() {
  return EMBB_DURATION_MIN_NANOSECONDS;
}

unsigned long long internal::Nanoseconds::Max() {
#if EMBB_DURATION_MAX_SECONDS < ULLONG_MAX / 1000000000
  return ULLONG_MAX;
#else
  return EMBB_DURATION_MAX_SECONDS * 1000000000ULL;
#endif
}

} // namespace base
} // namespace embb
