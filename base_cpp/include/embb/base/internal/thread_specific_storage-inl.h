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

#ifndef EMBB_BASE_INTERNAL_THREAD_SPECIFIC_STORAGE_INL_H_
#define EMBB_BASE_INTERNAL_THREAD_SPECIFIC_STORAGE_INL_H_

#include <embb/base/c/thread_specific_storage.h>
#include <embb/base/c/internal/unused.h>
#include <embb/base/memory_allocation.h>

#include <cassert>

namespace embb {
namespace base {

template<typename Type>
ThreadSpecificStorage<Type>::ThreadSpecificStorage()
    : rep_(), usage_flags_(NULL) {
  Prepare();
  for (unsigned int i = 0; i < rep_.size; i++) {
    rep_.values[i] = Allocation::New<Type>();
  }
}

template<typename Type>
template<typename Initializer>
ThreadSpecificStorage<Type>::ThreadSpecificStorage(Initializer initializer)
    : rep_(), usage_flags_(NULL) {
  Prepare();
  for (unsigned int i = 0; i < rep_.size; i++) {
    rep_.values[i] = Allocation::New<Type>(initializer);
  }
}

template<typename Type>
template<typename Initializer1, typename Initializer2>
ThreadSpecificStorage<Type>::ThreadSpecificStorage(
    Initializer1 initializer1, Initializer2 initializer2)
    : rep_(), usage_flags_(NULL) {
  Prepare();
  for (unsigned int i = 0; i < rep_.size; i++) {
    rep_.values[i] = Allocation::New<Type>(initializer1, initializer2);
  }
}

template<typename Type>
template<typename Initializer1, typename Initializer2, typename Initializer3>
ThreadSpecificStorage<Type>::ThreadSpecificStorage(
    Initializer1 initializer1, Initializer2 initializer2,
    Initializer3 initializer3)
    : rep_(), usage_flags_(NULL) {
  Prepare();
  for (unsigned int i = 0; i < rep_.size; i++) {
    rep_.values[i] = Allocation::New<Type>(initializer1, initializer2,
                                           initializer3);
  }
}

template<typename Type>
template<typename Initializer1, typename Initializer2, typename Initializer3,
         typename Initializer4>
ThreadSpecificStorage<Type>::ThreadSpecificStorage(
    Initializer1 initializer1, Initializer2 initializer2,
    Initializer3 initializer3, Initializer4 initializer4)
    : rep_(), usage_flags_(NULL) {
  Prepare();
  for (unsigned int i = 0; i < rep_.size; i++) {
    rep_.values[i] = Allocation::New<Type>(initializer1, initializer2,
                                           initializer3, initializer4);
  }
}

template<typename Type>
ThreadSpecificStorage<Type>::~ThreadSpecificStorage() {
  for (unsigned int i = 0; i < rep_.size; i++) {
    Type* value = static_cast<Type*>(rep_.values[i]);
    assert(value != NULL);
    Allocation::Delete(value);
  }
  embb_tss_delete(&rep_);
  Allocation::Free(usage_flags_);
}

template<typename Type>
Type& ThreadSpecificStorage<Type>::Get() {
  Type* value = static_cast<Type*>(embb_tss_get(&rep_));
  if (value == NULL) {
    EMBB_THROW(ErrorException, "No thread index could be obtained");
  }
  unsigned int thread_index = 0;
  int status = embb_internal_thread_index(&thread_index);
  assert(status == EMBB_SUCCESS);
  EMBB_UNUSED_IN_RELEASE(status);
  usage_flags_[thread_index] = true;
  return *value;
}

template<typename Type>
const Type& ThreadSpecificStorage<Type>::Get() const {
  const Type* value = static_cast<Type*>(embb_tss_get(&rep_));
  if (value == NULL) {
    EMBB_THROW(ErrorException, "No thread index could be obtained");
  }
  unsigned int thread_index = 0;
  int status = embb_internal_thread_index(&thread_index);
  assert(status == EMBB_SUCCESS);
  EMBB_UNUSED_IN_RELEASE(status);
  usage_flags_[thread_index] = true;
  return *value;
}

template<typename Type>
void ThreadSpecificStorage<Type>::Prepare() {
  int status = embb_tss_create(&rep_);
  if (status == EMBB_NOMEM) {
    EMBB_THROW(NoMemoryException, "Not enough memory to allocate "
      "thread-specific storage");
  }
  usage_flags_ = static_cast<bool*>(
                 Allocation::Allocate(sizeof(bool) * rep_.size));
  for (unsigned int i = 0; i < rep_.size; i++) {
    usage_flags_[i] = false;
  }
}

} // namespace base
} // namespace embb

#endif  // EMBB_BASE_INTERNAL_THREAD_SPECIFIC_STORAGE_INL_H_
