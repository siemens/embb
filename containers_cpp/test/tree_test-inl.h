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

#ifndef CONTAINERS_CPP_TEST_TREE_TEST_INL_H_
#define CONTAINERS_CPP_TEST_TREE_TEST_INL_H_

#include <utility>
#include <vector>
#include <algorithm>

#include "tree_test.h"

namespace embb {
namespace containers {
namespace test {

template<typename Tree>
TreeTest<Tree>::TreeTest()
    : tree_(NULL),
      bad_key_(static_cast<Key>(-1)),
      bad_value_(static_cast<Value>(-1)) {
  CreateUnit("TreeTestSingleThreadInsertDelete").
      Pre(&TreeTest::TreeTestInsertDelete_Pre, this).
      Add(&TreeTest::TreeTestInsertDeleteSingleThread_ThreadMethod,
          this, 1, NUM_ITERATIONS).
      Post(&TreeTest::TreeTestInsertDelete_Post, this);
  CreateUnit("TreeTestMultiThreadInsertDelete").
      Pre(&TreeTest::TreeTestInsertDelete_Pre, this).
      Add(&TreeTest::TreeTestInsertDeleteMultiThread_ThreadMethod, this,
          NUM_TEST_THREADS, NUM_ITERATIONS).
      Post(&TreeTest::TreeTestInsertDelete_Post, this);
  CreateUnit("TreeTestConcurrentGet").
      Pre(&TreeTest::TreeTestConcurrentGet_Pre, this).
      Add(&TreeTest::TreeTestConcurrentGet_WriterMethod, this,
          NUM_TEST_THREADS / 2, NUM_ITERATIONS).
      Add(&TreeTest::TreeTestConcurrentGet_ReaderMethod, this,
          NUM_TEST_THREADS / 2, NUM_ITERATIONS).
      Post(&TreeTest::TreeTestConcurrentGet_Post, this);
  CreateUnit("TreeTestConcurrentGetMinimal").
      Pre(&TreeTest::TreeTestConcurrentGetMinimal_Pre, this).
      Add(&TreeTest::TreeTestConcurrentGet_WriterMethod, this,
          NUM_TEST_THREADS / 2, NUM_ITERATIONS).
      Add(&TreeTest::TreeTestConcurrentGet_ReaderMethod, this,
          NUM_TEST_THREADS / 2, NUM_ITERATIONS).
      Post(&TreeTest::TreeTestConcurrentGet_Post, this);
//  CreateUnit("TreeTestBalance").
//      Pre(&TreeTest::TreeTestBalance_Pre, this).
//      Add(&TreeTest::TreeTestBalance_ThreadMethod, this,
//          NUM_TEST_THREADS, 1).
//      Post(&TreeTest::TreeTestBalance_Post, this);
}

template<typename Tree>
TreeTest<Tree>::Worker::
Worker(TreePtr tree, int thread_id)
    : tree_(tree), thread_id_(thread_id) {}

template<typename Tree>
void TreeTest<Tree>::Worker::
InsertReplaceDelete(int num_elements) {
  PrepareElements(num_elements);
  InsertAll();
  ReplaceHalf();
  DeleteAll();
}

template<typename Tree>
void TreeTest<Tree>::Worker::
PrepareElements(int num_elements) {
  // Fill the "elements_" vector
  elements_.clear();
  for (int i = 0; i < num_elements; ++i) {
    Key   key   = static_cast<Key  >(i * 100 + thread_id_);
    Value value = static_cast<Value>(i * 100 + thread_id_);
    elements_.push_back(std::make_pair(key, value));
  }
}

template<typename Tree>
void TreeTest<Tree>::Worker::
InsertAll() {
  // Insert elements into the tree
  ::std::random_shuffle(elements_.begin(), elements_.end());
  for (ElementIterator it = elements_.begin(); it != elements_.end(); ++it) {
    Value old_value;
    Value bad_value = tree_->GetUndefinedValue();
    bool success = tree_->TryInsert(it->first, it->second, old_value);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
    PT_ASSERT_EQ_MSG(old_value, bad_value, "A key was already in the tree.");
  }

  // Verify that all inserted elements are available in the tree
  ::std::random_shuffle(elements_.begin(), elements_.end());
  for (ElementIterator it = elements_.begin(); it != elements_.end(); ++it) {
    Value value;
    bool success = tree_->Get(it->first, value);
    PT_ASSERT_MSG(success, "Failed to get an element from the tree.");
    PT_ASSERT_EQ_MSG(it->second, value, "Wrong value retrieved from the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::Worker::
ReplaceHalf() {
  // Replace some of the elements that were inserted earlier
  ::std::random_shuffle(elements_.begin(), elements_.end());
  ElementIterator elements_middle = elements_.begin() +
      static_cast<ptrdiff_t>(elements_.size() / 2);
  for (ElementIterator it = elements_.begin(); it != elements_middle; ++it) {
    Value old_value;
    Value expected = it->second;
    it->second *= 13;
    bool success = tree_->TryInsert(it->first, it->second, old_value);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
    PT_ASSERT_EQ_MSG(old_value, expected, "Wrong value replaced in the tree.");
  }

  // Verify again that all elements are in the tree and have correct values
  ::std::random_shuffle(elements_.begin(), elements_.end());
  for (ElementIterator it = elements_.begin(); it != elements_.end(); ++it) {
    Value value;
    bool success = tree_->Get(it->first, value);
    PT_ASSERT_MSG(success, "Failed to get an element from the tree.");
    PT_ASSERT_EQ_MSG(it->second, value, "Wrong value retrieved from the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::Worker::
DeleteAll() {
  // Delete elements from the tree
  ::std::random_shuffle(elements_.begin(), elements_.end());
  for (ElementIterator it = elements_.begin(); it != elements_.end(); ++it) {
    Value old_value;
    Value expected = it->second;
    bool success = tree_->TryDelete(it->first, old_value);
    PT_ASSERT_MSG(success, "Failed to delete element from the tree.");
    PT_ASSERT_EQ_MSG(expected, old_value, "Wrong value deleted from the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::Worker::
DeleteHalf() {
  // Delete half of the elements from the tree
  ::std::random_shuffle(elements_.begin(), elements_.end());
  ElementIterator elements_middle = elements_.begin() +
      static_cast<ptrdiff_t>(elements_.size() / 2);
  for (ElementIterator it = elements_.begin(); it != elements_middle; ++it) {
    Value old_value;
    Value expected = it->second;
    bool success = tree_->TryDelete(it->first, old_value);
    PT_ASSERT_MSG(success, "Failed to delete element from the tree.");
    PT_ASSERT_EQ_MSG(expected, old_value, "Wrong value deleted from the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestInsertDelete_Pre() {
  embb_internal_thread_index_reset();
  tree_ = new Tree(TREE_CAPACITY, bad_key_, bad_value_);
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestInsertDeleteSingleThread_ThreadMethod() {
  Worker worker(tree_, 0);

  for (int i = 1; i <= 10; ++i) {
    worker.InsertReplaceDelete(i * (TREE_CAPACITY / 10));
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestInsertDeleteMultiThread_ThreadMethod() {
  int thread_id = static_cast<int>(partest::TestSuite::GetCurrentThreadID());
  thread_id %= NUM_TEST_THREADS;

  int num_elements = TREE_CAPACITY / (NUM_TEST_THREADS + 1);
  if (thread_id == 0) {
    num_elements *= 2;
  }

  Worker worker(tree_, thread_id);

  for (int i = 1; i <= 10; ++i) {
    worker.InsertReplaceDelete(i * (num_elements / 10));
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestInsertDelete_Post() {
  PT_ASSERT_MSG((tree_->IsEmpty()), "The tree must be empty at this point.");
  delete tree_;
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestConcurrentGet_Pre() {
  embb_internal_thread_index_reset();
  tree_ = new Tree(TREE_CAPACITY / 2, bad_key_, bad_value_);

  ElementVector elements;
  for (int i = 0; i < TREE_CAPACITY / 2; ++i) {
    Key   key   = static_cast<Key  >(i);
    Value value = static_cast<Value>(i);
    elements.push_back(std::make_pair(key, value));
  }

  ::std::random_shuffle(elements.begin(), elements.end());
  for (ElementIterator it = elements.begin(); it != elements.end(); ++it) {
    Value old_value;
    Value bad_value = tree_->GetUndefinedValue();
    bool success = tree_->TryInsert(it->first, it->second, old_value);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
    PT_ASSERT_EQ_MSG(old_value, bad_value, "A key was already in the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestConcurrentGetMinimal_Pre() {
  embb_internal_thread_index_reset();
  tree_ = new Tree(NUM_TEST_THREADS / 2, bad_key_, bad_value_);

  for (int i = 0; i < NUM_TEST_THREADS / 2; ++i) {
    Key   key   = static_cast<Key  >(TREE_CAPACITY / 4 + i);
    Value value = static_cast<Value>(TREE_CAPACITY / 4 + i);
    Value old_value;
    Value bad_value = tree_->GetUndefinedValue();
    bool success = tree_->TryInsert(key, value, old_value);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
    PT_ASSERT_EQ_MSG(old_value, bad_value, "A key was already in the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestConcurrentGet_WriterMethod() {
  int idx = static_cast<int>(partest::TestSuite::GetCurrentThreadID());
  idx %= (NUM_TEST_THREADS / 2);

  Key   key   = static_cast<Key  >(TREE_CAPACITY / 4 + idx);
  Value value = static_cast<Value>(TREE_CAPACITY / 4 + idx);

  for (int i = 0; i < 1000; ++i) {
    Value old_value;
    bool success = tree_->TryDelete(key, old_value);
    PT_ASSERT_MSG(success, "Failed to delete element from the tree.");
    PT_ASSERT_EQ_MSG(old_value, value, "Wrong value deleted from the tree.");

    Value bad_value = tree_->GetUndefinedValue();
    success = tree_->TryInsert(key, value, old_value);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
    PT_ASSERT_EQ_MSG(old_value, bad_value, "A key was already in the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestConcurrentGet_ReaderMethod() {
  int idx = static_cast<int>(partest::TestSuite::GetCurrentThreadID());
  idx %= (NUM_TEST_THREADS / 2);

  Key key = static_cast<Key>(TREE_CAPACITY / 4 + idx);

  for (int i = 0; i < 1000; ++i) {
    Value value;
    tree_->Get(key, value);
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestConcurrentGet_Post() {
  delete tree_;
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestBalance_Pre() {
  embb_internal_thread_index_reset();
  tree_ = new Tree(TREE_CAPACITY, bad_key_, bad_value_);
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestBalance_ThreadMethod() {
  int thread_id = static_cast<int>(partest::TestSuite::GetCurrentThreadID());
  thread_id %= NUM_TEST_THREADS;

  int num_elements = TREE_CAPACITY / NUM_TEST_THREADS;

  Worker worker(tree_, thread_id);

  worker.PrepareElements(num_elements);
  worker.InsertAll();
  worker.DeleteHalf();
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestBalance_Post() {
  PT_ASSERT_MSG((tree_->IsBalanced()), "The tree must balanced at this point.");
  delete tree_;
}

}  // namespace test
}  // namespace containers
}  // namespace embb

#endif // CONTAINERS_CPP_TEST_TREE_TEST_INL_H_
