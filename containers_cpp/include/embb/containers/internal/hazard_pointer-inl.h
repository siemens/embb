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

#ifndef EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_
#define EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_

namespace embb {
namespace containers {
namespace internal {

// Visual Studio is complaining, that the return in the last line of this
// function is not reachable. This is true, as long as exceptions are enabled.
// Otherwise, the exception becomes an assertion and with disabling assertions,
// the code becomes reachable. So, disabling this warning.
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4702)
#endif
template< typename GuardType >
unsigned int HazardPointer< GuardType >::GetCurrentThreadIndex() {

  // first, get the EMBB native thread id.
  unsigned int embbThreadIndex;

  int return_val = embb_internal_thread_index(&embbThreadIndex);

  if (return_val != EMBB_SUCCESS) {
    EMBB_THROW(embb::base::ErrorException, "Could not get thread id");
  }

  // iterate over the mappings array
  for (unsigned int i = 0; i != accessorCount; ++i) {
    // end of mappings? then we need to write our id
    if (threadIdMapping[i] == -1) {
      // try to CAS the initial value with out thread id
      int expected = -1;
      if (threadIdMapping[i].CompareAndSwap(expected,
        static_cast<int>(embbThreadIndex))) {
        //successful, return our mapping
        return i;
      }
    }

    if (threadIdMapping[i] == static_cast<int>(embbThreadIndex)) {
      // found our mapping!
      return i;
    }
  }

  // when we reach this point, we have too many accessors
  // (no mapping possible)
  EMBB_THROW(embb::base::ErrorException, "Too many accessors");

  return 0;
}
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif

template< typename GuardType >
void HazardPointer< GuardType >::RemoveGuard(int guardPosition){
  const unsigned int myThreadId = GetCurrentThreadIndex();

  // check invariants...
  assert(guardPosition < guardsPerThread && myThreadId < accessorCount);

  // set guard
  guards[guardPosition*accessorCount + myThreadId] = undefinedGuard;
}

template< typename GuardType >
HazardPointer< GuardType >::HazardPointer(
  embb::base::Function<void, GuardType> freeGuardCallback,
  GuardType undefinedGuard, int guardsPerThread, int accessors) :
  accessorCount(accessors == -1 ?
  embb::base::Thread::GetThreadsMaxCount() :
  accessors),
  undefinedGuard(undefinedGuard),
  guardsPerThread(guardsPerThread),
  freeGuardCallback(freeGuardCallback) {

  threadIdMapping =
    static_cast<embb::base::Atomic<int>*>(
    embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int>)
    *accessorCount));

  for (unsigned int i = 0; i != accessorCount; ++i) {
    //in-place new for each cell
    new (&threadIdMapping[i]) embb::base::Atomic < int > ;
  }

  guards = static_cast<embb::base::Atomic< GuardType >*>
    (embb::base::Allocation::Allocate(
    sizeof(embb::base::Atomic< GuardType >) *
    guardsPerThread * accessorCount
    ));

  for (unsigned int i = 0; i != guardsPerThread * accessorCount; ++i) {
    //in-place new for each cell
    new (&guards[i]) embb::base::Atomic < GuardType > ;
  }

  threadLocalRetiredListsTemp = static_cast<GuardType*>
    (embb::base::Allocation::Allocate(
    sizeof(GuardType) *
    guardsPerThread * accessorCount * accessorCount
    ));

  for (unsigned int i = 0; i !=
    guardsPerThread * accessorCount * accessorCount; ++i) {
    //in-place new for each cell
    new (&threadLocalRetiredListsTemp[i]) GuardType;
  }

  threadLocalRetiredLists = static_cast<GuardType*>
    (embb::base::Allocation::Allocate(
    sizeof(GuardType) *
    guardsPerThread * accessorCount * accessorCount
    ));

  for (unsigned int i = 0; i !=
    guardsPerThread * accessorCount * accessorCount; ++i) {
    //in-place new for each cell
    new (&threadLocalRetiredLists[i]) GuardType;
  }

  // init guards and retired lists to the undefined guard
  for (unsigned int i = 0; i != static_cast<unsigned int>(guardsPerThread);
    ++i) {
    for (unsigned int i2 = 0; i2 != accessorCount; ++i2) {
      guards[i*accessorCount + i2] = undefinedGuard;
    }
  }

  for (unsigned int j = 0; j != accessorCount; ++j) {
    for (unsigned int i = 0; i != guardsPerThread*accessorCount; ++i) {
      threadLocalRetiredListsTemp
        [j*(accessorCount*guardsPerThread) + i] =
        undefinedGuard;
      threadLocalRetiredLists
        [j*(accessorCount*guardsPerThread) + i] =
        undefinedGuard;
    }
  }

  for (unsigned int i = 0; i != accessorCount; ++i) {
    //in-place new for each cell
    threadIdMapping[i] = -1;
  }
}

template< typename GuardType >
HazardPointer< GuardType >::~HazardPointer() {

  // Release references from all retired lists. Note that for this to work, the
  // data structure using hazard pointer has still to be active... So first, the
  // hazard pointer class shall be destructed, then the memory management class
  // (e.g. some pool). Otherwise, the hazard pointer class would try to return
  // memory to an already destructed memory manager.
  for (unsigned int j = 0; j != accessorCount; ++j) {
    for (unsigned int i = 0; i != accessorCount*guardsPerThread; ++i) {
      GuardType pointerToFree =
        threadLocalRetiredLists
        [j * accessorCount * guardsPerThread + i];
      if (pointerToFree == undefinedGuard) {
        break;
      }
      freeGuardCallback(pointerToFree);
    }
  }

  for (unsigned int i = 0; i != accessorCount; ++i) {
    threadIdMapping[i].~Atomic();
  }

  embb::base::Allocation::Free(threadIdMapping);

  for (unsigned int i = 0; i != guardsPerThread * accessorCount; ++i) {
    guards[i].~Atomic();
  }

  embb::base::Allocation::Free(guards);

  for (unsigned int i = 0; i !=
    guardsPerThread * accessorCount * accessorCount; ++i) {
    threadLocalRetiredListsTemp[i].~GuardType();
  }

  embb::base::Allocation::Free(threadLocalRetiredListsTemp);

  for (unsigned int i = 0; i !=
    guardsPerThread * accessorCount * accessorCount; ++i) {
    threadLocalRetiredLists[i].~GuardType();
  }

  embb::base::Allocation::Free(threadLocalRetiredLists);
}

template< typename GuardType >
void HazardPointer< GuardType >::Guard(int guardPosition,
  GuardType guardedElement) {
  const unsigned int myThreadId = GetCurrentThreadIndex();

  // check invariants...
  assert(guardPosition < guardsPerThread && myThreadId < accessorCount);

  // set guard
  guards[guardPosition*accessorCount + myThreadId] = guardedElement;
}

template< typename GuardType >
size_t HazardPointer< GuardType >::ComputeMaximumRetiredObjectCount(
size_t guardsPerThread, int accessors) {

  unsigned int accessorCount = (accessors == -1 ?
    embb::base::Thread::GetThreadsMaxCount() :
    accessors);

  return static_cast<size_t>(
      guardsPerThread * accessorCount * accessorCount);
}

template< typename GuardType >
void HazardPointer< GuardType >::CopyRetiredList(GuardType* sourceList,
  GuardType* targetList, unsigned int retiredListSize,
  GuardType undefinedGuard) {
  bool done = false;
  for (unsigned int ii = 0; ii != retiredListSize; ++ii) {
    if (!done) {
      GuardType guardToCopy = sourceList[ii];

      if (guardToCopy == undefinedGuard) {
        done = true;

        if (targetList[ii] == undefinedGuard) {
          // end of target list
          break;
        }
      }
      targetList[ii] = guardToCopy;
    }
    else {
      // we copied the whole source list, remaining values in the target
      // have to be zeroed.
      if (targetList[ii] == undefinedGuard) {
        // end of target list
        break;
      }
      else {
        targetList[ii] = undefinedGuard;
      }
    }
  }
}

template< typename GuardType >
void HazardPointer< GuardType >::UpdateRetiredList(GuardType* retiredList,
  GuardType* updatedRetiredList, unsigned int retiredListSize,
  GuardType guardedElement, GuardType consideredHazard,
  GuardType undefinedGuard) {

  // no hazard set here
  if (consideredHazard == undefinedGuard)
      return;

  // if this hazard is currently in the union of
  // threadLocalRetiredLists and pointerToRetire, but not yet in
  // threadLocalRetiredListsTemp, add it to that list
  bool containedInUnion = false;

  // first iterate over our retired list
  for (unsigned int ii = 0; ii != retiredListSize; ++ii) {
    // when reaching 0, we can stop iterating (end of the "list")
    if (retiredList[ii] == 0)
        break;

    // the hazard is contained in the retired list... it shall go
    // into the temp list, if not already there
    if (retiredList[ii] == consideredHazard) {
      containedInUnion = true;
      break;
    }
  }

  // the union also contains pointerToRetire
  if (!containedInUnion) {
    containedInUnion = (consideredHazard == guardedElement);
  }

  // add the pointer to temp. retired list, if not already there
  if (containedInUnion) {
    for (unsigned int iii = 0; iii != retiredListSize; ++iii) {

      // is it already there?
      if (updatedRetiredList[iii] == consideredHazard)
          break;

      // end of the list
      if (updatedRetiredList[iii] == undefinedGuard) {

        // add hazard
        updatedRetiredList[iii] = consideredHazard;

        // we are done here...
        break;
      }
    }
  }
}

template< typename GuardType >
void HazardPointer< GuardType >::EnqueueForDeletion(GuardType toRetire) {

  unsigned int myThreadId = GetCurrentThreadIndex();

  // check for invariant
  assert(myThreadId < accessorCount);

  unsigned int retiredListSize = accessorCount * guardsPerThread;

  GuardType* retiredList =
    &threadLocalRetiredLists[myThreadId * retiredListSize];

  GuardType* retiredListTemp =
    &threadLocalRetiredListsTemp[myThreadId * retiredListSize];

  // wipe my temp. retired list...
  for (unsigned int i = 0; i < retiredListSize; ++i) {
    // the list is filled always from left to right, so occurring the first
    // undefinedGuard, the remaining ones are also undefinedGuard...
    if (retiredListTemp[i] == undefinedGuard)
        break;

    retiredListTemp[i] = undefinedGuard;
  }

  // we test each hazard if it is in the union of retiredList and
  // guardedElement. If it is, it goes into the new retired list...
  for (unsigned int i = 0; i != accessorCount*guardsPerThread; ++i) {
    // consider each current active guard
    GuardType consideredHazard = guards[i].Load();
    UpdateRetiredList(retiredList, retiredListTemp, retiredListSize,
      toRetire, consideredHazard, undefinedGuard);
  }

  // now we created a a new retired list... the elements that are "removed" from
  // the old retired list can be safely deleted now...
  for (int ii = -1; ii != static_cast<int>(retiredListSize); ++ii) {
    // we iterate over the current retired list... -1 is used as dummy element
    // in the iteration, to also iterate over the pointerToRetire, which is
    // logically also part of the current retired list...

    // end of the list, stop iterating
    if (ii >= 0 && retiredList[ii] == undefinedGuard)
        break;

    GuardType toCheckIfInNewList = undefinedGuard;

    toCheckIfInNewList = (ii == -1 ? toRetire : retiredList[ii]);

    // still in the new retired list?
    bool stillInList = false;
    for (unsigned int iii = 0; iii != retiredListSize; ++iii) {
      // end of list
      if (retiredListTemp[iii] == undefinedGuard)
          break;

      if (toCheckIfInNewList == retiredListTemp[iii]) {
        // still in list, cannot delete!
        stillInList = true;
        break;
      }
    }

    if (!stillInList) {
      this->freeGuardCallback(toCheckIfInNewList);
    }
  }

  // copy the updated retired list (temp) to the retired list...
  CopyRetiredList(retiredListTemp, retiredList, retiredListSize,
    undefinedGuard);
}

} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_
