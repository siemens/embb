/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#include <atomic_test.h>
#include <embb/base/c/atomic.h>

namespace embb {
namespace base {
namespace test {

AtomicTest::AtomicTest() {
  CreateUnit("Initialization and destroy").Add(&AtomicTest::TestInitDestroy, this);
  CreateUnit("AndAssign").Add(&AtomicTest::TestAndAssign, this);
  CreateUnit("Load").Add(&AtomicTest::TestLoad, this);
}

void AtomicTest::TestInitDestroy() {
  embb_atomic_int var;
  embb_atomic_init_int(&var, 1);
  PT_EXPECT_EQ(var.internal_variable, 1);
#ifdef EMBB_THREADING_ANALYSIS_MODE
  PT_EXPECT_EQ(var.init_flag, EMBB_ATOMIC_INTERNAL_INITIALIZED_VALUE);
#endif

  embb_atomic_destroy_int(&var);
#ifdef EMBB_THREADING_ANALYSIS_MODE
  PT_EXPECT_NE(var.init_flag, EMBB_ATOMIC_INTERNAL_INITIALIZED_VALUE);
#endif
}

void AtomicTest::TestAndAssign() {
  embb_atomic_int var;
  embb_atomic_init_int(&var, 1);
  embb_atomic_and_assign_int(&var, 2);
  PT_EXPECT_EQ(var.internal_variable, 3);
  embb_atomic_destroy_int(&var);
}

void AtomicTest::TestLoad() {
  embb_atomic_int var;
  embb_atomic_init_int(&var, 1);
  int value = embb_atomic_load_int(&var);
  PT_EXPECT_EQ(value, 1);
}

} // namespace test
} // namespace base
} // namespace embb
