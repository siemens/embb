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

#ifndef EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_BASE_H_
#define EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_BASE_H_

#include <stdint.h>
#include <string.h>
#include <cstddef>
#include <cassert>
#include <utility>
#include <algorithm>

#include <embb/base/internal/atomic/atomic_utility.h>
#include <embb/base/internal/atomic/atomic_implementation.h>

namespace embb {
namespace base {
namespace internal {
namespace atomic {

template<typename BaseType>
class AtomicBase {
  // The copy constructor is undefined, since it cannot be implemented
  // atomically.
  AtomicBase(const AtomicBase&);
  // The assignment operator is undefined, since it cannot be implemented
  // atomically.
  AtomicBase& operator=(const AtomicBase&);

 protected:
  typedef typename embb::base::internal::atomic::
    AtomicTraits<sizeof(BaseType)>::
    AtomicType AtomicType;

  typedef typename embb::base::internal::atomic::
    AtomicTraits<sizeof(BaseType)>::
    NativeType NativeType;

  mutable AtomicType AtomicValue;

#ifdef EMBB_THREADING_ANALYSIS_MODE
  mutable embb_mutex_t internal_mutex;
#endif

 public:
  /**
   * Default constructor.
   * Initializes the object with zero.
   */
  AtomicBase();

  /**
   * Constructor.
   * Initializes the object with the passed value.
   *
   * \param val Initial value
   */
  explicit AtomicBase(BaseType val);

  /**
   * Destructor.
   */
  ~AtomicBase();

  // The members below are documented in atomic.h
  BaseType operator=(BaseType val);
  operator BaseType() const;
  bool IsLockFree() const;
  bool IsArithmetic() const;
  bool IsInteger() const;
  bool IsPointer() const;
  void Store(BaseType val);
  BaseType Load() const;
  BaseType Swap(BaseType val);
  bool CompareAndSwap(BaseType& expected, BaseType desired);
};

template<typename BaseType>
inline AtomicBase<BaseType>::AtomicBase() : AtomicValue(0) {
  EMBB_ATOMIC_MUTEX_INIT(internal_mutex);
}

template<typename BaseType>
inline AtomicBase<BaseType>::AtomicBase(BaseType val) /*: AtomicValue(val)*/ {
  EMBB_ATOMIC_MUTEX_INIT(internal_mutex);
  memcpy(&AtomicValue, &val, sizeof(AtomicValue));
}

template<typename BaseType>
inline AtomicBase<BaseType>::~AtomicBase() {
  EMBB_ATOMIC_MUTEX_DESTROY(internal_mutex);
}

template<typename BaseType>
inline BaseType AtomicBase<BaseType>::operator=(BaseType val) {
  Store(val);
  return val;
}

template<typename BaseType>
inline AtomicBase<BaseType>::operator BaseType() const {
  return Load();
}

template<typename BaseType>
inline bool AtomicBase<BaseType>::IsArithmetic() const {
  return false;
}

template<typename BaseType>
inline bool AtomicBase<BaseType>::IsInteger() const {
  return false;
}

template<typename BaseType>
inline bool AtomicBase<BaseType>::IsPointer() const {
  return false;
}

template<typename BaseType>
inline void AtomicBase<BaseType>::Store(BaseType val) {
  NativeType storage_value;
  // Justification for using memcpy instead of pointer casts
  // or union type punning:
  //  - with strict aliasing, type punning using pointer casts or
  //    unions is unsafe
  //  - memcpy is not slower, as the compiler will optimize it away
  //    anyway...
  memcpy(&storage_value, &val, sizeof(storage_value));

  EMBB_ATOMIC_MUTEX_LOCK(internal_mutex);
  store_implementation< AtomicType, NativeType >
    ::Store(&AtomicValue, storage_value);
  EMBB_ATOMIC_MUTEX_UNLOCK(internal_mutex);
}

template<typename BaseType>
inline BaseType AtomicBase<BaseType>::Load() const {
  BaseType return_value;

  EMBB_ATOMIC_MUTEX_LOCK(internal_mutex);
  NativeType storage_value =
    load_implementation< AtomicType, NativeType >::Load(&AtomicValue);
  EMBB_ATOMIC_MUTEX_UNLOCK(internal_mutex);

  memcpy(&return_value, &storage_value, sizeof(return_value));

  return return_value;
}

template<typename BaseType>
inline BaseType AtomicBase<BaseType>::Swap(BaseType val) {
  NativeType storage_value;
  BaseType return_value;

  memcpy(&storage_value, &val, sizeof(storage_value));

  EMBB_ATOMIC_MUTEX_LOCK(internal_mutex);
  NativeType storage_value2 = swap_implementation< AtomicType, NativeType >
    ::Swap(&AtomicValue, storage_value);
  EMBB_ATOMIC_MUTEX_UNLOCK(internal_mutex);

  memcpy(&return_value, &storage_value2, sizeof(return_value));

  return return_value;
}

template<typename BaseType>
inline bool AtomicBase<BaseType>::
CompareAndSwap(BaseType& expected, BaseType desired) {
  NativeType native_expected;
  NativeType native_desired;

  memcpy(&native_expected, &expected, sizeof(expected));
  memcpy(&native_desired, &desired, sizeof(desired));

  EMBB_ATOMIC_MUTEX_LOCK(internal_mutex);
  bool return_val =
    (compare_and_swap_implementation<AtomicType, NativeType>::
    compare_and_swap(&AtomicValue, &native_expected, native_desired)) !=0
    ? true : false;
  EMBB_ATOMIC_MUTEX_UNLOCK(internal_mutex);

  memcpy(&expected, &native_expected, sizeof(expected));

  return return_val;
}

}  // namespace atomic
}  // namespace internal
}  // namespace base
}  // namespace embb

#endif // EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_BASE_H_
