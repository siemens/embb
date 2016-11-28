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

#ifndef EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_INTEGER_H_
#define EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_INTEGER_H_

#include <stdint.h>
#include <string.h>
#include <cstddef>
#include <cassert>
#include <utility>

#include <embb/base/internal/atomic/atomic_arithmetic.h>

namespace embb {
namespace base {
namespace internal {
namespace atomic {

/**
 * Subclass for integers.
 * This class provides bitwise Boolean operations to manipulate integers.
 *
 * \tparam BaseType Underlying type (must be an integral type)
 */
template<typename BaseType>
class AtomicInteger : public AtomicArithmetic<BaseType, BaseType, 1u> {
 private:
  typedef typename AtomicArithmetic<BaseType, BaseType, 1u>::
    AtomicType AtomicType;

  typedef typename AtomicArithmetic<BaseType, BaseType, 1u>::
    NativeType NativeType;

 public:
  // See base class
  AtomicInteger();
  // See base class
  explicit AtomicInteger(BaseType val);

  // See base class
  BaseType operator=(BaseType val);

  // See base class
  bool IsInteger() const;

  // The methods below are documented in atomic.h
  void operator&=(BaseType val);
  void operator|=(BaseType val);
  void operator^=(BaseType val);
};

template<typename BaseType>
inline AtomicInteger<BaseType>::AtomicInteger() :
AtomicArithmetic<BaseType, BaseType, 1u>() {}

template<typename BaseType>
inline AtomicInteger<BaseType>::AtomicInteger(BaseType val) :
AtomicArithmetic<BaseType, BaseType, 1u>(val) {}

template<typename BaseType>
inline BaseType AtomicInteger<BaseType>::operator=(BaseType val) {
  return AtomicArithmetic<BaseType, BaseType, 1u>::operator=(val);
}

template<typename BaseType>
inline bool AtomicInteger<BaseType>::IsInteger() const {
  return true;
}

template<typename BaseType>
inline void AtomicInteger<BaseType>::operator&=(BaseType val) {
  NativeType native_operand;
  memcpy(&native_operand, &val, sizeof(val));

  EMBB_ATOMIC_MUTEX_LOCK(this->internal_mutex);
  and_assign_implementation<AtomicType, NativeType>::
    and_assign(&this->AtomicValue, native_operand);
  EMBB_ATOMIC_MUTEX_UNLOCK(this->internal_mutex);
}

template<typename BaseType>
inline void AtomicInteger<BaseType>::operator|=(BaseType val) {
  NativeType native_operand;
  memcpy(&native_operand, &val, sizeof(val));

  EMBB_ATOMIC_MUTEX_LOCK(this->internal_mutex);
  or_assign_implementation<AtomicType, NativeType>::
    or_assign(&this->AtomicValue, native_operand);
  EMBB_ATOMIC_MUTEX_UNLOCK(this->internal_mutex);
}

template<typename BaseType>
inline void AtomicInteger<BaseType>::operator^=(BaseType val) {
  NativeType native_operand;
  memcpy(&native_operand, &val, sizeof(val));

  EMBB_ATOMIC_MUTEX_LOCK(this->internal_mutex);
  xor_assign_implementation<AtomicType, NativeType>::
    xor_assign(&this->AtomicValue, native_operand);
  EMBB_ATOMIC_MUTEX_UNLOCK(this->internal_mutex);
}

}  // namespace atomic
}  // namespace internal
}  // namespace base
}  // namespace embb

#endif //  EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_INTEGER_H_
