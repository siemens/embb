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

#ifndef EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_
#define EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_

namespace embb {
namespace containers {
namespace internal {
// Visual Studio is complaining that the return in the last line of this
// function is not reachable. This is true, as long as exceptions are enabled.
// Otherwise, the exception becomes an assertion and with disabling assertions,
// the code becomes reachable. So, disabling this warning.
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4702)
#endif
  template< typename GuardType >
  unsigned int HazardPointer< GuardType >::GetObjectLocalThreadIndex() {
    // first, get the EMBB native thread id.
    unsigned int embb_thread_index;

    int return_val = embb_internal_thread_index(&embb_thread_index);

    if (return_val != EMBB_SUCCESS) {
      EMBB_THROW(embb::base::ErrorException, "Could not get thread id");
    }

    // iterate over the mappings array
    for (unsigned int i = 0; i != max_accessors_count_; ++i) {
      // end of mappings? then we need to write our id
      if (thread_id_mapping_[i] == -1) {
        // try to CAS the initial value with out thread id
        int expected = -1;
        if (thread_id_mapping_[i].CompareAndSwap(expected,
          static_cast<int>(embb_thread_index))) {
          //successful, return our mapping
          return i;
        }
      }

      if (thread_id_mapping_[i] == static_cast<int>(embb_thread_index)) {
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
  void HazardPointer< GuardType >::RemoveGuard(int guard_position) {
    const unsigned int my_thread_id = GetObjectLocalThreadIndex();

    // check invariants...
    assert(guard_position < max_guards_per_thread_);
    assert(my_thread_id < max_accessors_count_);

    // set guard
    guards_[guard_position*max_accessors_count_ + my_thread_id] =
      undefined_guard_;
  }

  template< typename GuardType >
  HazardPointer< GuardType >::HazardPointer(
    embb::base::Function<void, GuardType> freeGuardCallback,
    GuardType undefined_guard, int guardsPerThread, int accessors) :
    max_accessors_count_(accessors < 0 ?
    embb::base::Thread::GetThreadsMaxCount() : accessors),
    undefined_guard_(undefined_guard),
    max_guards_per_thread_(guardsPerThread),
    release_object_callback_(freeGuardCallback),
    thread_id_mapping_(static_cast<embb::base::Atomic<int>*>(
      embb::base::Allocation::Allocate(sizeof(embb::base::Atomic<int>)
      *max_accessors_count_))),
    guards_(static_cast<embb::base::Atomic< GuardType >*>
      (embb::base::Allocation::Allocate(
      sizeof(embb::base::Atomic< GuardType >) * max_guards_per_thread_ *
      max_accessors_count_))),
    thread_local_retired_lists_temp_(static_cast<GuardType*>
      (embb::base::Allocation::Allocate(
      sizeof(GuardType) * max_guards_per_thread_ * max_accessors_count_ *
      max_accessors_count_
      ))),
    thread_local_retired_lists_(static_cast<GuardType*>
      (embb::base::Allocation::Allocate(
      sizeof(GuardType) * max_guards_per_thread_ * max_accessors_count_ *
      max_accessors_count_
      ))) {
    const unsigned int count_guards =
      max_guards_per_thread_ * max_accessors_count_;

    const unsigned int count_ret_elements =
      count_guards * max_accessors_count_;

    for (unsigned int i = 0; i != max_accessors_count_; ++i) {
      //in-place new for each cell
      new (&thread_id_mapping_[i]) embb::base::Atomic < int >(-1);
    }

    for (unsigned int i = 0; i != count_guards; ++i) {
      //in-place new for each cell
      new (&guards_[i]) embb::base::Atomic < GuardType >(undefined_guard);
    }

    for (unsigned int i = 0; i != count_ret_elements; ++i) {
      //in-place new for each cell
      new (&thread_local_retired_lists_temp_[i]) GuardType(undefined_guard);
    }

    for (unsigned int i = 0; i != count_ret_elements; ++i) {
      //in-place new for each cell
      new (&thread_local_retired_lists_[i]) GuardType(undefined_guard);
    }
  }

  template< typename GuardType >
  HazardPointer< GuardType >::~HazardPointer() {
    const unsigned int count_guards =
      max_guards_per_thread_ * max_accessors_count_;

    const unsigned int count_ret_elements =
      count_guards * max_accessors_count_;

    // Release references from all retired lists. Note that for this to work,
    // the data structure using hazard pointer has still to be active... So
    // first, the hazard pointer class shall be destructed, then the memory
    // management class (e.g. some pool). Otherwise, the hazard pointer class
    // would try to return memory to an already destructed memory manager.
    for (unsigned int i = 0; i != count_ret_elements; ++i) {
      GuardType pointerToFree =
          thread_local_retired_lists_[i];
      if (pointerToFree == undefined_guard_) {
        break;
      }
      release_object_callback_(pointerToFree);
    }

    for (unsigned int i = 0; i != max_accessors_count_; ++i) {
      thread_id_mapping_[i].~Atomic();
    }

    embb::base::Allocation::Free(thread_id_mapping_);

    for (unsigned int i = 0; i != count_guards; ++i) {
      guards_[i].~Atomic();
    }

    embb::base::Allocation::Free(guards_);

    for (unsigned int i = 0; i != count_ret_elements; ++i) {
      thread_local_retired_lists_temp_[i].~GuardType();
    }

    embb::base::Allocation::Free(thread_local_retired_lists_temp_);

    for (unsigned int i = 0; i != count_ret_elements; ++i) {
      thread_local_retired_lists_[i].~GuardType();
    }

    embb::base::Allocation::Free(thread_local_retired_lists_);
  }

  template< typename GuardType >
  void HazardPointer< GuardType >::Guard(int guardPosition,
    GuardType guardedElement) {
    const unsigned int my_thread_id = GetObjectLocalThreadIndex();

    // check invariants...
    assert(guardPosition < max_guards_per_thread_);
    assert(my_thread_id < max_accessors_count_);

    // set guard
    guards_[guardPosition*max_accessors_count_ + my_thread_id] = guardedElement;
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

  /**
   * Remark: it might be faster to just swap pointers for temp retired list and
   * retired list. However, with the current implementation (one array for all
   * retired and retired temp lists, respectively) this is not possible. This is
   * not changed until this copying accounts for a performance problem. The
   * copying is not the bottleneck currently.
   */
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
      } else {
        // we copied the whole source list, remaining values in the target
        // have to be zeroed.
        if (targetList[ii] == undefinedGuard) {
          // end of target list
          break;
        } else {
          targetList[ii] = undefinedGuard;
        }
      }
    }
  }

  template< typename GuardType >
  void HazardPointer< GuardType >::UpdateRetiredList(GuardType* retired_list,
    GuardType* updated_retired_list, unsigned int retired_list_size,
    GuardType guarded_element, GuardType considered_hazard,
    GuardType undefined_guard) {
    // no hazard set here
    if (considered_hazard == undefined_guard)
        return;

    // if this hazard is currently in the union of
    // threadLocalRetiredLists and pointerToRetire, but not yet in
    // threadLocalRetiredListsTemp, add it to that list
    bool contained_in_union = false;

    // first iterate over our retired list
    for (unsigned int i = 0; i != retired_list_size; ++i) {
      // when reaching 0, we can stop iterating (end of the "list")
      if (retired_list[i] == 0)
          break;

      // the hazard is contained in the retired list... it shall go
      // into the temp list, if not already there
      if (retired_list[i] == considered_hazard) {
        contained_in_union = true;
        break;
      }
    }

    // the union also contains pointerToRetire
    if (!contained_in_union) {
      contained_in_union = (considered_hazard == guarded_element);
    }

    // add the pointer to temp. retired list, if not already there
    if (contained_in_union) {
      for (unsigned int ii = 0; ii != retired_list_size; ++ii) {
        // is it already there?
        if (updated_retired_list[ii] == considered_hazard)
            break;

        // end of the list
        if (updated_retired_list[ii] == undefined_guard) {
          // add hazard
          updated_retired_list[ii] = considered_hazard;

          // we are done here...
          break;
        }
      }
    }
  }

  template< typename GuardType >
  void HazardPointer< GuardType >::EnqueueForDeletion(GuardType toRetire) {
    unsigned int my_thread_id = GetObjectLocalThreadIndex();

    // check for invariant
    assert(my_thread_id < max_accessors_count_);

    const unsigned int retired_list_size = max_accessors_count_ *
      max_guards_per_thread_;

    const unsigned int count_guards = max_accessors_count_ *
      max_guards_per_thread_;

    GuardType* retired_list =
      &thread_local_retired_lists_[my_thread_id * retired_list_size];

    GuardType* retired_list_temp =
      &thread_local_retired_lists_temp_[my_thread_id * retired_list_size];

    // wipe my temp. retired list...
    for (unsigned int i = 0; i < retired_list_size; ++i) {
      // the list is filled always from left to right, so occurring the first
      // undefinedGuard, the remaining ones are also undefinedGuard...
      if (retired_list_temp[i] == undefined_guard_)
          break;

      retired_list_temp[i] = undefined_guard_;
    }

    // we test each hazard if it is in the union of retiredList and
    // guardedElement. If it is, it goes into the new retired list...
    for (unsigned int i = 0; i != count_guards; ++i) {
      // consider each current active guard
      GuardType considered_hazard = guards_[i].Load();
      UpdateRetiredList(retired_list, retired_list_temp, retired_list_size,
        toRetire, considered_hazard, undefined_guard_);
    }

    int retired_list_size_signed = static_cast<int>(retired_list_size);
    assert(retired_list_size_signed >= 0);

    // now we created a a new retired list... the elements that are "removed"
    // from the old retired list can be safely deleted now...
    for (int i = -1; i != retired_list_size_signed; ++i) {
      // we iterate over the current retired list... -1 is used as dummy element
      // in the iteration, to also iterate over the pointerToRetire, which is
      // logically also part of the current retired list...

      // end of the list, stop iterating
      if (i >= 0 && retired_list[i] == undefined_guard_)
          break;

      GuardType to_check_if_in_new_list = undefined_guard_;

      to_check_if_in_new_list = (i == -1 ? toRetire : retired_list[i]);

      // still in the new retired list?
      bool still_in_list = false;
      for (unsigned int ii = 0; ii != retired_list_size; ++ii) {
        // end of list
        if (retired_list_temp[ii] == undefined_guard_)
            break;

        if (to_check_if_in_new_list == retired_list_temp[ii]) {
          // still in list, cannot delete element!
          still_in_list = true;
          break;
        }
      }

      if (!still_in_list) {
        this->release_object_callback_(to_check_if_in_new_list);
      }
    }

    // copy the updated retired list (temp) to the retired list...
    CopyRetiredList(retired_list_temp, retired_list, retired_list_size,
      undefined_guard_);
  }
} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_
