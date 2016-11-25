/*
 * Copyright (c) 2016, embedded brains GmbH. All rights reserved.
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

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_C11_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_C11_H_

#include <embb/base/c/internal/config.h>

#if defined EMBB_PLATFORM_ARCH_CXX11
#include <atomic>
#include <cstdint>
#elif defined EMBB_PLATFORM_ARCH_C11
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#else
#error "Not available for selected C or C++ standard"
#endif

EMBB_PLATFORM_INLINE void embb_atomic_memory_barrier(void)
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_thread_fence(std::memory_order_seq_cst);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_thread_fence(memory_order_seq_cst);
#endif
}

typedef uint_least8_t EMBB_BASE_BASIC_TYPE_SIZE_1;
typedef uint_least16_t EMBB_BASE_BASIC_TYPE_SIZE_2;
typedef uint_least32_t EMBB_BASE_BASIC_TYPE_SIZE_4;
typedef uint_least64_t EMBB_BASE_BASIC_TYPE_SIZE_8;

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and((std::atomic_uint_least8_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and((atomic_uint_least8_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and((std::atomic_uint_least16_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and((atomic_uint_least16_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and((std::atomic_uint_least32_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and((atomic_uint_least32_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_and_assign_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and((std::atomic_uint_least64_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and((atomic_uint_least64_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_1 desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong((std::atomic_uint_least8_t*)variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong((atomic_uint_least8_t*)variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_2 desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong((std::atomic_uint_least16_t*)variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong((atomic_uint_least16_t*)variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_4 desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong((std::atomic_uint_least32_t*)variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong((atomic_uint_least32_t*)variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_internal__atomic_compare_and_swap_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8* expected,
  EMBB_BASE_BASIC_TYPE_SIZE_8 desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong((std::atomic_uint_least64_t*)variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong((atomic_uint_least64_t*)variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_1 embb_internal__atomic_fetch_and_add_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add((std::atomic_uint_least8_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add((atomic_uint_least8_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_2 embb_internal__atomic_fetch_and_add_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add((std::atomic_uint_least16_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add((atomic_uint_least16_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_4 embb_internal__atomic_fetch_and_add_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add((std::atomic_uint_least32_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add((atomic_uint_least32_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_8 embb_internal__atomic_fetch_and_add_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add((std::atomic_uint_least64_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add((atomic_uint_least64_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_1 embb_internal__atomic_load_1(
  const EMBB_BASE_BASIC_TYPE_SIZE_1* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load((std::atomic_uint_least8_t*)variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load((atomic_uint_least8_t*)variable);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_2 embb_internal__atomic_load_2(
  const EMBB_BASE_BASIC_TYPE_SIZE_2* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load((std::atomic_uint_least16_t*)variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load((atomic_uint_least16_t*)variable);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_4 embb_internal__atomic_load_4(
  const EMBB_BASE_BASIC_TYPE_SIZE_4* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load((std::atomic_uint_least32_t*)variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load((atomic_uint_least32_t*)variable);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_8 embb_internal__atomic_load_8(
  const EMBB_BASE_BASIC_TYPE_SIZE_8* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load((std::atomic_uint_least64_t*)variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load((atomic_uint_least64_t*)variable);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or((std::atomic_uint_least8_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or((atomic_uint_least8_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or((std::atomic_uint_least16_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or((atomic_uint_least16_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or((std::atomic_uint_least32_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or((atomic_uint_least32_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_or_assign_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or((std::atomic_uint_least64_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or((atomic_uint_least64_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store((std::atomic_uint_least8_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store((atomic_uint_least8_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store((std::atomic_uint_least16_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store((atomic_uint_least16_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store((std::atomic_uint_least32_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store((atomic_uint_least32_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_store_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store((std::atomic_uint_least64_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store((atomic_uint_least64_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_1 embb_internal__atomic_swap_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange((std::atomic_uint_least8_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange((atomic_uint_least8_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_2 embb_internal__atomic_swap_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange((std::atomic_uint_least16_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange((atomic_uint_least16_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_4 embb_internal__atomic_swap_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange((std::atomic_uint_least32_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange((atomic_uint_least32_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE EMBB_BASE_BASIC_TYPE_SIZE_8 embb_internal__atomic_swap_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange((std::atomic_uint_least64_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange((atomic_uint_least64_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_1(
  EMBB_BASE_BASIC_TYPE_SIZE_1* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_1 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor((std::atomic_uint_least8_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor((atomic_uint_least8_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_2(
  EMBB_BASE_BASIC_TYPE_SIZE_2* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_2 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor((std::atomic_uint_least16_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor((atomic_uint_least16_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_4(
  EMBB_BASE_BASIC_TYPE_SIZE_4* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_4 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor((std::atomic_uint_least32_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor((atomic_uint_least32_t*)variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_internal__atomic_xor_assign_8(
  EMBB_BASE_BASIC_TYPE_SIZE_8* variable,
  EMBB_BASE_BASIC_TYPE_SIZE_8 value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor((std::atomic_uint_least64_t*)variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor((atomic_uint_least64_t*)variable, value);
#endif
}

#if defined EMBB_PLATFORM_ARCH_CXX11

typedef std::atomic_char embb_atomic_char;
typedef std::atomic_short embb_atomic_short;
typedef std::atomic_ushort embb_atomic_unsigned_short;
typedef std::atomic_int embb_atomic_int;
typedef std::atomic_uint embb_atomic_unsigned_int;
typedef std::atomic_long embb_atomic_long;
typedef std::atomic_ulong embb_atomic_unsigned_long;
typedef std::atomic_llong embb_atomic_long_long;
typedef std::atomic_ullong embb_atomic_unsigned_long_long;
typedef std::atomic_intptr_t embb_atomic_intptr_t;
typedef std::atomic_uintptr_t embb_atomic_uintptr_t;
typedef std::atomic_size_t embb_atomic_size_t;
typedef std::atomic_ptrdiff_t embb_atomic_ptrdiff_t;
typedef std::atomic_uintmax_t embb_atomic_uintmax_t;

#elif defined EMBB_PLATFORM_ARCH_C11

typedef atomic_char embb_atomic_char;
typedef atomic_short embb_atomic_short;
typedef atomic_ushort embb_atomic_unsigned_short;
typedef atomic_int embb_atomic_int;
typedef atomic_uint embb_atomic_unsigned_int;
typedef atomic_long embb_atomic_long;
typedef atomic_ulong embb_atomic_unsigned_long;
typedef atomic_llong embb_atomic_long_long;
typedef atomic_ullong embb_atomic_unsigned_long_long;
typedef atomic_intptr_t embb_atomic_intptr_t;
typedef atomic_uintptr_t embb_atomic_uintptr_t;
typedef atomic_size_t embb_atomic_size_t;
typedef atomic_ptrdiff_t embb_atomic_ptrdiff_t;
typedef atomic_uintmax_t embb_atomic_uintmax_t;

#endif

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_and_assign_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_and(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_and(variable, value);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_char(
  embb_atomic_char* variable,
  char* expected,
  char desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_short(
  embb_atomic_short* variable,
  short* expected,
  short desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short* expected,
  unsigned short desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_int(
  embb_atomic_int* variable,
  int* expected,
  int desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int* expected,
  unsigned int desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_long(
  embb_atomic_long* variable,
  long* expected,
  long desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long* expected,
  unsigned long desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_long_long(
  embb_atomic_long_long* variable,
  long long* expected,
  long long desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long* expected,
  unsigned long long desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t* expected,
  intptr_t desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t* expected,
  uintptr_t desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_size_t(
  embb_atomic_size_t* variable,
  size_t* expected,
  size_t desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t* expected,
  ptrdiff_t desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t* expected,
  uintmax_t desired
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_compare_exchange_strong(variable, expected, desired);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_compare_exchange_strong(variable, expected, desired);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_char(
  embb_atomic_char* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_short(
  embb_atomic_short* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_short(
  embb_atomic_unsigned_short* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_int(
  embb_atomic_int* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_int(
  embb_atomic_unsigned_int* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_long(
  embb_atomic_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_long(
  embb_atomic_unsigned_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_long_long(
  embb_atomic_long_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_intptr_t(
  embb_atomic_intptr_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_uintptr_t(
  embb_atomic_uintptr_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_size_t(
  embb_atomic_size_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_destroy_uintmax_t(
  embb_atomic_uintmax_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)variable;
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)variable;
#endif
}

EMBB_PLATFORM_INLINE char embb_atomic_fetch_and_add_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE short embb_atomic_fetch_and_add_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned short embb_atomic_fetch_and_add_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_fetch_and_add_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned int embb_atomic_fetch_and_add_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE long embb_atomic_fetch_and_add_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned long embb_atomic_fetch_and_add_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE long long embb_atomic_fetch_and_add_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned long long embb_atomic_fetch_and_add_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE intptr_t embb_atomic_fetch_and_add_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE uintptr_t embb_atomic_fetch_and_add_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE size_t embb_atomic_fetch_and_add_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE ptrdiff_t embb_atomic_fetch_and_add_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE uintmax_t embb_atomic_fetch_and_add_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_fetch_add(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_fetch_add(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_init_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_init(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_init(variable, value);
#endif
}

EMBB_PLATFORM_INLINE char embb_atomic_load_char(
  const embb_atomic_char* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE short embb_atomic_load_short(
  const embb_atomic_short* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE unsigned short embb_atomic_load_unsigned_short(
  const embb_atomic_unsigned_short* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_load_int(
  const embb_atomic_int* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE unsigned int embb_atomic_load_unsigned_int(
  const embb_atomic_unsigned_int* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE long embb_atomic_load_long(
  const embb_atomic_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE unsigned long embb_atomic_load_unsigned_long(
  const embb_atomic_unsigned_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE long long embb_atomic_load_long_long(
  const embb_atomic_long_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE unsigned long long embb_atomic_load_unsigned_long_long(
  const embb_atomic_unsigned_long_long* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE intptr_t embb_atomic_load_intptr_t(
  const embb_atomic_intptr_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE uintptr_t embb_atomic_load_uintptr_t(
  const embb_atomic_uintptr_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE size_t embb_atomic_load_size_t(
  const embb_atomic_size_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE ptrdiff_t embb_atomic_load_ptrdiff_t(
  const embb_atomic_ptrdiff_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE uintmax_t embb_atomic_load_uintmax_t(
  const embb_atomic_uintmax_t* variable
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_load(variable);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_load(variable);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_or_assign_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_or(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_or(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_store_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_store(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_store(variable, value);
#endif
}

EMBB_PLATFORM_INLINE char embb_atomic_swap_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE short embb_atomic_swap_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned short embb_atomic_swap_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE int embb_atomic_swap_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned int embb_atomic_swap_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE long embb_atomic_swap_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned long embb_atomic_swap_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE long long embb_atomic_swap_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE unsigned long long embb_atomic_swap_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE intptr_t embb_atomic_swap_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE uintptr_t embb_atomic_swap_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE size_t embb_atomic_swap_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE ptrdiff_t embb_atomic_swap_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE uintmax_t embb_atomic_swap_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  return std::atomic_exchange(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  return atomic_exchange(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_char(
  embb_atomic_char* variable,
  char value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_short(
  embb_atomic_short* variable,
  short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_short(
  embb_atomic_unsigned_short* variable,
  unsigned short value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_int(
  embb_atomic_int* variable,
  int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_int(
  embb_atomic_unsigned_int* variable,
  unsigned int value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_long(
  embb_atomic_long* variable,
  long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_long(
  embb_atomic_unsigned_long* variable,
  unsigned long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_long_long(
  embb_atomic_long_long* variable,
  long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_unsigned_long_long(
  embb_atomic_unsigned_long_long* variable,
  unsigned long long value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_intptr_t(
  embb_atomic_intptr_t* variable,
  intptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_uintptr_t(
  embb_atomic_uintptr_t* variable,
  uintptr_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_size_t(
  embb_atomic_size_t* variable,
  size_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_ptrdiff_t(
  embb_atomic_ptrdiff_t* variable,
  ptrdiff_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_uintmax_t(
  embb_atomic_uintmax_t* variable,
  uintmax_t value
  )
{
#if defined EMBB_PLATFORM_ARCH_CXX11
  (void)std::atomic_fetch_xor(variable, value);
#elif defined EMBB_PLATFORM_ARCH_C11
  (void)atomic_fetch_xor(variable, value);
#endif
}

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_C11_H_
