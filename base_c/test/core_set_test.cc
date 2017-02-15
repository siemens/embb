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
#include <embb/base/c/core_set.h>

namespace embb {
namespace base {
namespace test {

CoreSetTest::CoreSetTest() {
  CreateUnit("Test all").Add(&CoreSetTest::Test, this);
}

void CoreSetTest::Test() {
  embb_core_set_t set;
  unsigned int available_cores = embb_core_count_available();
  unsigned int zero_cores = 0;
  // Test setting all cores
  embb_core_set_init(&set, 0);
  unsigned int cores = embb_core_set_count(&set);
  PT_EXPECT_EQ(cores, zero_cores);
  embb_core_set_init(&set, 1);
  cores = embb_core_set_count(&set);
  PT_EXPECT_EQ(cores, available_cores);

  // Test setting individual cores
  embb_core_set_init(&set, 0);
  cores = embb_core_set_count(&set);
  PT_EXPECT_EQ(cores, zero_cores);
  for (unsigned int i = 0; i < available_cores; i++) {
    embb_core_set_add(&set, i);
    int is_set = embb_core_set_contains(&set, i);
    PT_EXPECT_EQ(is_set, 1);
    if (i < available_cores - 1) {
      is_set = embb_core_set_contains(&set, i+1);
      PT_EXPECT_EQ(is_set, 0);
    }
    cores = embb_core_set_count(&set);
    PT_EXPECT_EQ(cores, i+1);
  }

  // Test logical & and | operations
  embb_core_set_t set2;
  embb_core_set_init(&set, 0);
  embb_core_set_init(&set2, 1);
  embb_core_set_intersection(&set, &set2);
  cores = embb_core_set_count(&set);
  PT_EXPECT_EQ(cores, zero_cores);
  embb_core_set_union(&set, &set2);
  cores = embb_core_set_count(&set);
  PT_EXPECT_EQ(cores, available_cores);
}

} // namespace test
} // namespace base
} // namespace embb



