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

#ifndef CONTAINERS_CPP_TEST_HAZARD_POINTER_TEST_H_
#define CONTAINERS_CPP_TEST_HAZARD_POINTER_TEST_H_

#include <vector>
#include <partest/partest.h>
#include <embb/containers/internal/hazard_pointer.h>
#include <embb/containers/object_pool.h>
#include <embb/containers/lock_free_stack.h>

namespace embb {
namespace containers {
namespace test {
class HazardPointerTest : public partest::TestCase {
 private:
  embb::base::Function<void, embb::base::Atomic<int>*> delete_pointer_callback;

  //used to allocate random stuff, we will just use the pointers, not the
  //contents
  embb::containers::ObjectPool< embb::base::Atomic<int> >* object_pool;

  //used to move pointer between threads
  embb::containers::LockFreeStack< embb::base::Atomic<int>* >* stack;
  embb::base::Mutex vector_mutex;
  embb::containers::internal::HazardPointer<embb::base::Atomic<int>*>* hp;
  std::vector< embb::base::Atomic<int>* > deleted_vector;
  int n_threads;
  int n_elements_per_thread;
  int n_elements;

 public:
  /**
  * Adds test methods.
  */
  HazardPointerTest();
  void HazardPointerTest1_Pre();
  void HazardPointerTest1_Post();
  void HazardPointerTest1_ThreadMethod();
  void DeletePointerCallback(embb::base::Atomic<int>* to_delete);
};
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_HAZARD_POINTER_TEST_H_
