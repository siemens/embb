/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 * Copyright (c) 2016-2017, embedded brains GmbH. All rights reserved.
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

/*
 * This file was automatically generated.  Do not edit it.  Edit gen_atomic.py
 * instead and regenerate this file with it.
 */

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_ATOMIC_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_ATOMIC_H_

#include <embb/base/c/internal/config.h>
#include <embb/base/c/internal/atomic/atomic_sizes.h>

#include <stddef.h>
#include <stdint.h>

#if defined EMBB_PLATFORM_ARCH_CXX11
#include <atomic>
#elif defined EMBB_PLATFORM_ARCH_C11
#include <stdatomic.h>
#else
#include <string.h>
#include <embb/base/c/internal/macro_helper.h>
#if defined EMBB_PLATFORM_ARCH_X86
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#include <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void __fastcall embb_internal__atomic_memory_barrier_asm();

#ifdef __cplusplus
}
#endif

#endif
#elif !defined EMBB_PLATFORM_ARCH_ARM
#error "unsupported platform"
#endif
#endif

EMBB_PLATFORM_INLINE void embb_atomic_memory_barrier(void) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_thread_fence(std::memory_order_seq_cst);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_thread_fence(memory_order_seq_cst);
#elif defined EMBB_PLATFORM_ARCH_X86
#ifdef EMBB_PLATFORM_COMPILER_MSVC
  _ReadWriteBarrier();
  embb_internal__atomic_memory_barrier_asm();
  _ReadWriteBarrier();
#else
  __asm__ __volatile__ ("mfence" : : : "memory");
#endif
#elif defined EMBB_PLATFORM_ARCH_ARM
  __asm__ __volatile__ ("dmb" : : : "memory");
#endif
}

typedef uint_least8_t EMBB_BASE_BASIC_TYPE_SIZE_1;
typedef uint_least16_t EMBB_BASE_BASIC_TYPE_SIZE_2;
typedef uint_least32_t EMBB_BASE_BASIC_TYPE_SIZE_4;
typedef uint_least64_t EMBB_BASE_BASIC_TYPE_SIZE_8;

#if defined EMBB_PLATFORM_ARCH_CXX11

typedef std::atomic_uint_least8_t EMBB_BASE_BASIC_TYPE_ATOMIC_1;
typedef std::atomic_uint_least16_t EMBB_BASE_BASIC_TYPE_ATOMIC_2;
typedef std::atomic_uint_least32_t EMBB_BASE_BASIC_TYPE_ATOMIC_4;
typedef std::atomic_uint_least64_t EMBB_BASE_BASIC_TYPE_ATOMIC_8;

#elif defined EMBB_PLATFORM_ARCH_C11

typedef atomic_uint_least8_t EMBB_BASE_BASIC_TYPE_ATOMIC_1;
typedef atomic_uint_least16_t EMBB_BASE_BASIC_TYPE_ATOMIC_2;
typedef atomic_uint_least32_t EMBB_BASE_BASIC_TYPE_ATOMIC_4;
typedef atomic_uint_least64_t EMBB_BASE_BASIC_TYPE_ATOMIC_8;

#else

typedef EMBB_BASE_BASIC_TYPE_SIZE_1 EMBB_BASE_BASIC_TYPE_ATOMIC_1;
typedef EMBB_BASE_BASIC_TYPE_SIZE_2 EMBB_BASE_BASIC_TYPE_ATOMIC_2;
typedef EMBB_BASE_BASIC_TYPE_SIZE_4 EMBB_BASE_BASIC_TYPE_ATOMIC_4;
typedef EMBB_BASE_BASIC_TYPE_SIZE_8 EMBB_BASE_BASIC_TYPE_ATOMIC_8;

#endif

#if defined EMBB_PLATFORM_ARCH_CXX11 || defined EMBB_PLATFORM_ARCH_C11

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_1 desired
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_2 desired
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_4 desired
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_8 desired
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_1 embb_internal__atomic_fetch_and_add_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_2 embb_internal__atomic_fetch_and_add_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_4 embb_internal__atomic_fetch_and_add_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_8 embb_internal__atomic_fetch_and_add_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_1 embb_internal__atomic_load_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->load();
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_2 embb_internal__atomic_load_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->load();
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_4 embb_internal__atomic_load_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->load();
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_8 embb_internal__atomic_load_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->load();
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_1 embb_internal__atomic_swap_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_2 embb_internal__atomic_swap_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_4 embb_internal__atomic_swap_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_8 embb_internal__atomic_swap_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  return variable->exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_1(
  EMBB_BASE_BASIC_TYPE_ATOMIC_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_2(
  EMBB_BASE_BASIC_TYPE_ATOMIC_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_4(
  EMBB_BASE_BASIC_TYPE_ATOMIC_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_8(
  EMBB_BASE_BASIC_TYPE_ATOMIC_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  ) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

#else

#ifdef __cplusplus
extern "C" {
#endif

#include <embb/base/c/internal/atomic/load.h>
#include <embb/base/c/internal/atomic/and_assign.h>
#include <embb/base/c/internal/atomic/store.h>
#include <embb/base/c/internal/atomic/or_assign.h>
#include <embb/base/c/internal/atomic/xor_assign.h>
#include <embb/base/c/internal/atomic/swap.h>
#include <embb/base/c/internal/atomic/fetch_and_add.h>
#include <embb/base/c/internal/atomic/compare_and_swap.h>

#ifdef __cplusplus
}
#endif

#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_char internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_char internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_char;
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_short internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_short internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_short;
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_ushort internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_ushort internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_unsigned_short;
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_int internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_int internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_int;
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_uint internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_uint internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_unsigned_int;
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_long internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_long internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_long;
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_ulong internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_ulong internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_unsigned_long;
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_llong internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_llong internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_long_long;
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_ullong internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_ullong internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_unsigned_long_long;
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_intptr_t internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_intptr_t internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_intptr_t;
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_uintptr_t internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_uintptr_t internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_uintptr_t;
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_size_t internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_size_t internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_size_t;
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_ptrdiff_t internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_ptrdiff_t internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_ptrdiff_t;
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
typedef struct {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_uintmax_t internal_variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_uintmax_t internal_variable;
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) internal_variable;
#endif
#ifdef EMBB_THREADING_ANALYSIS_MODE
  embb_mutex_t internal_mutex;
#endif
#ifdef EMBB_DEBUG
  volatile uint32_t marker;
#endif
} embb_atomic_uintmax_t;
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_init_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);
  EMBB_ATOMIC_INIT_MARKER(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_and(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_and_assign_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_char(
  embb_atomic_char* variable,
  char* expected,
  char desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_short(
  embb_atomic_short* variable,
  short* expected,
  short desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short* expected,
  unsigned short desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_int(
  embb_atomic_int* variable,
  int* expected,
  int desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int* expected,
  unsigned int desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_long(
  embb_atomic_long* variable,
  long* expected,
  long desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long* expected,
  unsigned long desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_long_long(
  embb_atomic_long_long* variable,
  long long* expected,
  long long desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long* expected,
  unsigned long long desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t* expected,
  intptr_t desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t* expected,
  uintptr_t desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_size_t(
  embb_atomic_size_t* variable,
  size_t* expected,
  size_t desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t* expected,
  ptrdiff_t desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t* expected,
  uintmax_t desired
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.compare_exchange_strong(*expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_compare_exchange_strong(&variable->internal_variable, expected, desired);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) desired_basic;
  memcpy(&desired_basic, &desired, sizeof(desired_basic));
  int return_val = EMBB_XCAT2(embb_internal__atomic_compare_and_swap_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, (EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) *)expected, desired_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE char embb_atomic_fetch_and_add_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  char return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  char return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
  char return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE short embb_atomic_fetch_and_add_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  short return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  short return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  short return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned short embb_atomic_fetch_and_add_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned short return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned short return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned short return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_fetch_and_add_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  int return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned int embb_atomic_fetch_and_add_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned int return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned int return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned int return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE long embb_atomic_fetch_and_add_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  long return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  long return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned long embb_atomic_fetch_and_add_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned long return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned long return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE long long embb_atomic_fetch_and_add_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  long long return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  long long return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  long long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned long long embb_atomic_fetch_and_add_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned long long return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned long long return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned long long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE intptr_t embb_atomic_fetch_and_add_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  intptr_t return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  intptr_t return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  intptr_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE uintptr_t embb_atomic_fetch_and_add_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  uintptr_t return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  uintptr_t return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  uintptr_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE size_t embb_atomic_fetch_and_add_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  size_t return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  size_t return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  size_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE ptrdiff_t embb_atomic_fetch_and_add_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  ptrdiff_t return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  ptrdiff_t return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  ptrdiff_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE uintmax_t embb_atomic_fetch_and_add_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  uintmax_t return_val = variable->internal_variable.fetch_add(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  uintmax_t return_val = atomic_fetch_add(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_fetch_and_add_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  uintmax_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE char embb_atomic_load_char(
  embb_atomic_char* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  char return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  char return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable);
  char return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE short embb_atomic_load_short(
  embb_atomic_short* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  short return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  short return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable);
  short return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned short embb_atomic_load_unsigned_short(
  embb_atomic_unsigned_short* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned short return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned short return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable);
  unsigned short return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_load_int(
  embb_atomic_int* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable);
  int return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned int embb_atomic_load_unsigned_int(
  embb_atomic_unsigned_int* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned int return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned int return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable);
  unsigned int return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE long embb_atomic_load_long(
  embb_atomic_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  long return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  long return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable);
  long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned long embb_atomic_load_unsigned_long(
  embb_atomic_unsigned_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned long return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned long return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable);
  unsigned long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE long long embb_atomic_load_long_long(
  embb_atomic_long_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  long long return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  long long return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable);
  long long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned long long embb_atomic_load_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned long long return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned long long return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable);
  unsigned long long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE intptr_t embb_atomic_load_intptr_t(
  embb_atomic_intptr_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  intptr_t return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  intptr_t return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable);
  intptr_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE uintptr_t embb_atomic_load_uintptr_t(
  embb_atomic_uintptr_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  uintptr_t return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  uintptr_t return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable);
  uintptr_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE size_t embb_atomic_load_size_t(
  embb_atomic_size_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  size_t return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  size_t return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable);
  size_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE ptrdiff_t embb_atomic_load_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  ptrdiff_t return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  ptrdiff_t return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable);
  ptrdiff_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE uintmax_t embb_atomic_load_uintmax_t(
  embb_atomic_uintmax_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  uintmax_t return_val = variable->internal_variable.load();
#elif defined EMBB_PLATFORM_ARCH_C11
  uintmax_t return_val = atomic_load(&variable->internal_variable);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_load_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable);
  uintmax_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_or(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_or_assign_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_store_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  variable->internal_variable.store(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_store_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE char embb_atomic_swap_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  char return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  char return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
  char return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE short embb_atomic_swap_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  short return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  short return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  short return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned short embb_atomic_swap_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned short return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned short return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned short return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE int embb_atomic_swap_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  int return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  int return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  int return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned int embb_atomic_swap_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned int return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned int return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned int return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE long embb_atomic_swap_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  long return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  long return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned long embb_atomic_swap_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned long return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned long return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE long long embb_atomic_swap_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  long long return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  long long return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  long long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE unsigned long long embb_atomic_swap_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  unsigned long long return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  unsigned long long return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
  unsigned long long return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE intptr_t embb_atomic_swap_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  intptr_t return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  intptr_t return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  intptr_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE uintptr_t embb_atomic_swap_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  uintptr_t return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  uintptr_t return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  uintptr_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE size_t embb_atomic_swap_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  size_t return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  size_t return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  size_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE ptrdiff_t embb_atomic_swap_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  ptrdiff_t return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  ptrdiff_t return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  ptrdiff_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE uintmax_t embb_atomic_swap_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  uintmax_t return_val = variable->internal_variable.exchange(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  uintmax_t return_val = atomic_exchange(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) return_val_basic = EMBB_XCAT2(embb_internal__atomic_swap_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
  uintmax_t return_val;
  memcpy(&return_val, &return_val_basic, sizeof(return_val));
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
  return return_val;
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_char(
  embb_atomic_char* variable,
  char value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_CHAR_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_CHAR_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_short(
  embb_atomic_short* variable,
  short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_SHORT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_UNSIGNED_SHORT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_int(
  embb_atomic_int* variable,
  int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_INT_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_UNSIGNED_INT_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_long(
  embb_atomic_long* variable,
  long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_UNSIGNED_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_long_long(
  embb_atomic_long_long* variable,
  long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_INTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_INTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTPTR_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_UINTPTR_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_size_t(
  embb_atomic_size_t* variable,
  size_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_SIZE_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_SIZE_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PTRDIFF_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_PTRDIFF_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable->internal_variable.fetch_xor(value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(&variable->internal_variable, value);
#else
  EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_UINTMAX_T_TYPE_SIZE) value_basic;
  memcpy(&value_basic, &value, sizeof(value_basic));
  EMBB_XCAT2(embb_internal__atomic_xor_assign_, EMBB_UINTMAX_T_TYPE_SIZE)(&variable->internal_variable, value_basic);
#endif
  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);
}
#endif

#ifdef EMBB_CHAR_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_char(
  embb_atomic_char* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_short(
  embb_atomic_short* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_UNSIGNED_SHORT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_short(
  embb_atomic_unsigned_short* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_int(
  embb_atomic_int* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_UNSIGNED_INT_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_int(
  embb_atomic_unsigned_int* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_long(
  embb_atomic_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_long(
  embb_atomic_unsigned_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_long_long(
  embb_atomic_long_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_UNSIGNED_LONG_LONG_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_INTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_intptr_t(
  embb_atomic_intptr_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_UINTPTR_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_uintptr_t(
  embb_atomic_uintptr_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_SIZE_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_size_t(
  embb_atomic_size_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_PTRDIFF_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#ifdef EMBB_UINTMAX_T_TYPE_IS_ATOMIC
EMBB_PLATFORM_INLINE void embb_atomic_destroy_uintmax_t(
  embb_atomic_uintmax_t* variable
  ) {
  EMBB_ATOMIC_INIT_CHECK(variable);
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#else
  (void)variable;
#endif
  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);
  EMBB_ATOMIC_DESTROY_MARKER(variable);
}
#endif

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_ATOMIC_H_
