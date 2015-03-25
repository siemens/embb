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

#include <llx_scx_test.h>
#include <embb/containers/internal/fixed_size_list.h>
#include <embb/containers/primitives/llx_scx.h>

namespace embb {
namespace containers {
namespace test {

using embb::containers::internal::FixedSizeList;
using embb::containers::primitives::LlxScxRecord;
using embb::containers::primitives::LlxScx;

LlxScxTest::LlxScxTest() :
  num_threads_(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())) {
  CreateUnit("SerialTest").Add(&LlxScxTest::SerialTest, this);
}

void LlxScxTest::SerialTest() {
  typedef LlxScxTest::Node Node;

  // Global:
  LlxScx<Node> llxscx(3);

  // Multiset { a, b, b, c }
  Node n1(1, 'a');
  Node n2(2, 'b');
  Node n3(1, 'c');
  
  // V = { dr1, dr2, dr3 }
  // R = { dr1, dr2 }
  LlxScxRecord<Node> dr1(n1);
  LlxScxRecord<Node> dr2(n2);
  LlxScxRecord<Node> dr3(n3);

  dr1->next_.Store(&dr2);
  dr2->next_.Store(&dr3);

  // Thread-local:
  LlxScxRecord<Node> l1, l2, l3;
  bool finalized;
  PT_ASSERT(llxscx.TryLoadLinked(&dr1, l1, finalized));
  PT_ASSERT(!finalized);
  PT_ASSERT(llxscx.TryLoadLinked(&dr2, l2, finalized));
  PT_ASSERT(!finalized);
  PT_ASSERT(llxscx.TryLoadLinked(&dr3, l3, finalized));
  PT_ASSERT(!finalized);

  FixedSizeList< LlxScxRecord<Node> * > 
    linked_deps(3);
  linked_deps.PushBack(&dr1);
  linked_deps.PushBack(&dr2);
  linked_deps.PushBack(&dr3);
  FixedSizeList< LlxScxRecord<Node> * >
    finalize_deps(2);
  finalize_deps.PushBack(&dr2);
  finalize_deps.PushBack(&dr3);

  typedef LlxScxRecord<Node> * field_t;
  
  LlxScxRecord<Node> new_node(n3);
  PT_ASSERT(
    llxscx.TryStoreConditional<LlxScxRecord<Node> *>(
      &n2.next_,     // fld: field to update
      &new_node,     // new value
      linked_deps,   // V: dependencies, must be LL'd before
      finalize_deps  // R: Subsequence of V to be finalized
    ));
}

} // namespace test
} // namespace containers
} // namespace embb
