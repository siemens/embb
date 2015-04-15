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

#ifndef CONTAINERS_CPP_TEST_LLX_SCX_TEST_H_
#define CONTAINERS_CPP_TEST_LLX_SCX_TEST_H_

#include <partest/partest.h>
#include <embb/containers/internal/llx_scx.h>

namespace embb {
namespace containers {
namespace test {

class LlxScxTest : public partest::TestCase {

 private:
  class Node {
   public:
     typedef internal::LlxScxRecord<Node> * node_ptr_t;
   
   public:
     embb::base::Atomic<internal::LlxScxRecord<Node> *> next_;
    embb::base::Atomic<int> count_;
    char value_;

   public:
    Node()
    : value_(-1) {
      next_.Store(reinterpret_cast<node_ptr_t>(0));
      count_.Store(0);
    }

    Node(int count, char value)
    : value_(value) {
      count_.Store(count);
      next_.Store(reinterpret_cast<node_ptr_t>(0));
    }

    Node(int count, char value, node_ptr_t const next_node)
    : value_(value) {
      count_.Store(count);
      next_.Store(next_node);
    }

    Node(const Node & other)
    : value_(other.value_) {
      next_.Store(other.next_.Load());
      count_.Store(other.count_.Load());
    }

    Node & operator=(const Node & rhs) {
      if (this != &rhs) {
        count_.Store(rhs.count_.Load());
        next_.Store(rhs.next_.Load());
        value_ = rhs.value_;
      }
      return *this;
    }
  };

 public:
  /**
   * Adds test methods.
   */
  LlxScxTest();

 private:
  void SerialArrayTest();
  void ParallelTestPre();
  void ParallelTest();
  void ParallelTestPost();

  int num_threads_;
  internal::LlxScx<Node> llxscx_;
  Node tail;
  Node head;
  internal::LlxScxRecord<Node> tail_llx;
  internal::LlxScxRecord<Node> head_llx;
};

} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_LLX_SCX_TEST_H_
