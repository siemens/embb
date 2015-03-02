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
template< typename ElementT >
FixedSizeList<ElementT>::FixedSizeList(size_t max_size) :
  max_size(max_size),
  size(0) {
  elementsArray = static_cast<ElementT*>(
    embb::base::Allocation::Allocate(sizeof(ElementT) *
    max_size));
}

template< typename ElementT >
inline size_t FixedSizeList<ElementT>::GetSize() const {
  return size;
}

template< typename ElementT >
inline size_t FixedSizeList<ElementT>::GetMaxSize() const {
  return max_size;
}

template< typename ElementT >
inline void FixedSizeList<ElementT>::clear() {
  size = 0;
}

template< typename ElementT >
typename FixedSizeList<ElementT>::iterator
FixedSizeList<ElementT>::begin() const {
  return &elementsArray[0];
}

template< typename ElementT >
typename FixedSizeList<ElementT>::iterator
FixedSizeList<ElementT>::end() const {
  return &elementsArray[size];
}

template< typename ElementT >
FixedSizeList< ElementT > &
FixedSizeList<ElementT>::operator= (const FixedSizeList & other) {
  size = 0;

  if (max_size < other.size) {
    EMBB_THROW(embb::base::ErrorException, "Copy target to small");
  }

  for (const_iterator it = other.begin(); it != other.end(); ++it) {
    PushBack(*it);
  }
  return *this;
}

template< typename ElementT >
bool FixedSizeList<ElementT>::PushBack(ElementT const el) {
  if (size + 1 > max_size) {
    return false;
  }
  elementsArray[size] = el;
  size++;
  return true;
}

template< typename ElementT >
FixedSizeList<ElementT>::~FixedSizeList() {
  embb::base::Allocation::Free(elementsArray);
}

template< typename GuardType >
bool HazardPointerThreadEntry<GuardType>::IsActive() {
  return is_active;
}

template< typename GuardType >
bool HazardPointerThreadEntry<GuardType>::TryReserve() {
  bool expected = false;
  return is_active.CompareAndSwap(expected, true);
}

template< typename GuardType >
void HazardPointerThreadEntry<GuardType>::Deactivate() {
  is_active = false;
}

template< typename GuardType >
size_t HazardPointerThreadEntry<GuardType>::GetRetiredCounter() {
  return retired_list.GetSize();
}

template< typename GuardType >
FixedSizeList< GuardType >& HazardPointerThreadEntry<GuardType>::
GetRetired() {
  return retired_list;
}

template< typename GuardType >
FixedSizeList< GuardType >& HazardPointerThreadEntry<GuardType>::
GetRetiredTemp() {
  return retired_list_temp;
}

template< typename GuardType >
FixedSizeList< GuardType >& HazardPointerThreadEntry<GuardType>::
GetHazardTemp() {
  return hazard_pointer_list_temp;
}

template< typename GuardType >
void HazardPointerThreadEntry<GuardType>::
SetRetired(internal::FixedSizeList< GuardType > const & retired_list) {
  this->retired_list = retired_list;
}

template< typename GuardType >
HazardPointerThreadEntry<GuardType>::
HazardPointerThreadEntry(GuardType undefined_guard, int guards_per_thread,
  size_t max_size_retired_list) :
#ifdef EMBB_DEBUG
  who_is_scanning(-1),
#endif
  undefined_guard(undefined_guard),
  guards_per_thread(guards_per_thread),
  max_size_retired_list(max_size_retired_list),
  // initially, each potential thread is active... if that is not the case
  // another thread could call "HelpScan", and block this thread in making
  // progress.
  // Still, threads can be leave the hazard pointer processing (deactivation),
  // but this can only be done once, i.e., this is not revertable...
  is_active(1),
  retired_list(max_size_retired_list),
  retired_list_temp(max_size_retired_list),
  hazard_pointer_list_temp(embb::base::Thread::GetThreadsMaxCount() *
    guards_per_thread) {
  // Initialize guarded pointer list
  guarded_pointers = static_cast<embb::base::Atomic<GuardType>*>
    (embb::base::Allocation::Allocate(
    sizeof(embb::base::Atomic<GuardType>)*guards_per_thread));

  for (int i = 0; i != guards_per_thread; ++i) {
    new (static_cast<void*>(&guarded_pointers[i]))
      embb::base::Atomic<GuardType>(undefined_guard);
  }
}

template< typename GuardType >
HazardPointerThreadEntry<GuardType>::~HazardPointerThreadEntry() {
  for (int i = 0; i != guards_per_thread; ++i) {
    guarded_pointers[i].~Atomic();
  }

  embb::base::Allocation::Free(guarded_pointers);
}

template< typename GuardType >
GuardType HazardPointerThreadEntry<GuardType>::GetGuard(int pos) const {
  return guarded_pointers[pos];
}

template< typename GuardType >
void HazardPointerThreadEntry<GuardType>::AddRetired(GuardType pointerToGuard) {
  retired_list.PushBack(pointerToGuard);
}

template< typename GuardType >
void HazardPointerThreadEntry<GuardType>::
GuardPointer(int guardNumber, GuardType pointerToGuard) {
  guarded_pointers[guardNumber] = pointerToGuard;
}

template< typename GuardType >
void HazardPointerThreadEntry<GuardType>::SetActive(bool active) {
  is_active = active;
}

template< typename GuardType >
unsigned int HazardPointer< GuardType >::GetCurrentThreadIndex() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);

  if (return_val != EMBB_SUCCESS)
    EMBB_THROW(embb::base::ErrorException, "Could not get thread id!");

  return thread_index;
}
template< typename GuardType >
bool HazardPointer< GuardType >::IsThresholdExceeded() {
  double retiredCounterLocThread =
    static_cast<double>(GetHazardPointerElementForCurrentThread().
    GetRetiredCounter());

  return (retiredCounterLocThread >=
    RETIRE_THRESHOLD *
    static_cast<double>(active_hazard_pointer)*
    static_cast<double>(guards_per_thread));
}

template< typename GuardType >
size_t HazardPointer< GuardType >::GetActiveHazardPointers() {
  return active_hazard_pointer;
}
template< typename GuardType >
typename HazardPointer< GuardType >::HazardPointerThreadEntry_t &
HazardPointer< GuardType >::GetHazardPointerElementForCurrentThread() {
  // For each thread, there is a slot in the hazard pointer array.
  // Initially, the active flag of a hazard pointer entry is false.
  // Only the respective thread changes the flag from true to false.
  // This means that the current thread tells that he is about to
  // stop operating, and the others are responsible for his retired
  // list.

  return hazard_pointer_thread_entry_array[GetCurrentThreadIndex()];
}

template< typename GuardType >
void HazardPointer< GuardType >::HelpScan() {
  // This is a little bit different than in the paper. In the paper,
  // the retired nodes from other threads are added to our retired list.
  // To be able to give a bound on memory consumption, we execute scan
  // for those threads, without moving elements. The effect shall be
  // the same.

  for (size_t i = 0; i != hazard_pointers; ++i) {
    // Try to find non active lists...
    if (!hazard_pointer_thread_entry_array[i].IsActive() &&
      hazard_pointer_thread_entry_array[i].TryReserve()) {
      // Here: grab retired things, first check if there are any...
      if (hazard_pointer_thread_entry_array[i].GetRetiredCounter() > 0) {
        Scan(&hazard_pointer_thread_entry_array[i]);
      }

      // We are done, mark it as deactivated again
      hazard_pointer_thread_entry_array[i].Deactivate();
    }
  }
}

template< typename GuardType >
void HazardPointer< GuardType >::
Scan(HazardPointerThreadEntry_t* currentHazardPointerEntry) {
#ifdef EMBB_DEBUG
  // scan should only be executed by one thread at a time, otherwise we have
  // a bug... this assertions checks that
  int expected = -1;
  if (!currentHazardPointerEntry->GetScanningThread().CompareAndSwap(
    expected, static_cast<int>(GetCurrentThreadIndex()))) {
    assert(false);
  }
#endif
  // In this function, we compute the intersection between local retired
  // pointers and all hazard pointers. This intersection cannot be deleted and
  // forms the new local retired pointers list.
  // It is assumed that the union of all retired pointers contains no two
  // pointers with the same value. However, the union of all hazard guards
  // might.

  // Here, we store the temporary hazard pointers. We have to store them,
  // as iterating multiple time over them might be expensive, as this
  // atomic array is shared between threads.
  currentHazardPointerEntry->GetHazardTemp().clear();

  // Get all active hazard pointers!
  for (unsigned int i = 0; i != hazard_pointers; ++i) {
    // Only consider guards of active threads
    if (hazard_pointer_thread_entry_array[i].IsActive()) {
      // For each guard in an hazard pointer entry
      for (int pos = 0; pos != guards_per_thread; ++pos) {
        GuardType guard = hazard_pointer_thread_entry_array[i].GetGuard(pos);

        // UndefinedGuard means not guarded
        if (guard == undefined_guard)
          continue;

        currentHazardPointerEntry->GetHazardTemp().PushBack(guard);
      }
    }
  }

  currentHazardPointerEntry->GetRetiredTemp().clear();

  // Sort them, we will do a binary search on each entry from the retired list
  std::sort(
    currentHazardPointerEntry->GetHazardTemp().begin(),
    currentHazardPointerEntry->GetHazardTemp().end());

  for (
    EMBB_CONTAINERS_CPP_DEPENDANT_TYPENAME FixedSizeList< GuardType >::iterator
      it = currentHazardPointerEntry->GetRetired().begin();
  it != currentHazardPointerEntry->GetRetired().end(); ++it) {
    if (false == ::std::binary_search(
      currentHazardPointerEntry->GetHazardTemp().begin(),
      currentHazardPointerEntry->GetHazardTemp().end(), *it)) {
      this->free_guard_callback(*it);
    } else {
      currentHazardPointerEntry->GetRetiredTemp().PushBack(*it);
    }
  }
  currentHazardPointerEntry->SetRetired(
    currentHazardPointerEntry->GetRetiredTemp());

#ifdef EMBB_DEBUG
  currentHazardPointerEntry->GetScanningThread().Store(-1);
#endif
}

template< typename GuardType >
size_t HazardPointer< GuardType >::GetRetiredListMaxSize() const {
  return static_cast<size_t>(RETIRE_THRESHOLD *
        static_cast<double>(embb::base::Thread::GetThreadsMaxCount()) *
        static_cast<double>(guards_per_thread)) + 1;
}

template< typename GuardType >
HazardPointer< GuardType >::HazardPointer(
  embb::base::Function<void, GuardType> free_guard_callback,
  GuardType undefined_guard, int guards_per_thread) :
  undefined_guard(undefined_guard),
  guards_per_thread(guards_per_thread),
  //initially, all potential hazard pointers are active...
  active_hazard_pointer(embb::base::Thread::GetThreadsMaxCount()),
  free_guard_callback(free_guard_callback) {
  hazard_pointers = embb::base::Thread::GetThreadsMaxCount();

  hazard_pointer_thread_entry_array = static_cast<HazardPointerThreadEntry_t*>(
    embb::base::Allocation::Allocate(sizeof(HazardPointerThreadEntry_t) *
    hazard_pointers));

  for (size_t i = 0; i != hazard_pointers; ++i) {
    new (static_cast<void*>(&(hazard_pointer_thread_entry_array[i])))
      HazardPointerThreadEntry_t(undefined_guard, guards_per_thread,
      GetRetiredListMaxSize());
  }
}

template< typename GuardType >
HazardPointer< GuardType >::~HazardPointer() {
  for (size_t i = 0; i != hazard_pointers; ++i) {
    hazard_pointer_thread_entry_array[i].~HazardPointerThreadEntry_t();
  }

  embb::base::Allocation::Free(static_cast < void* >
    (hazard_pointer_thread_entry_array));
}

template< typename GuardType >
void HazardPointer< GuardType >::DeactivateCurrentThread() {
  HazardPointerThreadEntry_t* current_thread_entry =
    &hazard_pointer_thread_entry_array[GetCurrentThreadIndex()];

  // Deactivating a non-active hazard pointer entry has no effect!
  if (!current_thread_entry->IsActive()) {
    return;
  } else {
    current_thread_entry->SetActive(false);
    active_hazard_pointer--;
  }
}

template< typename GuardType >
void HazardPointer< GuardType >::GuardPointer(int guardPosition,
  GuardType guardedElement) {
  GetHazardPointerElementForCurrentThread().GuardPointer(
    guardPosition, guardedElement);
}

template< typename GuardType >
void HazardPointer< GuardType >::EnqueuePointerForDeletion(
  GuardType guardedElement) {
  GetHazardPointerElementForCurrentThread().AddRetired(guardedElement);
  if (IsThresholdExceeded()) {
    HazardPointerThreadEntry_t* currentHazardPointerEntry =
      &GetHazardPointerElementForCurrentThread();

    Scan(currentHazardPointerEntry);

    // Help deactivated threads to clean their retired nodes.
    HelpScan();
  }
}

template<typename GuardType>
const double embb::containers::internal::HazardPointer<GuardType>::
  RETIRE_THRESHOLD = 1.25f;
} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_HAZARD_POINTER_INL_H_
