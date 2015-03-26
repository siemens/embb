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

#include <multiset_test.h>
// #include <embb/containers/multiset.h>

namespace embb {
namespace containers {
namespace test {

MultisetTest::MultisetTest() :
  num_threads_(static_cast<int>(
    partest::TestSuite::GetDefaultNumThreads())) {
  CreateUnit("SerialTest").Add(&MultisetTest::SerialTest, this);
}

void MultisetTest::SerialTest() {
#if 0
  Multiset<char, '-'> multiset(3);
  multiset.Insert('a', 5);
  multiset.Insert('b', 5);
  multiset.Insert('c', 5);
  PT_ASSERT(multiset.TryDelete('a', 4));
  PT_ASSERT(multiset.TryDelete('b', 4));
  PT_ASSERT(multiset.TryDelete('c', 4));
  PT_ASSERT(multiset.Get('a') == 1);
  PT_ASSERT(multiset.Get('b') == 1);
  PT_ASSERT(multiset.Get('c') == 1);
  PT_ASSERT(multiset.TryDelete('a', 1));
  PT_ASSERT(multiset.TryDelete('b', 1));
  PT_ASSERT(multiset.TryDelete('c', 1));
#endif
}

} // namespace test
} // namespace containers
} // namespace embb
