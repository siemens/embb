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

#include "./hazard_pointer_test.h"

#include <embb/base/internal/config.h>

namespace embb {
namespace containers {
namespace test {

IntObjectTestPool::IntObjectTestPool(unsigned int poolSize) :
poolSize(poolSize)
{
  simplePoolObjects = static_cast<int*>(
    embb::base::Allocation::Allocate(sizeof(int)*poolSize));

  simplePool = static_cast<embb::base::Atomic<int>*> (
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int>)*
    poolSize));

  for (unsigned int i = 0; i != poolSize; ++i) {
    //in-place new for each array cell
    new (&simplePool[i]) embb::base::Atomic<int>;
  }

  for (unsigned int i = 0; i != poolSize; ++i) {
    simplePool[i] = FREE_MARKER;
    simplePoolObjects[i] = 0;
  }
}

IntObjectTestPool::~IntObjectTestPool() {
  embb::base::Allocation::Free(simplePoolObjects);

  for (unsigned int i = 0; i != poolSize; ++i) {
    //in-place new for each array cell
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

void IntObjectTestPool::Release(int* objectPointer) {
  int cell = objectPointer - simplePoolObjects;
  simplePool[cell].Store(FREE_MARKER);
}

HazardPointerTest::HazardPointerTest() :
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
deletePointerCallback(*this, &HazardPointerTest::DeletePointerCallback),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
  objectPool(NULL),
  stack(NULL),
  hazardPointer(NULL),
  nThreads(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())) {
  nElementsPerThread = 100;
  nElements = nThreads*nElementsPerThread;
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
    this, static_cast<size_t>(nThreads)).
    Post(&HazardPointerTest::HazardPointerTest1Post, this);
}

void HazardPointerTest::HazardPointerTest1Pre() {
  embb_internal_thread_index_reset();

  objectPool =
     embb::base::Allocation::
     New<embb::containers::ObjectPool< embb::base::Atomic<int> > >
     (static_cast<size_t>(nElements));

  stack = embb::base::Allocation::
    New<embb::containers::LockFreeStack< embb::base::Atomic<int>* > >
    (static_cast<size_t>(nElements));

  hazardPointer = embb::base::Allocation::
    New<embb::containers::internal::HazardPointer < embb::base::Atomic<int>* > >
    (deletePointerCallback,
    static_cast<embb::base::Atomic<int>*>(NULL),
    1);
}

void HazardPointerTest::HazardPointerTest1Post() {
  embb::base::Allocation::Delete(hazardPointer);
  embb::base::Allocation::Delete(objectPool);
  embb::base::Allocation::Delete(stack);
}

void HazardPointerTest::HazardPointerTest1ThreadMethod() {
  unsigned int thread_index;
  embb_internal_thread_index(&thread_index);

  for (int i = 0; i != nElementsPerThread; ++i) {
    embb::base::Atomic<int>* allocated_object = objectPool->Allocate(0);

    hazardPointer->Guard(0, allocated_object);

    bool success = stack->TryPush(allocated_object);

    PT_ASSERT(success == true);

    embb::base::Atomic<int>* allocated_object_from_different_thread(0);

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
    hazardPointer->EnqueueForDeletion(allocated_object);

    if (!same) {
      hazardPointer->Guard(0, allocated_object_from_different_thread);

      // if this holds, we were successful in guarding... otherwise we
      // were to late, because the pointer has already been added
      // to the retired list.
      if (*allocated_object_from_different_thread == 0) {
        // the pointer must not be deleted here!
        vectorMutex.Lock();
        for (std::vector< embb::base::Atomic<int>* >::iterator
          it = deletedVector.begin();
          it != deletedVector.end();
        ++it) {
          PT_ASSERT(*it != allocated_object_from_different_thread);
        }
        vectorMutex.Unlock();
      }
      hazardPointer->Guard(0, NULL);
    }
  }
}

void HazardPointerTest::DeletePointerCallback
(embb::base::Atomic<int>* to_delete) {
  vectorMutex.Lock();
  deletedVector.push_back(to_delete);
  vectorMutex.Unlock();
}

void HazardPointerTest2::DeletePointerCallback(int* toDelete) {
  testPool->Release(toDelete);
}

bool HazardPointerTest2::SetRelativeGuards() {
  unsigned int threadIndex;
  embb_internal_thread_index(&threadIndex);

  unsigned int my_begin = guardsPerThreadCount*threadIndex;
  int guardNumber = 0;
  unsigned int alreadyGuarded = 0;

  for (unsigned int i = my_begin; i != my_begin + guardsPerThreadCount; ++i){
    if (sharedGuarded[i] != 0) {
      alreadyGuarded++;
      guardNumber++;
      continue;
    }

    int * toGuard = sharedAllocated[i];
    if (toGuard) {
      hazardPointer->Guard(guardNumber, toGuard);

      // changed in the meantime?
      if (toGuard == sharedAllocated[i].Load()) {
        // guard was successful. Communicate to other threads.
        sharedGuarded[i] = toGuard;
      }
      else {
        // reset the guard, couldn't guard...
        hazardPointer->RemoveGuard(guardNumber);
      }
    }
    guardNumber++;
  }
  return(alreadyGuarded == guardsPerThreadCount);
}

void HazardPointerTest2::HazardPointerTest2Master() {
  // while the hazard pointer guard array is not full
  int** allocatedLocal = static_cast<int**>(
  embb::base::Allocation::Allocate(sizeof(int*)*guaranteedCapacityPool));

  bool full = false;
  while (!full) {
    full = true;
    for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
      if (sharedGuarded[i] == 0) {
        full = false;
        break;
      }
    }

    // not all guards set
    for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
      allocatedLocal[i] = testPool->Allocate();
      sharedAllocated[i].Store(allocatedLocal[i]);
    }

    // set my hazards. We do not have to check, this must be successful
    // here.
    SetRelativeGuards();

    // free
    for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
      sharedAllocated[i].Store(0);
      hazardPointer->EnqueueForDeletion(allocatedLocal[i]);
    }
  }

  embb::base::Allocation::Free(allocatedLocal);
}

void HazardPointerTest2::HazardPointerTest2Slave() {
  unsigned int thread_index;
  embb_internal_thread_index(&thread_index);

  while (!SetRelativeGuards()) {};
}

void HazardPointerTest2::HazardPointerTest2Pre() {
  embb_internal_thread_index_reset();
  currentMaster = 0;
  sync1 = 0;
  sync2 = 0;

  // first the test pool has to be created
  testPool = embb::base::Allocation::New<IntObjectTestPool>(poolSizeUsingHazardPointer);

  // after the pool has been created, we create the hp class
  hazardPointer = embb::base::Allocation::New <
    embb::containers::internal::HazardPointer<int*> >
    (deletePointerCallback, static_cast<int*>(NULL),
    static_cast<int>(guardsPerThreadCount), nThreads);

  sharedGuarded = static_cast<embb::base::Atomic<int*>*>(
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int*>)*
    guaranteedCapacityPool)
  );

  for (unsigned int i = 0; i !=
  guaranteedCapacityPool; ++i) {
    //in-place new for each array cell
    new (&sharedGuarded[i]) embb::base::Atomic < int* > ;
  }

  sharedAllocated = static_cast<embb::base::Atomic<int*>*>(
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int*>)*
    guaranteedCapacityPool)
  );

  for (unsigned int i = 0; i !=
  guaranteedCapacityPool; ++i) {
    //in-place new for each array cell
    new (&sharedAllocated[i]) embb::base::Atomic < int* > ;
  }

  for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
    sharedGuarded[i] = 0;
    sharedAllocated[i] = 0;
  }
}

void HazardPointerTest2::HazardPointerTest2Post() {

  for (unsigned int i = 0; i != static_cast<unsigned int>(nThreads); ++i) {
    for (unsigned int i2 = 0; i2 != static_cast<unsigned int>(nThreads)*
      guardsPerThreadCount; ++i2) {
      if (hazardPointer->threadLocalRetiredLists
        [i2 + i*nThreads*guardsPerThreadCount] == NULL) {
        // all retired lists must be completely filled
        PT_ASSERT(false);
      }
    }
  }

  unsigned int checks = 0;
  for (unsigned int i = 0; i != static_cast<unsigned int>(nThreads); ++i) {
    for (unsigned int i2 = 0; i2 != static_cast<unsigned int>(nThreads)*
      guardsPerThreadCount; ++i2) {
      for (unsigned int j = 0; j != static_cast<unsigned int>(nThreads); ++j) {
        for (unsigned int j2 = 0; j2 != static_cast<unsigned int>(nThreads)*
          guardsPerThreadCount; ++j2) {
          if (i2 == j2 && i == j)
            continue;

          // all retired elements have to be disjoint
          PT_ASSERT(
            hazardPointer->threadLocalRetiredLists
            [i2 + i*nThreads*guardsPerThreadCount] !=
            hazardPointer->threadLocalRetiredLists
            [j2 + j*nThreads*guardsPerThreadCount]
          );

          checks++;
        }
      }
    }
  }

  // sanity check on the count of expected comparisons.
  PT_ASSERT(
    checks ==
    nThreads*nThreads*guardsPerThreadCount *
    (nThreads*nThreads*guardsPerThreadCount - 1)
    );

  std::vector< int* > additionallyAllocated;

  // we should be able to still allocate the guaranteed capacity of
  // elements from the pool.
  for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
    int* allocated = testPool->Allocate();

    // allocated is not allowed to be zero
    PT_ASSERT(allocated != NULL);

    // push to vector, to check if elements are disjunctive and to release
    // afterwards.
    additionallyAllocated.push_back(allocated);
  }

  // the pool should now be empty
  PT_ASSERT(testPool->Allocate() == NULL);

  // release allocated elements...
  for (unsigned int i = 0; i != additionallyAllocated.size(); ++i) {
    testPool->Release(additionallyAllocated[i]);
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
    for (unsigned int i = 0; i != static_cast<unsigned int>(nThreads); ++i) {
      for (unsigned int i2 = 0; i2 != static_cast<unsigned int>(nThreads)*
        guardsPerThreadCount; ++i2) {
        PT_ASSERT(
          hazardPointer->threadLocalRetiredLists
          [i2 + i*nThreads*guardsPerThreadCount] !=
          additionallyAllocated[a]
          );
      }
    }
  }

  for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
    //in-place new for each array cell
    sharedGuarded[i].~Atomic();
  }

  embb::base::Allocation::Free(sharedGuarded);

  for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
    //in-place new for each array cell
    sharedAllocated[i].~Atomic();
  }

  embb::base::Allocation::Free(sharedAllocated);
  embb::base::Allocation::Delete(hazardPointer);

  // after deleting the hazard pointer object, all retired pointers have
  // to be returned to the pool!
  std::vector<int*> elementsInPool;

  int* nextElement;
  while ((nextElement = testPool->Allocate()) != NULL) {
    for (unsigned int i = 0; i != elementsInPool.size(); ++i) {
      // all elements need to be disjoint
      PT_ASSERT(elementsInPool[i] != nextElement);
    }
    elementsInPool.push_back(nextElement);
  }

  // all elements should have been returned by the hp object, so we should be
  // able to acquire all elements.
  PT_ASSERT(elementsInPool.size() == poolSizeUsingHazardPointer);

  embb::base::Allocation::Delete(testPool);
}

void HazardPointerTest2::HazardPointerTest2ThreadMethod() {
  for (;;) {
    unsigned int threadIndex;
    embb_internal_thread_index(&threadIndex);

    if (threadIndex == currentMaster) {
      HazardPointerTest2Master();
    }
    else {
      HazardPointerTest2Slave();
    }

    sync1.FetchAndAdd(1);

    // wait until cleanup thread signals to be finished
    while (sync1 != 0) {
      int expected = nThreads;
      int desired = finishMarker;
      // select thread, responsible for cleanup
      if (sync1.CompareAndSwap(expected, desired)) {

        //wipe arrays!
        for (unsigned int i = 0; i != guaranteedCapacityPool; ++i) {
          sharedGuarded[i] = 0;
          sharedAllocated[i] = 0;
        }

        // increase master
        currentMaster.FetchAndAdd(1);
        sync2 = 0;
        sync1.Store(0);
      }
    }

    // wait for all threads to reach this position
    sync2.FetchAndAdd(1);
    while (sync2 != static_cast<unsigned int>(nThreads)) {}

    // if each thread was master once, terminate.
    if (currentMaster == static_cast<unsigned int>(nThreads)) {
      return;
    }
  }
}

HazardPointerTest2::HazardPointerTest2() :
nThreads(static_cast<int>
(partest::TestSuite::GetDefaultNumThreads())),

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
   deletePointerCallback(
   *this,
   &HazardPointerTest2::DeletePointerCallback)
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
{
  guardsPerThreadCount = 5;
  guaranteedCapacityPool = guardsPerThreadCount*nThreads;
  poolSizeUsingHazardPointer = guaranteedCapacityPool +
    guardsPerThreadCount*nThreads*nThreads;

  embb::base::Thread::GetThreadsMaxCount();
  CreateUnit("HazardPointerTestSimulateMemoryWorstCase").
    Pre(&HazardPointerTest2::HazardPointerTest2Pre, this).
    Add(
    &HazardPointerTest2::HazardPointerTest2ThreadMethod,
    this, static_cast<size_t>(nThreads)).
    Post(&HazardPointerTest2::HazardPointerTest2Post, this);
}

} // namespace test
} // namespace containers
} // namespace embb
