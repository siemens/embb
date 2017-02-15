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

#include <core_set_test.h>
#include <embb/base/core_set.h>

namespace embb {
namespace base {
namespace test {

CoreSetTest::CoreSetTest() {
  CreateUnit("Test all").Add(&CoreSetTest::Test, this);
}

void CoreSetTest::Test() {
  unsigned int cores = CoreSet::CountAvailable();
  PT_EXPECT_GT(cores, static_cast<unsigned int>(0));
  { // Default construction
    CoreSet set;
    PT_EXPECT_EQ(set.Count(), (size_t)0);
  }
  { // One argument construction with no cores set
    CoreSet set(false);
    PT_EXPECT_EQ(set.Count(), (size_t)0);
  }
  { // One argument construction with all cores set
    CoreSet set(true);
    PT_EXPECT_EQ(set.Count(), cores);
  }
  { // Copy construction
    CoreSet set(true);
    CoreSet copy(set);
    PT_EXPECT_EQ(copy.Count(), set.Count());
  }
  { // Assignment
    CoreSet set(true);
    CoreSet assigned = set;
    PT_EXPECT_EQ(assigned.Count(), set.Count());
  }
  { // Resetting all cores
    CoreSet set;
    set.Reset(true);
    PT_EXPECT_EQ(set.Count(), cores);
    set.Reset(false);
    PT_EXPECT_EQ(set.Count(), (size_t)0);
  }
  { // Adding, removing, and checking single cores
    CoreSet set;
    for (unsigned int i = 0; i < cores; i++) {
      set.Add(i);
      bool is_contained = set.IsContained(i);
      PT_EXPECT_EQ(is_contained, true);
      if (i < cores - 1) {
        is_contained = set.IsContained(i+1);
        PT_EXPECT_EQ(is_contained, false);
      }
      PT_EXPECT_EQ(set.Count(), i+1);
    }
    for (unsigned int i = 0; i < cores; i++) {
      set.Remove(i);
      bool is_contained = set.IsContained(i);
      PT_EXPECT_EQ(is_contained, false);
      PT_EXPECT_EQ(set.Count(), cores - i - 1);
    }
  }
  { // Logical operators
    CoreSet lhs(false);
    CoreSet rhs(true);
    PT_EXPECT_EQ((lhs & rhs).Count(), (size_t)0);
    PT_EXPECT_EQ((lhs | rhs).Count(), cores);
    lhs &= rhs;
    PT_EXPECT_EQ(lhs.Count(), (size_t)0);
    lhs |= rhs;
    PT_EXPECT_EQ(lhs.Count(), rhs.Count());
  }
}

} // namespace test
} // namespace base
} // namespace embb



