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

#include "./hazard_pointer_test.h"

#include <embb/base/internal/config.h>

namespace embb {
namespace containers {
namespace test {
IntObjectTestPool::IntObjectTestPool(unsigned int pool_size) :
poolSize(pool_size) {
  simplePoolObjects = static_cast<int*>(
    embb::base::Allocation::Allocate(sizeof(int)*pool_size));

  simplePool = static_cast<embb::base::Atomic<int>*> (
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int>)*
    pool_size));

  for (unsigned int i = 0; i != pool_size; ++i) {
    // in-place new for each array cell
    new (&simplePool[i]) embb::base::Atomic<int>;
  }

  for (unsigned int i = 0; i != pool_size; ++i) {
    simplePool[i] = FREE_MARKER;
    simplePoolObjects[i] = 0;
  }
}

IntObjectTestPool::~IntObjectTestPool() {
  embb::base::Allocation::Free(simplePoolObjects);

  for (unsigned int i = 0; i != poolSize; ++i) {
    // in-place new for each array cell
    simplePool[i].~Atomic();
  }

  embb::base::Allocation::Free(simplePool);
}

int* IntObjectTestPool::Allocate() {
  for (unsigned int i = 0; i != poolSize; ++i) {
    int expected = FREE_MARKER;
    if (simplePool[i].CompareAndSwap
      (expected, ALLOCATED_MARKER)) {
      return &simplePoolObjects[i];
    }
  }
  return 0;
}

void IntObjectTestPool::Release(int* object_pointer) {
  int cell = static_cast<int>(object_pointer - simplePoolObjects);
  simplePool[cell].Store(FREE_MARKER);
}

HazardPointerTest::HazardPointerTest() :
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
delete_pointer_callback_(*this, &HazardPointerTest::DeletePointerCallback),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
  object_pool_(NULL),
  stack_(NULL),
  hazard_pointer_(NULL),
  n_threads_(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())) {
  n_elements_per_thread_ = 100;
  n_elements_ = n_threads_*n_elements_per_thread_;
  embb::base::Function < void, embb::base::Atomic<int>* >
    deletePointerCallback(
    *this,
    &HazardPointerTest::DeletePointerCallback);

  // Kind of timing depending test. But tests exactly what hazard pointers are
  // designed for. One thread creates an element, does something, retires it.
  // Another thread also accesses this element (passed using a stack), by
  // placing a guard it protects this element. If the guard was successfully
  // placed, the pointer is not allowed to be deleted until the second thread
  // removes this guard.
  CreateUnit("HazardPointerTestThatGuardWorks").
    Pre(&HazardPointerTest::HazardPointerTest1Pre, this).
    Add(
    &HazardPointerTest::HazardPointerTest1ThreadMethod,
    this, static_cast<size_t>(n_threads_)).
    Post(&HazardPointerTest::HazardPointerTest1Post, this);
}

void HazardPointerTest::HazardPointerTest1Pre() {
  embb_internal_thread_index_reset();

  object_pool_ =
     embb::base::Allocation::
     New<embb::containers::ObjectPool< embb::base::Atomic<int> > >
     (static_cast<size_t>(n_elements_));

  stack_ = embb::base::Allocation::
    New<embb::containers::LockFreeStack< embb::base::Atomic<int>* > >
    (static_cast<size_t>(n_elements_));

  hazard_pointer_ = embb::base::Allocation::
    New<embb::containers::internal::HazardPointer < embb::base::Atomic<int>* > >
    (delete_pointer_callback_,
    static_cast<embb::base::Atomic<int>*>(NULL),
    1);
}

void HazardPointerTest::HazardPointerTest1Post() {
  embb::base::Allocation::Delete(hazard_pointer_);
  embb::base::Allocation::Delete(object_pool_);
  embb::base::Allocation::Delete(stack_);
}

void HazardPointerTest::HazardPointerTest1ThreadMethod() {
  unsigned int thread_index;
  embb_internal_thread_index(&thread_index);

  for (int i = 0; i != n_elements_per_thread_; ++i) {
    embb::base::Atomic<int>* allocated_object = object_pool_->Allocate(0);
    PT_ASSERT(NULL != allocated_object);

    hazard_pointer_->Guard(0, allocated_object);

    bool success = stack_->TryPush(allocated_object);

    PT_ASSERT(success == true);

    embb::base::Atomic<int>* allocated_object_from_different_thread(0);

    int diff_count = 0;

    bool same = false;
    bool success_pop;

    while (
      (success_pop = stack_->TryPop(allocated_object_from_different_thread))
      == true
      && allocated_object_from_different_thread == allocated_object
      ) {
      // try to make it probable to get an element from a different thread
      // however, can be the same. Try 10000 times to get a different element.
      if (diff_count++ > 10000) {
        same = true;
        break;
      }
      success = stack_->TryPush(allocated_object_from_different_thread);
      PT_ASSERT(success == true);
    }
    PT_ASSERT(success_pop == true);
    allocated_object->Store(1);
    hazard_pointer_->EnqueueForDeletion(allocated_object);

    if (!same) {
      hazard_pointer_->Guard(0, allocated_object_from_different_thread);

      // if this holds, we were successful in guarding... otherwise we
      // were to late, because the pointer has already been added
      // to the retired list.
      if (*allocated_object_from_different_thread == 0) {
        // the pointer must not be deleted here!
        vector_mutex_.Lock();
        for (std::vector< embb::base::Atomic<int>* >::iterator
          it = deleted_vector_.begin();
          it != deleted_vector_.end();
        ++it) {
          PT_ASSERT(*it != allocated_object_from_different_thread);
        }
        vector_mutex_.Unlock();
      }
      hazard_pointer_->Guard(0, NULL);
    }
  }
}

void HazardPointerTest::DeletePointerCallback
(embb::base::Atomic<int>* to_delete) {
  vector_mutex_.Lock();
  deleted_vector_.push_back(to_delete);
  vector_mutex_.Unlock();
}

void HazardPointerTest2::DeletePointerCallback(int* to_delete) {
  test_pool_->Release(to_delete);
}

bool HazardPointerTest2::SetRelativeGuards() {
  unsigned int thread_index = 0;
  int result = embb_internal_thread_index(&thread_index);
  PT_ASSERT(EMBB_SUCCESS == result);

  unsigned int my_begin = guards_per_phread_count_*thread_index;
  int guard_number = 0;
  unsigned int alreadyGuarded = 0;

  for (unsigned int i = my_begin; i != my_begin + guards_per_phread_count_;
    ++i) {
    if (shared_guarded_[i] != 0) {
      alreadyGuarded++;
      guard_number++;
      continue;
    }

    int * to_guard = shared_allocated_[i];
    if (to_guard) {
      hazard_pointer_->Guard(guard_number, to_guard);

      // changed in the meantime?
      if (to_guard == shared_allocated_[i].Load()) {
        // guard was successful. Communicate to other threads.
        shared_guarded_[i] = to_guard;
      } else {
        // reset the guard, couldn't guard...
        hazard_pointer_->RemoveGuard(guard_number);
      }
    }
    guard_number++;
  }
  return(alreadyGuarded == guards_per_phread_count_);
}

void HazardPointerTest2::HazardPointerTest2Master() {
  // while the hazard pointer guard array is not full
  int** allocatedLocal = static_cast<int**>(
  embb::base::Allocation::Allocate(sizeof(int*)*guaranteed_capacity_pool_));
  PT_ASSERT(NULL != allocatedLocal);

  bool full = false;
  while (!full) {
    full = true;
    for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
      if (shared_guarded_[i] == 0) {
        full = false;
        break;
      }
    }

    // not all guards set
    for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
      allocatedLocal[i] = test_pool_->Allocate();
      shared_allocated_[i].Store(allocatedLocal[i]);
    }

    // set my hazards. We do not have to check, this must be successful
    // here.
    SetRelativeGuards();

    // free
    for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
      shared_allocated_[i].Store(0);
      hazard_pointer_->EnqueueForDeletion(allocatedLocal[i]);
    }
  }

  embb::base::Allocation::Free(allocatedLocal);
}

void HazardPointerTest2::HazardPointerTest2Slave() {
  unsigned int thread_index;
  embb_internal_thread_index(&thread_index);

  while (!SetRelativeGuards()) {}
}

void HazardPointerTest2::HazardPointerTest2Pre() {
  embb_internal_thread_index_reset();
  current_master_ = 0;
  sync1_ = 0;
  sync2_ = 0;

  // first the test pool has to be created
  test_pool_ = embb::base::Allocation::New<IntObjectTestPool>
    (pool_size_using_hazard_pointer_);
  PT_ASSERT(NULL != test_pool_);

  // after the pool has been created, we create the hp class
  hazard_pointer_ = embb::base::Allocation::New <
    embb::containers::internal::HazardPointer<int*> >
    (delete_pointer_callback_, static_cast<int*>(NULL),
    static_cast<int>(guards_per_phread_count_), n_threads);
  PT_ASSERT(NULL != hazard_pointer_);

  shared_guarded_ = static_cast<embb::base::Atomic<int*>*>(
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int*>)*
    guaranteed_capacity_pool_));
  PT_ASSERT(NULL != shared_guarded_);

  for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
    // in-place new for each array cell
    new (&shared_guarded_[i]) embb::base::Atomic < int* >;
  }

  shared_allocated_ = static_cast<embb::base::Atomic<int*>*>(
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int*>)*
    guaranteed_capacity_pool_));

  for (unsigned int i = 0; i !=
  guaranteed_capacity_pool_; ++i) {
    // in-place new for each array cell
    new (&shared_allocated_[i]) embb::base::Atomic < int* >;
  }

  for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
    shared_guarded_[i] = 0;
    shared_allocated_[i] = 0;
  }
}

void HazardPointerTest2::HazardPointerTest2Post() {
  for (unsigned int i = 0; i != static_cast<unsigned int>(n_threads); ++i) {
    for (unsigned int i2 = 0; i2 != static_cast<unsigned int>(n_threads)*
      guards_per_phread_count_; ++i2) {
      if (hazard_pointer_->thread_local_retired_lists_
        [i2 + i*n_threads*guards_per_phread_count_] == NULL) {
        // all retired lists must be completely filled
        PT_ASSERT(false);
      }
    }
  }

  unsigned int checks = 0;
  for (unsigned int i = 0; i != static_cast<unsigned int>(n_threads); ++i) {
    for (unsigned int i2 = 0; i2 != static_cast<unsigned int>(n_threads)*
      guards_per_phread_count_; ++i2) {
      for (unsigned int j = 0; j != static_cast<unsigned int>(n_threads); ++j) {
        for (unsigned int j2 = 0; j2 != static_cast<unsigned int>(n_threads)*
          guards_per_phread_count_; ++j2) {
          if (i2 == j2 && i == j)
            continue;

          // all retired elements have to be disjoint
          PT_ASSERT(
            hazard_pointer_->thread_local_retired_lists_
            [i2 + i*n_threads*guards_per_phread_count_] !=
            hazard_pointer_->thread_local_retired_lists_
            [j2 + j*n_threads*guards_per_phread_count_]);

          checks++;
        }
      }
    }
  }

  // sanity check on the count of expected comparisons.
  PT_ASSERT(
    checks ==
    n_threads*n_threads*guards_per_phread_count_ *
    (n_threads*n_threads*guards_per_phread_count_ - 1));

  std::vector< int* > additionallyAllocated;

  // we should be able to still allocate the guaranteed capacity of
  // elements from the pool.
  for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
    int* allocated = test_pool_->Allocate();

    // allocated is not allowed to be zero
    PT_ASSERT(allocated != NULL);

    // push to vector, to check if elements are disjunctive and to release
    // afterwards.
    additionallyAllocated.push_back(allocated);
  }

  // the pool should now be empty
  PT_ASSERT(test_pool_->Allocate() == NULL);

  // release allocated elements...
  for (unsigned int i = 0; i != additionallyAllocated.size(); ++i) {
    test_pool_->Release(additionallyAllocated[i]);
  }

  // the additionallyAllocated elements shall be disjoint
  for (unsigned int i = 0; i != additionallyAllocated.size(); ++i) {
    for (unsigned int i2 = 0; i2 != additionallyAllocated.size(); ++i2) {
      if (i == i2)
        continue;
      PT_ASSERT(additionallyAllocated[i] !=
        additionallyAllocated[i2]);
    }
  }

  // no allocated element should be in any retired list...
  for (unsigned int a = 0; a != additionallyAllocated.size(); ++a) {
    for (unsigned int i = 0; i != static_cast<unsigned int>(n_threads); ++i) {
      for (unsigned int i2 = 0; i2 != static_cast<unsigned int>(n_threads)*
        guards_per_phread_count_; ++i2) {
        PT_ASSERT(
          hazard_pointer_->thread_local_retired_lists_
          [i2 + i*n_threads*guards_per_phread_count_] !=
          additionallyAllocated[a]);
      }
    }
  }

  for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
    // in-place new for each array cell
    shared_guarded_[i].~Atomic();
  }

  embb::base::Allocation::Free(shared_guarded_);

  for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
    // in-place new for each array cell
    shared_allocated_[i].~Atomic();
  }

  embb::base::Allocation::Free(shared_allocated_);
  embb::base::Allocation::Delete(hazard_pointer_);

  // after deleting the hazard pointer object, all retired pointers have
  // to be returned to the pool!
  std::vector<int*> elementsInPool;

  int* nextElement;
  while ((nextElement = test_pool_->Allocate()) != NULL) {
    for (unsigned int i = 0; i != elementsInPool.size(); ++i) {
      // all elements need to be disjoint
      PT_ASSERT(elementsInPool[i] != nextElement);
    }
    elementsInPool.push_back(nextElement);
  }

  // all elements should have been returned by the hp object, so we should be
  // able to acquire all elements.
  PT_ASSERT(elementsInPool.size() == pool_size_using_hazard_pointer_);

  embb::base::Allocation::Delete(test_pool_);
}

void HazardPointerTest2::HazardPointerTest2ThreadMethod() {
  for (;;) {
    unsigned int thread_index = 0;
    int result = embb_internal_thread_index(&thread_index);
    PT_ASSERT(EMBB_SUCCESS == result);

    if (thread_index == current_master_) {
      HazardPointerTest2Master();
    } else {
      HazardPointerTest2Slave();
    }

    sync1_.FetchAndAdd(1);

    // wait until cleanup thread signals to be finished
    while (sync1_ != 0) {
      int expected = n_threads;
      int desired = FINISH_MARKER;
      // select thread, responsible for cleanup
      if (sync1_.CompareAndSwap(expected, desired)) {
        // wipe arrays!
        for (unsigned int i = 0; i != guaranteed_capacity_pool_; ++i) {
          shared_guarded_[i] = 0;
          shared_allocated_[i] = 0;
        }

        // increase master
        current_master_.FetchAndAdd(1);
        sync2_ = 0;
        sync1_.Store(0);
      }
    }

    // wait for all threads to reach this position
    sync2_.FetchAndAdd(1);
    while (sync2_ != static_cast<unsigned int>(n_threads)) {}

    // if each thread was master once, terminate.
    if (current_master_ == static_cast<unsigned int>(n_threads)) {
      return;
    }
  }
}

HazardPointerTest2::HazardPointerTest2() :
n_threads(static_cast<int>
(partest::TestSuite::GetDefaultNumThreads())),

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  delete_pointer_callback_(
  *this,
  &HazardPointerTest2::DeletePointerCallback),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
  shared_guarded_(NULL),
  shared_allocated_(NULL),
  test_pool_(NULL),
  hazard_pointer_(NULL) {
  guards_per_phread_count_ = 5;
  guaranteed_capacity_pool_ = guards_per_phread_count_*n_threads;
  pool_size_using_hazard_pointer_ = guaranteed_capacity_pool_ +
    guards_per_phread_count_*n_threads*n_threads;

  embb::base::Thread::GetThreadsMaxCount();
  CreateUnit("HazardPointerTestSimulateMemoryWorstCase").
    Pre(&HazardPointerTest2::HazardPointerTest2Pre, this).
    Add(
    &HazardPointerTest2::HazardPointerTest2ThreadMethod,
    this, static_cast<size_t>(n_threads)).
    Post(&HazardPointerTest2::HazardPointerTest2Post, this);
}
}  // namespace test
}  // namespace containers
}  // namespace embb
