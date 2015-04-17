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
  class Worker {
   public:
    Worker(Tree& tree, size_t thread_id, int num_elements);
    void Run();

   private:
    Tree&  tree_;
    size_t thread_id_;
    int    num_elements_;
  };

  typedef int Key;
  typedef int Value;
  typedef ::std::pair<Key, Value> Element;
  typedef ::std::vector<Element>  ElementVector;
  typedef ElementVector::iterator ElementIterator;

  static const int TREE_CAPACITY = 2000;
  static const int NUM_TEST_THREADS = 3;

  void TreeTestSingleThreadInsertDelete_Pre();
  void TreeTestSingleThreadInsertDelete_ThreadMethod();
  void TreeTestMultiThreadInsertDelete_ThreadMethod();
  void TreeTestSingleThreadInsertDelete_Post();

  Tree *tree_;
};

}  // namespace test
}  // namespace containers
}  // namespace embb

#include "./tree_test-inl.h"

#endif // CONTAINERS_CPP_TEST_TREE_TEST_H_
