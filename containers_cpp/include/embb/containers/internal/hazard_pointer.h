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

#ifndef EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_H_
#define EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_H_

#include <embb/base/atomic.h>
#include <embb/base/thread_specific_storage.h>
#include <embb/base/thread.h>
#include <embb/containers/wait_free_array_value_pool.h>
#include <embb/base/function.h>
#include <algorithm>

#if defined(EMBB_PLATFORM_COMPILER_MSVC)
#define EMBB_CONTAINERS_CPP_DEPENDANT_TYPENAME
#else
#define EMBB_CONTAINERS_CPP_DEPENDANT_TYPENAME typename
#endif

// forward declaration for white-box test, used in friend declaration of
// HazardPointer class.
namespace embb {
namespace containers {
namespace test {
class HazardPointerTest2;
}
}
}

namespace embb {
namespace containers {
namespace internal {
/**
 * This class contains a hazard pointer implementation following publication:
 *
 * Maged M. Michael. "Hazard pointers: Safe memory reclamation for lock-free
 * objects." IEEE Transactions on Parallel and Distributed Systems, 15.6 (2004)
 * : 491-504.
 *
 * Hazard pointers are a wait-free memory reclamation scheme for lock-free
 * algorithms. Loosely speaking, they act as garbage collector. The release of
 * objects contained within the memory, managed by the hazard pointer class, is
 * intercepted and possibly delayed to avoid concurrency bugs.
 *
 * Before accessing an object, threads announce their intention to do so (i.e.
 * the intention to dereference the respective pointer) to the hazard pointer
 * class. This is called guarding. From now on, the hazard pointer class will
 * prohibit the release or reuse of the guarded object. This is necessary, to
 * assure that the object is not released or reused while it is accessed and to
 * assure that it has not unnoticed changed (effectively avoiding the ABA
 * problem).
 *
 * Note that after guarding an object, a consecutive check that the object (i.e.
 * its pointer) is still valid is necessary; the object release could already
 * have been started when guarding the object. Guarding is repeated, until this
 * check eventually succeeds. Note that this "guard-and-check" loop makes the
 * usage of the hazard pointer class lock-free, even though its implementation
 * is wait-free.
 *
 * Internally, guarding is realized by providing each thread slots, where
 * pointers can be placed that should not be freed (so called guards). When
 * trying to release an object, it is checked if the object's pointer is
 * guarded, and if so this object is not released, but instead put into a
 * retired list for later release, when all guards for this object have been
 * removed.
 *
 * In contrast to the original implementation, our implementation consumes only
 * fixed-size memory. Note that the number of threads accessing the hazard
 * pointer object accounts quadratic for the memory consumption: managed objects
 * are provided from outside and the number of accessors accounts quadric for
 * the minimum count of those objects.
 *
 * Also in contrast to the original implementation, we do not provide a HelpScan
 * functionality, which gives threads the possibility, to not participate in the
 * garbage collection anymore: other threads will help to clean-up the objects
 * protected by the exiting thread. The reason is that the only use-case would
 * be a crashing thread, not participating anymore. However, as the thread has
 * to signal its exit himself, this is not possible to realize anyways. In the
 * end, it is still guaranteed that all memory is properly returned (in the
 * destructor).
 *
 * Additionally, the original implementation holds a threshold, which determines
 * when objects shall be freed. In this implementation, we free whenever it is
 * possibly to do so, as we want to keep the memory footprint as low as
 * possible. We also don't see a performance drop in the current algorithms that
 * are using hazard pointers, when not using a threshold.
 *
 * \tparam GuardType the type of the guards. Usually the pointer type of some
 *         object to protect.
 */
template< typename GuardType >
class HazardPointer  {
 public:
  /**
   * The user of the hazard pointer class has to provide the memory that is
   * managed here. The user has to take into account that releasing of memory
   * might be delayed. He has therefore to provide more memory than he wants to
   * guarantee at each point in time. More specific, on top of the guaranteed
   * count of objects, he has to provide the additional count of objects that
   * can be (worst-case) contained in the retired lists and therefore are not
   * released yet. The size sum of all retired lists is guardsPerThread *
   * accessorCount * accessorCount, which is computed using this function. So
   * the result of function denotes to the user, how many objects he has to
   * allocate additionally to the guaranteed count.
   *
   * \waitfree
   */
  static size_t ComputeMaximumRetiredObjectCount(
    size_t guardsPerThread,
      /**<[IN] the count of guards per thread*/
      int accessors = -1
      /**<[IN] Number of accessors. Determines, how many threads will access
               the hazard pointer object. Default value -1 will allow the
               maximum amount of threads as defined with
               \c embb::base::Thread::GetThreadsMaxCount()*/
      );

  /**
   * Initializes the hazard pointer object
   *
   * \notthreadsafe
   *
   * \memory We dynamically allocate the following:
   *
   * (sizeof(Atomic<int>) * accessors) + (sizeof(Atomic<GuardType>) *
   * guards_per_thread * accessors) + (2*sizeof(GuardType) *
   * guards_per_thread * accessors^2)
   *
   * The last addend is the dominant one, as accessorCount accounts
   * quadratically for it.
   */
  HazardPointer(
    embb::base::Function<void, GuardType> free_guard_callback,
    /**<[IN] Callback to the function that shall be called when a retired
             guard can be deleted */
    GuardType undefined_guard,
    /**<[IN] The guard value denoting "not guarded"*/
    int guards_per_thread,
    /**<[IN] Number of guards per thread*/
    int accessors = -1
    /**<[IN] Number of accessors. Determines, how many threads will access
              this hazard pointer object. Default value -1 will allow the
              maximum amount of threads as defined with
              \c embb::base::Thread::GetThreadsMaxCount()*/
    );

  /**
   * Deallocates internal data structures. Additionally releases all objects
   * currently held in the retired lists, using the release functor passed in
   * the constructor.
   *
   * \notthreadsafe
   */
  ~HazardPointer();

  /**
   * Guards \c to_guard. If the guarded_element is passed to \c EnqueueForDeletion
   * it is prevented from release from now on. The user must have a check that
   * EnqueueForDeletion has not been called on to_guard, before the guarding took
   * effect.
   *
   * \waitfree
   */
  void Guard(
    int guard_position,
    /**<[IN] position to place guard*/
    GuardType to_guard
    /**<[IN] element to guard*/
    );

  /**
   * Enqueue guarded element for deletion. If not guarded, it is deleted
   * immediately. If it is guarded, it is added to a thread local retired list,
   * and deleted in a subsequent call to \c EnqueueForDeletion, when no guard is
   * placed on it anymore.
   */
  void EnqueueForDeletion(
    GuardType guarded_element
    /**<[IN] element to logically delete*/
    );

  /**
   * Explicitly remove guard from thread local slot.
   *
   * \waitfree
   */
  void RemoveGuard(int guard_position);

 private:
  /**
   * HazardPointerTest2 is a white-box test, needing access to private members
   * of this class. So declaring it as friend.
   */
  friend class embb::containers::test::HazardPointerTest2;

  /**
   * This number determines the amount of maximal accessors (threads) that
   * will access this hazard pointer instance. Note that a thread once
   * accessing this object will be permanently count as accessor, even if not
   * participating anymore. If too many threads access this object, an
   * exception is thrown.
   */
  unsigned int max_accessors_count_;

  /**
   * The guard value denoting "not guarded"
   */
  GuardType undefined_guard_;

  /**
   * The maximal count of guards that can be set per thread.
   */
  int max_guards_per_thread_;

  /**
   * The functor that is called to release an object. This is called by this
   * class, when it is safe to do so, i.e., no thread accesses this object
   * anymore.
   */
  embb::base::Function<void, GuardType> release_object_callback_;

  /**
   * Mapping from EMBB thread id to hazard pointer thread ids. Hazard pointer
   * thread ids are in range [0;accesor_count-1]. The position of an EMBB thread
   * id in that array determines the respective hazard pointer thread id.
   */
  embb::base::Atomic<int>* thread_id_mapping_;

  /**
   * The hazard pointer guards, represented as array. Each thread has a fixed
   * set of slots (guardsPerThread) within this array.
   */
  embb::base::Atomic<GuardType>* guards_;

  /**
   * \see threadLocalRetiredLists documentation
   */
  GuardType* thread_local_retired_lists_temp_;

  /**
   * A list of lists, represented as single array. Each thread maintains a list
   * of retired pointers that are objects that are logically released but not
   * released because some thread placed a guard on it.
   */
  GuardType* thread_local_retired_lists_;

  /**
   * Each thread is assigned a thread index (starting with 0). Get the index of
   * the current thread. Note that this is not the global index, but an hazard
   * pointer class internal one. The user is free to define less accessors than
   * the amount of default threads. This is useful, as the number of accessors
   * accounts quadratic for the memory consumption, so the user should have the
   * possibility to avoid memory wastage when only having a small, fixed size,
   * number of accessors.
   *
   * @return current (hazard pointer object local) thread index
   */
  unsigned int GetObjectLocalThreadIndex();

  /**
   * Copy retired list \c sourceList to retired list \c targetList
   */
  static void CopyRetiredList(
    GuardType* source_list,
    /**<[IN] the source retired list*/
    GuardType* target_list,
    /**<[IN] the target retired list*/
    unsigned int single_retired_list_size,
    /**<[IN] the size of a thread local retired list*/
    GuardType undefined_guard
    /**<[IN] the undefined guard (usually the NULL pointer)*/
    );

  static void UpdateRetiredList(
    GuardType* retired_list,
    /**<[IN] the old retired list*/
    GuardType* updated_retired_list,
    /**<[IN] the updated retired list*/
    unsigned int retired_list_size,
    /**<[IN] the size of a thread local retired list*/
    GuardType to_retire,
    /**<[IN] the element to retire*/
    GuardType considered_hazard,
    /**<[IN] the currently considered hazard*/
    GuardType undefined_guard
    /**<[IN] the undefined guard (usually the NULL pointer)*/
    );
};
} // namespace internal
} // namespace containers
} // namespace embb

#include "./hazard_pointer-inl.h"

#endif  // EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_H_
