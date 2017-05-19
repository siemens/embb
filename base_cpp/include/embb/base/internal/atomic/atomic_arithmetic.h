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

#ifndef EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_ARITHMETIC_H_
#define EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_ARITHMETIC_H_

#include <stdint.h>
#include <string.h>
#include <cstddef>
#include <cassert>
#include <utility>

#include <embb/base/internal/atomic/atomic_base.h>

namespace embb {
namespace base {
namespace internal {
namespace atomic {

/**
 * Subclass that implements arithmetic operations.
 * This class provides arithmetic operations for integers and pointers.
 *
 * \tparam BaseType Underlying type (must be an integral type or a
 *         non-void pointer type)
 * \tparam DifferenceType Difference type (\c ptrdiff_t for pointers,
 *         otherwise \c BaseType)
 * \tparam Stride Stride (in case of pointers, size of the objects
 *         pointed to, otherwise 1)
 */
template<typename BaseType, typename DifferenceType, size_t Stride>
class AtomicArithmetic : public AtomicBase<BaseType> {
 protected:
  typedef typename AtomicBase<BaseType>::AtomicType AtomicType;

  typedef typename AtomicBase<BaseType>::NativeType NativeType;

 public:
  // See base class
  AtomicArithmetic();
  // See base class
  explicit AtomicArithmetic(BaseType val);

  // See base class
  BaseType operator=(BaseType val);

  // See base class
  bool IsArithmetic() const;

  // The methods below are documented in atomic.h
  BaseType FetchAndAdd(DifferenceType val);
  BaseType FetchAndSub(DifferenceType val);
  BaseType operator++(int);
  BaseType operator--(int);
  BaseType operator++();
  BaseType operator--();
  BaseType operator+=(DifferenceType val);
  BaseType operator-=(DifferenceType val);
};

template<typename BaseType, typename DifferenceType, size_t Stride>
inline AtomicArithmetic<BaseType, DifferenceType, Stride>::AtomicArithmetic()
: AtomicBase<BaseType>() {}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline AtomicArithmetic<BaseType, DifferenceType, Stride>::
AtomicArithmetic(BaseType val) : AtomicBase<BaseType>(val) {}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType
AtomicArithmetic<BaseType, DifferenceType, Stride>::operator=(BaseType val) {
  return AtomicBase<BaseType>::operator=(val);
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline bool AtomicArithmetic<BaseType, DifferenceType, Stride>::
IsArithmetic() const {
  return true;
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
FetchAndAdd(DifferenceType val) {
  /*
  Despite the (at first sight) complexity of different function variables
  and memcpy calls, the compiler is smart enough to optimize this...
  A call to a++ (invoking this function), where "a" is an atomic integer
  variable, translates to (tested using MSVC):
  00CA3744  mov         edx,1
  00CA3749  lea         ecx,[a]
  00CA374C  call        \embb_internal__atomic_fetch_and_add_4_asm\8 (0CA381Fh)
  */
  BaseType return_value;
  DifferenceType desired = static_cast<DifferenceType>(Stride)*val;

  NativeType native_desired;
  memcpy(&native_desired, &desired, sizeof(desired));

  EMBB_ATOMIC_MUTEX_LOCK(this->internal_mutex);
  NativeType storage_value = fetch_and_add_implementation<AtomicType, NativeType>::
    fetch_and_add(&this->AtomicValue, native_desired);
  EMBB_ATOMIC_MUTEX_UNLOCK(this->internal_mutex);

  memcpy(&return_value, &storage_value, sizeof(return_value));
  return return_value;
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
FetchAndSub(DifferenceType val) {
  return FetchAndAdd(-val);
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
operator++(int) {
  return FetchAndAdd(1);
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
operator--(int) {
  return FetchAndSub(1);
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
operator++() {
  return FetchAndAdd(1) + 1;
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
operator--() {
  return FetchAndSub(1) - 1;
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
operator+=(DifferenceType val) {
  return FetchAndAdd(val) + val;
}

template<typename BaseType, typename DifferenceType, size_t Stride>
inline BaseType AtomicArithmetic<BaseType, DifferenceType, Stride>::
operator-=(DifferenceType val) {
  return FetchAndSub(val) - val;
}

}  // namespace atomic
}  // namespace internal
}  // namespace base
}  // namespace embb

#endif  // EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_ARITHMETIC_H_
