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
    : tree_(NULL) {
  // Repeat twice to ensure that the tree remains operational after all the
  // elements were removed from it
  CreateUnit("TreeTestSingleThreadInsertDelete").
      Pre(&TreeTest::TreeTestSingleThreadInsertDelete_Pre, this).
      Add(&TreeTest::TreeTestSingleThreadInsertDelete_ThreadMethod, this, 1, 2).
      Post(&TreeTest::TreeTestSingleThreadInsertDelete_Post, this);
  CreateUnit("TreeTestMultiThreadInsertDelete").
      Pre(&TreeTest::TreeTestSingleThreadInsertDelete_Pre, this).
      Add(&TreeTest::TreeTestMultiThreadInsertDelete_ThreadMethod, this,
          NUM_TEST_THREADS, 2).
      Post(&TreeTest::TreeTestSingleThreadInsertDelete_Post, this);
}

template<typename Tree>
TreeTest<Tree>::Worker::
Worker(Tree& tree, size_t thread_id, int num_elements)
    : tree_(tree), thread_id_(thread_id), num_elements_(num_elements) {}

template<typename Tree>
void TreeTest<Tree>::Worker::
Run() {
  ElementVector elements;
  for (int i = 0; i < num_elements_; ++i) {
    Key   key   = i * 133 * 100 + thread_id_;
    Value value = i * 133 * 100 + thread_id_;
    elements.push_back(std::make_pair(key, value));
  }

  // Insert elements into the tree
  ::std::random_shuffle(elements.begin(), elements.end());
  for (ElementIterator it = elements.begin(); it != elements.end(); ++it) {
    bool success = tree_.TryInsert(it->first, it->second);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
  }

  // Verify that all inserted elements are available in the tree
  ::std::random_shuffle(elements.begin(), elements.end());
  for (ElementIterator it = elements.begin(); it != elements.end(); ++it) {
    Value value;
    bool success = tree_.Get(it->first, value);
    PT_ASSERT_MSG(success, "Failed to get an element from the tree.");
    PT_ASSERT_MSG(it->second == value, "Wrong value retrieved from the tree.");
  }

  // Replace some of the elements that were inserted earlier
  ::std::random_shuffle(elements.begin(), elements.end());
  ElementIterator elements_middle = elements.begin() + num_elements_ / 2;
  for (ElementIterator it = elements.begin(); it != elements_middle; ++it) {
    it->second *= 13;
    bool success = tree_.TryInsert(it->first, it->second);
    PT_ASSERT_MSG(success, "Failed to insert element into the tree.");
  }

  // Verify again that all elements are in the tree and have correct values
  ::std::random_shuffle(elements.begin(), elements.end());
  for (ElementIterator it = elements.begin(); it != elements.end(); ++it) {
    Value value;
    bool success = tree_.Get(it->first, value);
    PT_ASSERT_MSG(success, "Failed to get an element from the tree.");
    PT_ASSERT_MSG(it->second == value, "Wrong value retrieved from the tree.");
  }

  // Delete elements from the tree
  ::std::random_shuffle(elements.begin(), elements.end());
  for (ElementIterator it = elements.begin(); it != elements.end(); ++it) {
    Value value;
    bool success = tree_.TryDelete(it->first, value);
    PT_ASSERT_MSG(success, "Failed to delete element from the tree.");
    PT_ASSERT_MSG(it->second == value, "Wrong value deleted from the tree.");
  }
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestSingleThreadInsertDelete_Pre() {
  tree_ = new Tree(TREE_CAPACITY);
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestSingleThreadInsertDelete_ThreadMethod() {
  size_t thread_id = partest::TestSuite::GetCurrentThreadID();

  Worker worker(*tree_, thread_id, TREE_CAPACITY);

  worker.Run();
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestMultiThreadInsertDelete_ThreadMethod() {
  size_t thread_id = partest::TestSuite::GetCurrentThreadID();

  int num_elements = TREE_CAPACITY / (NUM_TEST_THREADS + 1);
  if (thread_id == 0) {
    num_elements *= 2;
  }
  Worker worker(*tree_, thread_id, num_elements);

  worker.Run();
}

template<typename Tree>
void TreeTest<Tree>::
TreeTestSingleThreadInsertDelete_Post() {
  PT_ASSERT_MSG((tree_->IsEmpty()), "The tree must be empty at this point.");
  delete tree_;
}

}  // namespace test
}  // namespace containers
}  // namespace embb

#endif // CONTAINERS_CPP_TEST_TREE_TEST_INL_H_
