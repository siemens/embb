/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#include "./hazard_pointer_test.h"

namespace embb {
namespace containers {
namespace test {
HazardPointerTest::HazardPointerTest() :
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  delete_pointer_callback(*this, &HazardPointerTest::DeletePointerCallback),
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  object_pool(NULL),
  stack(NULL),
  hp(NULL),
n_threads(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())) {
  n_elements_per_thread = 100;
  n_elements = n_threads*n_elements_per_thread;
  embb::base::Function < void, embb::base::Atomic<int>* >
    delete_pointer_callback(
    *this,
    &HazardPointerTest::DeletePointerCallback);

  // Kind of timing depending test. But tests exactly what hazard pointers are
  // designed for. One thread creates an element, does something, retires it.
  // Another thread also accesses this element (passed using a stack), by
  // placing a guard it protects this element. If the guard was successfully
  // placed, the pointer is not allowed to be deleted until the second thread
  // removes this guard.
  CreateUnit("HazardPointerTestThatGuardWorks").
    Pre(&HazardPointerTest::HazardPointerTest1_Pre, this).
    Add(
    &HazardPointerTest::HazardPointerTest1_ThreadMethod,
    this, static_cast<size_t>(n_threads)).
    Post(&HazardPointerTest::HazardPointerTest1_Post, this);
}

void HazardPointerTest::HazardPointerTest1_Pre() {
  embb_internal_thread_index_reset();
  object_pool = new embb::containers::ObjectPool< embb::base::Atomic<int> >
    (static_cast<size_t>(n_elements));
  stack = new embb::containers::LockFreeStack< embb::base::Atomic<int>* >
    (static_cast<size_t>(n_elements));
  hp = new embb::containers::internal::HazardPointer< embb::base::Atomic<int>*>
    (delete_pointer_callback,
    NULL,
    1);
}

void HazardPointerTest::HazardPointerTest1_Post() {
  delete object_pool;
  delete stack;
  delete hp;
}

void HazardPointerTest::HazardPointerTest1_ThreadMethod() {
  unsigned int thread_index;
  embb_internal_thread_index(&thread_index);

  for (int i = 0; i != n_elements_per_thread; ++i) {
    embb::base::Atomic<int>* allocated_object = object_pool->Allocate(0);

    hp->GuardPointer(0, allocated_object);

    bool success = stack->TryPush(allocated_object);

    PT_ASSERT(success == true);

    embb::base::Atomic<int>* allocated_object_from_different_thread;

    int diff_count = 0;

    bool same = false;
    bool success_pop;

    while (
      (success_pop = stack->TryPop(allocated_object_from_different_thread))
      == true
      && allocated_object_from_different_thread == allocated_object
      ) {
      //try to make it probable to get an element from a different thread
      //however, can be the same. Try 10000 times to get a different element.
      if (diff_count++ > 10000) {
        same = true;
        break;
      }
      bool success = stack->TryPush(allocated_object_from_different_thread);
      PT_ASSERT(success == true);
    }
    PT_ASSERT(success_pop == true);
    allocated_object->Store(1);
    hp->EnqueuePointerForDeletion(allocated_object);

    if (!same) {
      hp->GuardPointer(0, allocated_object_from_different_thread);

      // if this holds, we were successful in guarding... otherwise we
      // were to late, because the pointer has already been added
      // to the retired list.
      if (*allocated_object_from_different_thread == 0) {
        // the pointer must not be deleted here!
        vector_mutex.Lock();
        for (std::vector< embb::base::Atomic<int>* >::iterator
          it = deleted_vector.begin();
          it != deleted_vector.end();
        ++it) {
          PT_ASSERT(*it != allocated_object_from_different_thread);
        }
        vector_mutex.Unlock();
      }
      hp->GuardPointer(0, NULL);
    }
  }
}

void HazardPointerTest::DeletePointerCallback
(embb::base::Atomic<int>* to_delete) {
  vector_mutex.Lock();
  deleted_vector.push_back(to_delete);
  vector_mutex.Unlock();
}
} // namespace test
} // namespace containers
} // namespace embb
