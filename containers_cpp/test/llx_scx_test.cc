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
#include <embb/containers/internal/llx_scx.h>
// #include <embb/containers/multiword_ll_sc.h>

namespace embb {
namespace containers {
namespace test {

using embb::containers::internal::FixedSizeList;
using embb::containers::internal::LlxScxRecord;
using embb::containers::internal::LlxScx;

LlxScxTest::LlxScxTest() :
  num_threads_(
    static_cast<int>(partest::TestSuite::GetDefaultNumThreads())),
  llxscx_(3),
  tail(0, '-'),
  head(0, '-'),
  tail_llx(tail),
  head_llx(head) {
  CreateUnit("SerialArrayTest")
    .Add(&LlxScxTest::SerialArrayTest, this);
  CreateUnit("ParallelTest")
    .Pre(&LlxScxTest::ParallelTestPre, this)
    .Add(&LlxScxTest::ParallelTest, this,
         static_cast<unsigned int>(num_threads_), 1)
    .Post(&LlxScxTest::ParallelTestPost, this);
}

void LlxScxTest::ParallelTestPre() {
  embb_internal_thread_index_reset();
}

void LlxScxTest::ParallelTest() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);
  if (return_val != EMBB_SUCCESS)
    EMBB_THROW(embb::base::ErrorException, "Could not get thread id!"); 
  // Every thread adds every character from 'a' to 'z' into an ordered
  // linked list
  for (char value = 'a'; value <= 'z';) {
    // Find node to append new element on:
    internal::LlxScxRecord<Node> * node_rec = &head_llx;
    internal::LlxScxRecord<Node> * next_rec = node_rec->Data().next_;
    while (next_rec != 0 && next_rec->Data().value_ < value) {
      node_rec = next_rec;
      next_rec = next_rec->Data().next_;
    }
    Node node;
    bool finalized;
    // LLX on node the new element will be appended to:
    if (!llxscx_.TryLoadLinked(node_rec, node, finalized)) {
      continue;
    }
    PT_ASSERT_MSG(!finalized, "No node should be finalized");
    if (node.next_ == next_rec) {
      // Pointer still valid after LLX, try to append new node
      internal::FixedSizeList<LlxScxRecord<Node> *> linked_deps(1);
      internal::FixedSizeList<LlxScxRecord<Node> *> finalize_deps(0);
      linked_deps.PushBack(node_rec);
      // Create new node:
      Node new_node(static_cast<int>(thread_index), value, next_rec);
      internal::LlxScxRecord<Node> * new_node_ptr =
        new internal::LlxScxRecord<Node>(new_node);
      // Convert node pointer to size_t:
      size_t new_cas_value = reinterpret_cast<size_t>(new_node_ptr);
      // Convert target field pointer to size_t*:
      embb::base::Atomic<size_t> * field_cas_ptr =
        reinterpret_cast< embb::base::Atomic<size_t> * >(
          &(node_rec->Data().next_));
      // Call SCX:
      bool element_inserted =
        llxscx_.TryStoreConditional(
          field_cas_ptr,
          new_cas_value,
          linked_deps,
          finalize_deps);
      if (element_inserted) {
        // Value has been added to list, continue with next value
        ++value;
      }
    }
  }
}

void LlxScxTest::ParallelTestPost() {
  std::vector< std::pair<char, int> > values;
  internal::LlxScxRecord<Node> * node = &head_llx;
  internal::LlxScxRecord<Node> * next = head_llx.Data().next_;
  while (next != 0) {
    values.push_back(std::make_pair(
      next->Data().value_,
      next->Data().count_.Load()));
    node = next;
    next = next->Data().next_;
  }
  // Check if every character from 'a' to 'z' has been added to the
  // linked list in the correct order
  PT_ASSERT_EQ_MSG(static_cast<size_t>(26 * num_threads_), values.size(),
    "Unexpected size of result list");
}

void LlxScxTest::SerialArrayTest() {
  typedef int Payload;
  typedef embb::base::Atomic<size_t> AtomicField;
  // LLX/SCX with maximum of 3 active load-links in every thread:
  LlxScx<Payload> llxscx(3);

  // Atomic<size_t> not assignable, TryStoreConditional requires
  // a specialization for atomics that uses a.Store(b.Load()).
  AtomicField field(23);

  LlxScxRecord< Payload > r1(100);
  LlxScxRecord< Payload > r2(200);
  LlxScxRecord< Payload > r3(300);

  Payload l1, l2, l3;
  bool finalized;  
  PT_ASSERT(llxscx.TryLoadLinked(&r1, l1, finalized));
  PT_ASSERT_EQ(100, l1);
  PT_ASSERT(llxscx.TryLoadLinked(&r2, l2, finalized));
  PT_ASSERT_EQ(200, l2);
  PT_ASSERT(llxscx.TryLoadLinked(&r3, l3, finalized));
  PT_ASSERT_EQ(300, l3);

  // links = { dr1, dr2, dr3 }
  FixedSizeList< LlxScxRecord<Payload> * >
    links(3);
  links.PushBack(&r1);
  links.PushBack(&r2);
  links.PushBack(&r3);

  FixedSizeList< LlxScxRecord<Payload> * >
    finalize_links(1);
  finalize_links.PushBack(&r3);

  // Try to store new value depending on links:
  size_t a = 42;
  PT_ASSERT(llxscx.TryStoreConditional(
    &field, a,
    links,
    finalize_links));
  // New value should have been changed successfully:
  PT_ASSERT_EQ(field.Load(), a);
}

} // namespace test
} // namespace containers
} // namespace embb
