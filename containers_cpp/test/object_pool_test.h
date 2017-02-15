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

#ifndef CONTAINERS_CPP_TEST_OBJECT_POOL_TEST_H_
#define CONTAINERS_CPP_TEST_OBJECT_POOL_TEST_H_

#include <partest/partest.h>
#include <embb/base/duration.h>
#include <embb/containers/object_pool.h>

namespace embb {
namespace containers {
namespace test {
class ObjectPoolTestStruct {
 private:
  int thread_id;
 public:
  explicit ObjectPoolTestStruct(int thread_id);
  int GetThreadId() const;
};

template<typename ValuePool>
class ObjectPoolTest : public partest::TestCase {
 private:
  int number_threads_;
  int number_iterations_;
  int allocations_per_thread;
  int allocations;
  embb::containers::ObjectPool<ObjectPoolTestStruct, ValuePool> objectPool;

  void ParallelObjectPoolTest_Pre();
  void ParallelObjectPoolTest_Post();
  void ParallelObjectPoolTest_ThreadMethod();

 public:
  /**
   * Adds test methods.
   */
  ObjectPoolTest();
};
} // namespace test
} // namespace containers
} // namespace embb

#include "./object_pool_test-inl.h"
#endif  // CONTAINERS_CPP_TEST_OBJECT_POOL_TEST_H_
