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

#include <duration_test.h>
#include <embb/base/c/duration.h>
#include <embb/base/c/errors.h>
#include <limits.h>

namespace embb {
namespace base {
namespace test {

DurationTest::DurationTest() {
  CreateUnit("Compare durations").Add(&DurationTest::TestCompare, this);
  CreateUnit("Set zero durations").Add(&DurationTest::TestSetZero, this);
  CreateUnit("Set and get").Add(&DurationTest::TestSetAndGet, this);
  CreateUnit("Add").Add(&DurationTest::TestAdd, this);
}

void DurationTest::TestCompare() {
  embb_duration_t lhs = EMBB_DURATION_INIT;
  embb_duration_t rhs = EMBB_DURATION_INIT;

  int result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, 0);

  lhs.seconds = 1;
  lhs.nanoseconds = 0;
  rhs.seconds = 0;
  rhs.nanoseconds = 0;
  result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, 1);

  lhs.seconds = 0;
  lhs.nanoseconds = 0;
  rhs.seconds = 1;
  rhs.nanoseconds = 0;
  result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, -1);

  lhs.seconds = 0;
  lhs.nanoseconds = 1;
  rhs.seconds = 0;
  rhs.nanoseconds = 0;
  result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, 1);

  lhs.seconds = 0;
  lhs.nanoseconds = 0;
  rhs.seconds = 0;
  rhs.nanoseconds = 1;
  result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, -1);

  lhs.seconds = 1;
  lhs.nanoseconds = 0;
  rhs.seconds = 1;
  rhs.nanoseconds = 0;
  result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, 0);

  lhs.seconds = 0;
  lhs.nanoseconds = 1;
  rhs.seconds = 0;
  rhs.nanoseconds = 1;
  result = embb_duration_compare(&lhs, &rhs);
  PT_EXPECT_EQ(result, 0);
}

void DurationTest::TestSetZero() {
  PT_EXPECT_EQ(embb_duration_zero()->seconds,
      static_cast<unsigned long long>(0));
  PT_EXPECT_EQ(embb_duration_zero()->nanoseconds,
      static_cast<unsigned long>(0));

  embb_duration_t duration;
  duration.seconds = 1;
  duration.nanoseconds = 1;
  embb_duration_set_seconds(&duration, 0);
  int result = embb_duration_compare(&duration, embb_duration_zero());
  PT_EXPECT_EQ(result, 0);

  duration.seconds = 1;
  duration.nanoseconds = 1;
  embb_duration_set_milliseconds(&duration, 0);
  result = embb_duration_compare(&duration, embb_duration_zero());
  PT_EXPECT_EQ(result, 0);

  duration.seconds = 1;
  duration.nanoseconds = 1;
  embb_duration_set_microseconds(&duration, 0);
  result = embb_duration_compare(&duration, embb_duration_zero());
  PT_EXPECT_EQ(result, 0);

  duration.seconds = 1;
  duration.nanoseconds = 1;
  embb_duration_set_nanoseconds(&duration, 0);
  result = embb_duration_compare(&duration, embb_duration_zero());
  PT_EXPECT_EQ(result, 0);
}

void DurationTest::TestSetAndGet() {
  // No underflows are detected here, to be done

  embb_duration_t duration = EMBB_DURATION_INIT;

  unsigned long long ticks = 0;
  unsigned long long min_ticks = 0;
  embb_duration_as_nanoseconds(embb_duration_min(), &min_ticks);
  int result = embb_duration_set_nanoseconds(&duration, min_ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  result = embb_duration_as_nanoseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, min_ticks);
  result = embb_duration_as_microseconds(&duration, &ticks);
  if (min_ticks / 1000 == 0 || min_ticks % 1000 != 0) {
    PT_EXPECT_EQ(result, EMBB_UNDERFLOW);
  } else {
    PT_EXPECT_EQ(ticks, min_ticks / 1000);
  }
  result = embb_duration_as_milliseconds(&duration, &ticks);
  if (min_ticks / 1000000 == 0 || min_ticks % 1000000 != 0) {
    PT_EXPECT_EQ(result, EMBB_UNDERFLOW);
  } else {
    PT_EXPECT_EQ(ticks, min_ticks / 1000000);
  }
  result = embb_duration_as_seconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_UNDERFLOW);

  ticks = 0;
  embb_duration_as_microseconds(embb_duration_min(), &min_ticks);
  result = embb_duration_set_microseconds(&duration, min_ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  result = embb_duration_as_nanoseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, min_ticks*1000);
  result = embb_duration_as_microseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, min_ticks);
  result = embb_duration_as_milliseconds(&duration, &ticks);
  if (min_ticks / 1000 == 0 || min_ticks % 1000 != 0) {
    PT_EXPECT_EQ(result, EMBB_UNDERFLOW);
  } else {
    PT_EXPECT_EQ(ticks, min_ticks / 1000);
  }
  result = embb_duration_as_seconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_UNDERFLOW);

  ticks = 0;
  embb_duration_as_milliseconds(embb_duration_min(), &min_ticks);
  result = embb_duration_set_milliseconds(&duration, min_ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  result = embb_duration_as_nanoseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, min_ticks*1000000);
  result = embb_duration_as_microseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, min_ticks*1000);
  result = embb_duration_as_milliseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, min_ticks);
  result = embb_duration_as_seconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_UNDERFLOW);

  ticks = 0;
  result = embb_duration_set_seconds(&duration, 1);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  result = embb_duration_as_nanoseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, static_cast<unsigned long long>(1000000000));
  result = embb_duration_as_microseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, static_cast<unsigned long long>(1000000));
  result = embb_duration_as_milliseconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, static_cast<unsigned long long>(1000));
  result = embb_duration_as_seconds(&duration, &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(ticks, static_cast<unsigned long long>(1));

  ticks = 0;
  result = embb_duration_as_nanoseconds(embb_duration_min(), &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_GT(ticks, static_cast<unsigned long long>(0));

  ticks = 0;
  result = embb_duration_as_seconds(embb_duration_max(), &ticks);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_NE(ticks, static_cast<unsigned long long>(0));

  embb_duration_as_seconds(embb_duration_max(), &ticks);
  ticks++;
  result = embb_duration_set_seconds(&duration, ticks);
  PT_EXPECT_EQ(result, EMBB_OVERFLOW);
}

void DurationTest::TestAdd() {
  embb_duration_t lhs = EMBB_DURATION_INIT;
  embb_duration_t rhs = EMBB_DURATION_INIT;

  int result = embb_duration_add(&lhs, &rhs);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(embb_duration_compare(&lhs, embb_duration_zero()), 0);

  result = embb_duration_set_milliseconds(&rhs, 1);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  result = embb_duration_add(&lhs, &rhs);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(embb_duration_compare(&lhs, &rhs), 0);

  embb_duration_set_seconds(&lhs, 0);
  embb_duration_set_milliseconds(&rhs, 1100);
  result = embb_duration_add(&lhs, &rhs);
  PT_EXPECT_EQ(result, EMBB_SUCCESS);
  PT_EXPECT_EQ(lhs.seconds, static_cast<unsigned long long>(1));
  PT_EXPECT_EQ(lhs.nanoseconds, static_cast<unsigned long>(100000000));
}

} // namespace test
} // namespace base
} // namespace embb



