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

#ifndef EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_POINTER_H_
#define EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_POINTER_H_

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
 * Subclass for pointers.
 * This class provides dereferencing operators for pointers.
 *
 * \tparam BaseType Underlying type (must be a non-void pointer type)
 */
template<typename BaseType, typename DifferenceType, size_t S>
class AtomicPointer : public AtomicArithmetic<BaseType*, DifferenceType, S> {
 private:
  typedef typename AtomicArithmetic<BaseType*, DifferenceType, S>::
    NativeType NativeType;

 public:
  // See base class
  AtomicPointer();
  // See base class
  explicit AtomicPointer(BaseType* ptr);

  // See base class
  BaseType* operator=(BaseType* ptr);

  // See base class
  bool IsPointer() const;

  // The methods below are documented in atomic.h
  BaseType* operator->() const;
  BaseType& operator*() const;
};

template<typename BaseType, typename DifferenceType, size_t S>
inline AtomicPointer<BaseType, DifferenceType, S>::AtomicPointer() :
  AtomicArithmetic<BaseType*, DifferenceType, S>() {}

template<typename BaseType, typename DifferenceType, size_t S>
inline AtomicPointer<BaseType, DifferenceType, S>::
  AtomicPointer(BaseType* val) :
  AtomicArithmetic<BaseType*, DifferenceType, S>(val) {}

template<typename BaseType, typename DifferenceType, size_t S>
inline BaseType* AtomicPointer<BaseType, DifferenceType, S>::
  operator=(BaseType* val) {
  return AtomicArithmetic<BaseType*, DifferenceType, S>::
    operator=(val);
}

template<typename BaseType, typename DifferenceType, size_t S>
inline bool AtomicPointer<BaseType, DifferenceType, S>::
  IsPointer() const {
  return true;
}

template<typename BaseType, typename DifferenceType, size_t S>
inline BaseType* AtomicPointer<BaseType, DifferenceType, S>::
  operator->() const {
  return this->Load();
}

template<typename BaseType, typename DifferenceType, size_t S>
inline BaseType& AtomicPointer<BaseType, DifferenceType, S>::
  operator*() const {
  return *(this->Load());
}

}  // namespace atomic
}  // namespace internal
}  // namespace base
}  // namespace embb

#endif  // EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_POINTER_H_
