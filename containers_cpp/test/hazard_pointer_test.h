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
/**
 * @brief a very simple wait-free object pool implementation to have tests
 * being independent of the EMBB object pool implementation.
 */
class IntObjectTestPool {
 private:
  int* simplePoolObjects;
  embb::base::Atomic<int>* simplePool;

 public:
  static const int ALLOCATED_MARKER = 1;
  static const int FREE_MARKER = 0;
  unsigned int poolSize;

  explicit IntObjectTestPool(unsigned int pool_size);

  ~IntObjectTestPool();

  /**
   * Allocate object from the pool
   *
   * @return the allocated object
   */
  int* Allocate();

  /**
   * Return an element to the pool
   *
   * @param objectPointer the object to be freed
   */
  void Release(int* object_pointer);
};

class HazardPointerTest : public partest::TestCase {
 public:
  /**
  * Adds test methods.
  */
  HazardPointerTest();
  void HazardPointerTest1Pre();
  void HazardPointerTest1Post();
  void HazardPointerTest1ThreadMethod();
  void DeletePointerCallback(embb::base::Atomic<int>* to_delete);

 private:
  embb::base::Function<void, embb::base::Atomic<int>*> delete_pointer_callback_;

  //used to allocate random stuff, we will just use the pointers, not the
  //contents
  embb::containers::ObjectPool< embb::base::Atomic<int> >* object_pool_;

  //used to move pointer between threads
  embb::containers::LockFreeStack< embb::base::Atomic<int>* >* stack_;
  embb::base::Mutex vector_mutex_;
  embb::containers::internal::HazardPointer<embb::base::Atomic<int>*>*
    hazard_pointer_;
  std::vector< embb::base::Atomic<int>* > deleted_vector_;
  int n_threads_;
  int n_elements_per_thread_;
  int n_elements_;
};

class HazardPointerTest2 : public partest::TestCase {
 public:
  void DeletePointerCallback(int* to_delete);
  bool SetRelativeGuards();
  void HazardPointerTest2Master();
  void HazardPointerTest2Slave();

  void HazardPointerTest2Pre();
  void HazardPointerTest2Post();

  void HazardPointerTest2ThreadMethod();

  HazardPointerTest2();

 private:
  // number of threads, participating in that test
  int n_threads;

  embb::base::Function<void, int*> delete_pointer_callback_;
  // the thread id of the master
  embb::base::Atomic<unsigned int> current_master_;

  // variables, to synchronize threads. At each point in time, one master,
  // the master changes each round until each thread was assigned master once.
  embb::base::Atomic<int> sync1_;
  embb::base::Atomic<unsigned int> sync2_;

  unsigned int guards_per_phread_count_;
  unsigned int guaranteed_capacity_pool_;
  unsigned int pool_size_using_hazard_pointer_;

  // The threads write here, if they guarded an object successfully. Used to
  // determine when all allocated objects were guarded successfully.
  embb::base::Atomic<int*>* shared_guarded_;

  // This array is used by the master, to communicate and share what he has
  // allocated with the slaves.
  embb::base::Atomic<int*>* shared_allocated_;

  // Reference to the object pool
  IntObjectTestPool* test_pool_;

  embb::containers::internal::HazardPointer<int*>* hazard_pointer_;
  static const int FINISH_MARKER = -1;
};
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_HAZARD_POINTER_TEST_H_
