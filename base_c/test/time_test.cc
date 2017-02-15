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

#include <time_test.h>
#include <embb/base/c/duration.h>
#include <embb/base/c/time.h>
#include <embb/base/c/errors.h>

namespace embb {
namespace base {
namespace test {


TimeTest::TimeTest() {
  CreateUnit("Time in duration").Add(&TimeTest::TestTimeInDuration, this);
  CreateUnit("Monotonicity").Add(
      &TimeTest::TestMonotonicity, this,
      1, partest::TestSuite::GetDefaultNumIterations() * 10);
}

void TimeTest::TestTimeInDuration() {
  embb_time_t time;

  int status = embb_time_in(&time, embb_duration_min());
  PT_EXPECT_EQ(status, EMBB_SUCCESS);

  status = embb_time_in(&time, embb_duration_max());
  PT_EXPECT_EQ(status, EMBB_SUCCESS);
}

void TimeTest::TestMonotonicity() {
  embb_time_t first;
  embb_time_t second;
  int status1 = embb_time_in(&first, embb_duration_zero());
  int status2 = embb_time_in(&second, embb_duration_zero());
  PT_EXPECT_EQ(status1, EMBB_SUCCESS);
  PT_EXPECT_EQ(status2, EMBB_SUCCESS);
  unsigned long long first_abs = first.seconds * 1000 +
                                 first.nanoseconds / 1000000;
  unsigned long long second_abs = second.seconds * 1000 +
                                  second.nanoseconds / 1000000;
  PT_EXPECT_GE(second_abs, first_abs);
}

} // namespace test
} // namespace base
} // namespace embb
