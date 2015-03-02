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

namespace embb {
namespace containers {
namespace internal {
/**
 * A list with fixed size, implemented as an array. Replaces std::vector that
 * was used in previous hazard pointer implementation.
 *
 * Provides iterators, so we can apply algorithms from the STL.
 *
 * \tparam ElementT Type of the elements contained in the list.
 */
template< typename ElementT >
class FixedSizeList {
 private:
   /**
    * Capacity of the list
    */
  size_t max_size;

  /**
   * Size of the list
   */
  size_t size;

  /**
   * Pointer to the array containing the list
   */
  ElementT* elementsArray;

  /**
   * Copy constructor not implemented. Would require dynamic memory allocation.
   */
  FixedSizeList(
    const FixedSizeList &
    /**< [IN] Other list */);

 public:
  /**
   * Definition of an iterator
   */
  typedef ElementT * iterator;

  /**
   * Definition of a const iterator
   */
  typedef const ElementT * const_iterator;

  /**
   * Constructor, initializes list with given capacity
   */
  FixedSizeList(
    size_t max_size
    /**< [IN] Capacity of the list */);

  /**
   * Gets the current size of the list
   *
   * \return Size of the list
   */
  inline size_t GetSize() const;

  /**
   * Gets the capacity of the list
   *
   * \return The capacity of the list
   */
  inline size_t GetMaxSize() const;

  /**
   * Removes all elements from the list without changing the capacity
   */
  inline void clear();

  /**
   * Iterator pointing to the first element
   *
   * \return Begin iterator
   */
  iterator begin() const;

  /**
   * Iterator pointing beyond the last element
   *
   * \return End iterator
   */
  iterator end() const;

  /**
   * Copies the elements of another list to this list. The capacity of
   * this list has to be greater than or equal to the size of the other list.
   */
  FixedSizeList & operator=(
    const FixedSizeList & other
    /**< [IN] Other list */);

  /**
   * Appends an element to the end of the list
   *
   * \return \c false if the operation was not successful because the list is
   *         full, otherwise \c true.
   */
  bool PushBack(
    ElementT const el
    /**< [IN] Element to append to the list */);

  /**
   * Destructs the list.
   */
  ~FixedSizeList();
};

/**
 * Hazard pointer entry for a single thread. Holds the actual guards that
 * determine if the current thread is about to use the guarded pointer.
 * Guarded pointers are protected and not deleted.
 *
 * Moreover, the retired list for this thread is contained. It determines
 * the pointers that have been allocated from this thread, but are not used
 * anymore by this thread. However, another thread could have a guard on it,
 * so the pointer cannot be deleted immediately.
 *
 * For the scan operation, the intersection of the guarded pointers from all
 * threads and the retired list has to be computed. For this computation, we
 * need thread local temporary lists which are also contained here.
 *
 * \tparam GuardType The type of guard, usually a pointer.
 */
template< typename GuardType >
class HazardPointerThreadEntry {
#ifdef EMBB_DEBUG

 public:
    embb::base::Atomic<int>& GetScanningThread() {
      return who_is_scanning;
    }

 private:
    embb::base::Atomic<int> who_is_scanning;
#endif

 private:
   /**
    * Value of the undefined guard (means that no guard is set).
    */
  GuardType undefined_guard;

  /**
   * The number of guards per thread. Determines the size of the guard array.
   */
  int guards_per_thread;

  /**
   * The capacity of the retired list. It is determined by number of guards,
   * retired threshold, and maximum number of threads.
   */
  size_t max_size_retired_list;

  /**
   * Set to true if the current thread is active. Is used for a thread to
   * signal that it is leaving. If a thread has left, the other threads are
   * responsible for cleaning up its retired list.
   */
  embb::base::Atomic< bool > is_active;

  /**
   * The guarded pointer of this thread, has size \c guard_per_thread.
   */
  embb::base::Atomic< GuardType >* guarded_pointers;

  /**
   * The retired list of this thread, contains pointer that shall be released
   * when no thread holds a guard on it anymore.
   */
  FixedSizeList< GuardType > retired_list;

  /**
   * Temporary retired list, has same capacity as \c retired_list, It is used to
   * compute the intersection of all guards and the \c retired list.
   */
  FixedSizeList< GuardType > retired_list_temp;

  /**
   * Temporary guards list. Used to compute the intersection of all guards and
   * the \c retired_list.
   */
  FixedSizeList< GuardType > hazard_pointer_list_temp;

  /**
   * HazardPointerThreadEntry shall not be copied
   */
  HazardPointerThreadEntry(const HazardPointerThreadEntry&);

  /**
   * HazardPointerThreadEntry shall not be assigned
   */
  HazardPointerThreadEntry & operator= (const HazardPointerThreadEntry&);

 public:
  /**
   * Checks if current thread is active (with respect to participating in hazard
   * pointer management)
   *
   * \return \c true if the current thread is active, otherwise \c false.
   */
  bool IsActive();

  /**
   * Tries to set the active flag to true (atomically). Used if the current
   * thread is not active anymore as lock for another thread to help cleaning
   * up hazard pointer.
   *
   * \return \c true if this thread was successful setting the active flag,
   *         otherwise \c false.
   */
  bool TryReserve();

  /**
   * Deactivates current thread by atomically setting active flag to false.
   */
  void Deactivate();

  /**
   * Gets the count of current retired pointer for the current thread.
   *
   * \return Count of current retired pointer
   */
  size_t GetRetiredCounter();

  /**
   * Gets the retired list.
   *
   * \return Reference to \c retired_list
   */
  FixedSizeList< GuardType >& GetRetired();

  /**
   * Gets the temporary retired list.
   *
   * \return Reference to \c retired_list_temp
   */
  FixedSizeList< GuardType >& GetRetiredTemp();

  /**
   * Gets the temporary hazard pointer list.
   *
   * \return Reference to \c hazard_pointer_list_temp
   */
  FixedSizeList< GuardType >& GetHazardTemp();

  /**
   * Sets the retired list.
   */
  void SetRetired(
    embb::containers::internal::FixedSizeList< GuardType > const & retired_list
    /**< [IN] Retired list */);

  /**
   * Constructor
   */
  HazardPointerThreadEntry(
    GuardType undefined_guard,
    /**< [IN] Value of the undefined guard (e.g. NULL) */
    int guards_per_thread,
    /**< [IN] Number of guards per thread */
    size_t max_size_retired_list
    /**< [IN] The capacity of the retired list(s) */);

  /**
   * Destructor
   *
   * Deallocate lists
   */
  ~HazardPointerThreadEntry();

  /**
   * Gets the guard at the specified position.
   * Positions are numbered, beginning with 0.
   */
  GuardType GetGuard(
    int pos
    /**< [IN] Position of the guard */) const;

  /**
   * Adds pointer to the retired list
   */
  void AddRetired(
    GuardType pointerToGuard
    /**< [IN] Guard to retire */);

  /**
   * Guards pointer
   */
  void GuardPointer(
    int guardNumber,
    /**< [IN] Position of guard */
    GuardType pointerToGuard
    /**<[IN] Pointer to guard */);

  /**
   * Sets the current thread active, i.e., announce that the thread
   * participates in managing hazard pointer.
   */
  void SetActive(
    bool active
    /**<[IN] \c true for active, \c false for inactive */);
};

/**
 * HazardPointer implementation as presented in:
 *
 * Maged M. Michael. "Hazard pointers: Safe memory reclamation for lock-free
 * objects." IEEE Transactions on Parallel and Distributed Systems, 15.6 (2004)
 * : 491-504.
 *
 * In contrast to the original implementation, our implementation only uses
 * fixed-size memory. There is a safe upper limit, hazard pointer are guaranteed
 * to not consume more memory. Memory is allocated solely at initialization.
 *
 * Hazard pointers solve the ABA problem for lock-free algorithms. Before
 * accessing a pointer, threads announce that they want to access this pointer
 * and then check if the pointer is still valid. This announcement is done by
 * placing a guard. It is guaranteed that the pointer is not reused until all
 * threads remove their guards to this pointer. Objects, these pointers are
 * pointing to, can therefore not be deleted directly. Instead, these pointers
 * are put into a list for later deletion (retired list). Regularly, this list
 * is processed to check which pointers can be deleted. If a pointer can be
 * deleted, a callback function provided by the user is called. The user can
 * then, e.g., free the respective object, so that the pointer can be safely
 * reused.
 */
template< typename GuardType >
class HazardPointer  {
 private:
   /**
    * Concrete hazard pointer entry type
    */
  typedef HazardPointerThreadEntry < GuardType >
    HazardPointerThreadEntry_t;

  /**
   * The guard value denoting "not guarding"
   */
  GuardType undefined_guard;

  /**
   * The capacity of the retired list (safe upper bound for retired list size)
   */
  int retired_list_max_size;

  /**
   * Guards that can be set per thread
   */
  int guards_per_thread;

  /**
   * Array of HazardPointerElements. Each thread is assigned to one.
   */
  HazardPointerThreadEntry_t* hazard_pointer_thread_entry_array;

  /**
   * The threshold, determines at which size of the retired list pointers
   * are tried to be deleted.
   */
  static const double RETIRE_THRESHOLD;

  /**
   * Each thread is assigned a thread index (starting with 0).
   * Get the index of the current thread.
   */
  static unsigned int GetCurrentThreadIndex();

  /**
   * The number of hazard pointers currently active.
   */
  size_t active_hazard_pointer;

  /**
   * Count of all hazard pointers.
   */
  size_t hazard_pointers;

  /**
   * The callback that is triggered when a retired guard can be
   * freed. Usually, the user will call a free here.
   */
  embb::base::Function<void, GuardType> free_guard_callback;

  /**
   * Checks if the current size of the retired list exceeds the threshold, so
   * that each retired guard is checked for being not hazardous anymore.
   *
   * \return \c true is threshold is exceeded, otherwise \c false.
   */
  bool IsThresholdExceeded();

  /**
   * Gets the number of hazard pointe, currently active
   *
   * \return Number of active hazard pointers
   */
  size_t GetActiveHazardPointers();

  /**
   * Gets the hazard pointer entry for the current thread
   *
   * \return Hazard pointer entry for current thread
   */
  HazardPointerThreadEntry_t&
    GetHazardPointerElementForCurrentThread();

  /**
   * Threads might leave from participating in hazard pointer management.
   * This method helps all those threads processing their retired list.
   */
  void HelpScan();

  /**
   * Checks the retired list of a hazard pointer entry for elements of the
   * retired list that can be freed, and executes the delete callback for those
   * elements.
   */
  void Scan(
    HazardPointerThreadEntry_t* currentHazardPointerEntry
    /**<[IN] Hazard pointer entry that should be checked for elements that
             can be deleted*/);

 public:
  /**
   * Gets the capacity of one retired list
   *
   * \waitfree
   */
  size_t GetRetiredListMaxSize() const;

  /**
   * Initializes hazard pointer
   *
   * \notthreadsafe
   *
   * \memory
   *  - Let \c t be the number of maximal threads determined by EMBB
   *  - Let \c g be the number of guards per thread
   *  - Let \c x be 1.25*t*g + 1
   *
   * We dynamically allocate \c x*(3*t+1) elements of size \c sizeof(void*).
   */
  HazardPointer(
    embb::base::Function<void, GuardType> free_guard_callback,
    /**<[IN] Callback to the function that shall be called when a retired
             guard can be deleted */
    GuardType undefined_guard,
    /**<[IN] The guard value denoting "not guarded"*/
    int guards_per_thread
    /**<[IN] Number of guards per thread*/);

  /**
   * Deallocates lists for hazard pointer management. Note that no objects
   * currently in the retired lists are deleted. This is the responsibility
   * of the user. Usually, HazardPointer manages pointers of an object pool.
   * After destructing HazardPointer, the object pool is deleted, so that
   * everything is properly cleaned up.
   */
  ~HazardPointer();

  /**
   * Announces that the current thread stops participating in hazard pointer
   * management. The other threads now take care of his retired list.
   *
   * \waitfree
   */
  void DeactivateCurrentThread();

  /**
   * Guards \c guardedElement with the guard at position \c guardPosition
   */
  void GuardPointer(int guardPosition, GuardType guardedElement);
  /**
   * Enqueue a pointer for deletion. It is added to the retired list and
   * deleted when no thread accesses it anymore.
   */
  void EnqueuePointerForDeletion(GuardType guardedElement);
};
} // namespace internal
} // namespace containers
} // namespace embb

#include "./hazard_pointer-inl.h"

#endif  // EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_H_
