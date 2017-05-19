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

#ifndef EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_UTILITY_H_
#define EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_UTILITY_H_

#include <stdint.h>
#include <cstddef>
#include <cassert>
#include <utility>

#include <embb/base/c/atomic.h>

namespace embb {
namespace base {
namespace internal {
namespace atomic {

template<size_t S> struct AtomicTraits;

/**
 * The templates below are use to map the size in bytes to the respective
 * native type and to define this type. The basic types are defined in the
 * base_c component.
 */

// Specialization for bytes
template<> struct AtomicTraits<1> {
  typedef EMBB_BASE_BASIC_TYPE_SIZE_1 NativeType;
  typedef EMBB_BASE_BASIC_TYPE_ATOMIC_1 AtomicType;
};

// Specialization for half-words
template<> struct AtomicTraits<2> {
  typedef EMBB_BASE_BASIC_TYPE_SIZE_2 NativeType;
  typedef EMBB_BASE_BASIC_TYPE_ATOMIC_2 AtomicType;
};

// Specialization for words
template<> struct AtomicTraits<4> {
  typedef EMBB_BASE_BASIC_TYPE_SIZE_4 NativeType;
  typedef EMBB_BASE_BASIC_TYPE_ATOMIC_4 AtomicType;
};

// Specialization for double-words
template<> struct AtomicTraits<8> {
  typedef EMBB_BASE_BASIC_TYPE_SIZE_8 NativeType;
  typedef EMBB_BASE_BASIC_TYPE_ATOMIC_8 AtomicType;
};

}  // namespace atomic
}  // namespace internal
}  // namespace base
}  // namespace embb

#endif  // EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_UTILITY_H_
