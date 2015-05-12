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

#ifndef CONTAINERS_CPP_TEST_TREE_TEST_H_
#define CONTAINERS_CPP_TEST_TREE_TEST_H_

namespace embb {
namespace containers {
namespace test {

template<typename Tree>
class TreeTest : public partest::TestCase {
 public:
  TreeTest();

 private:
  typedef Tree*                             TreePtr;
  typedef typename Tree::KeyType            Key;
  typedef typename Tree::ValueType          Value;
  typedef ::std::pair<Key, Value>           Element;
  typedef ::std::vector<Element>            ElementVector;
  typedef typename ElementVector::iterator  ElementIterator;

  class Worker {
   public:
    Worker(TreePtr tree, int thread_id);
    void InsertReplaceDelete(int num_elements);
    void PrepareElements(int num_elements);
    void InsertAll();
    void ReplaceHalf();
    void DeleteAll();
    void DeleteHalf();

   private:
    Worker(const Worker&);
    Worker& operator=(const Worker&);

    TreePtr       tree_;
    int           thread_id_;
    ElementVector elements_;
  };

  static const int TREE_CAPACITY = 100;
  static const int NUM_TEST_THREADS = 4;
  static const int NUM_ITERATIONS = 100;

  void TreeTestInsertDelete_Pre();
  void TreeTestInsertDeleteSingleThread_ThreadMethod();
  void TreeTestInsertDeleteMultiThread_ThreadMethod();
  void TreeTestInsertDelete_Post();
  void TreeTestConcurrentGet_Pre();
  void TreeTestConcurrentGetMinimal_Pre();
  void TreeTestConcurrentGet_WriterMethod();
  void TreeTestConcurrentGet_ReaderMethod();
  void TreeTestConcurrentGet_Post();
  void TreeTestBalance_Pre();
  void TreeTestBalance_ThreadMethod();
  void TreeTestBalance_Post();

  TreePtr tree_;
  Key     bad_key_;
  Value   bad_value_;
};

}  // namespace test
}  // namespace containers
}  // namespace embb

#include "./tree_test-inl.h"

#endif // CONTAINERS_CPP_TEST_TREE_TEST_H_
