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

#ifndef BASE_CPP_TEST_DURATION_TEST_H_
#define BASE_CPP_TEST_DURATION_TEST_H_

#include <partest/partest.h>
#include <embb/base/duration.h>

namespace embb {
namespace base {
namespace test {

class DurationTest : public partest::TestCase {
 public:
  /**
   * Adds test methods.
   */
  DurationTest();

 private:
  /**
   * Test second ticks.
   */
  template<typename Tick>
  void Test();
};

template<typename Tick>
void DurationTest::Test() {
  Duration<Tick> duration;
  PT_EXPECT_EQ(duration.Count(), (size_t)0);

  unsigned long long min_ticks = Duration<Tick>::Min().Count();

  duration += Duration<Tick>(min_ticks);
  PT_EXPECT_EQ(duration.Count(), min_ticks);

  duration += duration + duration;
  PT_EXPECT_EQ(duration.Count(), min_ticks * 3);

  Duration<Tick> duration_copy(duration);
  PT_EXPECT_EQ(duration_copy.Count(), min_ticks * 3);

  Duration<Tick> duration_assigned = duration;
  PT_EXPECT_EQ(duration_assigned.Count(), min_ticks * 3);
}

} // namespace test
} // namespace base
} // namespace embb


#endif // BASE_CPP_TEST_DURATION_TEST_H_
