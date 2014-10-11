/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_COMPARE_AND_SWAP_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_COMPARE_AND_SWAP_H_

#ifndef DOXYGEN

#include <embb/base/c/internal/config.h>
#include <embb/base/c/internal/atomic/atomic_sizes.h>
#include <embb/base/c/internal/macro_helper.h>
#include <embb/base/c/internal/atomic/atomic_variables.h>
#include <string.h>

/*
* See file and_assign.h for a detailed (and operation independent) description
* of the following macro.
*/
#ifdef EMBB_ARCH_X86

#ifdef EMBB_COMPILER_MSVC
#define EMBB_DEFINE_COMPARE_AND_SWAP(EMBB_PARAMETER_SIZE_BYTE, EMBB_ATOMIC_X86_SIZE_SUFFIX) \
  extern int __fastcall EMBB_CAT2(embb_internal__atomic_compare_and_swap_, EMBB_PARAMETER_SIZE_BYTE)_asm( \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* expected, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) desired); \
  EMBB_INLINE int __fastcall EMBB_CAT2(embb_internal__atomic_compare_and_swap_, EMBB_PARAMETER_SIZE_BYTE)( \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* expected, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) desired) { \
  int result; \
  _ReadWriteBarrier(); \
  result = EMBB_CAT2(embb_internal__atomic_compare_and_swap_, EMBB_PARAMETER_SIZE_BYTE)_asm(pointer_to_value, expected, desired); \
  _ReadWriteBarrier(); \
  return result; \
  }
#elif defined(EMBB_COMPILER_GNUC)
#define EMBB_DEFINE_COMPARE_AND_SWAP(EMBB_PARAMETER_SIZE_BYTE, EMBB_ATOMIC_X86_SIZE_SUFFIX) \
  EMBB_INLINE int EMBB_CAT2(embb_internal__atomic_compare_and_swap_, \
  EMBB_PARAMETER_SIZE_BYTE)(EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* expected, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) desired) { \
  char result; \
  __asm__ __volatile__ ("lock cmpxchg" EMBB_ATOMIC_X86_SIZE_SUFFIX\
  " %3, %0 \n\t" \
  "setz %2 \n\t" \
  : "+m" (*pointer_to_value), "+a" (*expected), "=q" (result) \
  : "q" (desired) \
  : "memory", "cc" ); \
  return result; \
  }
#else
#error "No atomic fetch and store implementation found"
#endif

/*
* The three or four macro calls below generate the methods for 1, 2, 4, and
* bytes, as stated in the macro definition.
*/
EMBB_DEFINE_COMPARE_AND_SWAP(1, "b")
EMBB_DEFINE_COMPARE_AND_SWAP(2, "w")
EMBB_DEFINE_COMPARE_AND_SWAP(4, "l")
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_DEFINE_COMPARE_AND_SWAP(8, "q")
#endif

#elif defined(EMBB_ARCH_ARM)

#if defined(EMBB_COMPILER_GNUC)
#define EMBB_DEFINE_COMPARE_AND_SWAP(EMBB_PARAMETER_SIZE_BYTE, \
  EMBB_ATOMIC_ARM_SIZE_SUFFIX) \
  EMBB_INLINE \
  int EMBB_CAT2(embb_internal__atomic_compare_and_swap_, \
  EMBB_PARAMETER_SIZE_BYTE)(\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) \
  volatile* pointer_to_value, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) \
  volatile* expected, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) desired) { \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) \
  oldval, res; \
  __asm__ __volatile__ ("dmb" : : : "memory"); \
  do { \
  __asm__ __volatile__ ( \
  "ldrex" EMBB_ATOMIC_ARM_SIZE_SUFFIX " %1, [%2]\n\t" \
  "mov %0, #0\n\t" \
  "teq %1, %3\n\t" \
  "it eq\n\t" \
  "strex" EMBB_ATOMIC_ARM_SIZE_SUFFIX "eq %0, %4, [%2]\n\t" \
  : "=&r" (res), "=&r" (oldval) \
  : "r" (pointer_to_value), "Ir" (*expected), "r" (desired) \
  : "cc" ); \
  } while (res); \
  __asm__ __volatile__ ("isb" : : : "memory"); \
  if (oldval == *expected) { \
  return 1; \
  } else { \
  *expected = oldval; \
  return 0; \
  } \
  }
#else
#error "No atomic fetch and store implementation found"
#endif

EMBB_DEFINE_COMPARE_AND_SWAP(1, "b")
EMBB_DEFINE_COMPARE_AND_SWAP(2, "h")
EMBB_DEFINE_COMPARE_AND_SWAP(4, "")

#else
#error "Unknown architecture"
#endif

/*
* See file and_assign.h for a detailed (and operation independent) description
* of the following macro.
*/
#define EMBB_ATOMIC_INTERNAL_DEFINE_COMPARE_AND_SWAP_METHOD(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) \
  EMBB_INLINE int EMBB_CAT2(embb_atomic_compare_and_swap_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)(\
  EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)* variable, EMBB_ATOMIC_PARAMETER_TYPE_NATIVE* expected, EMBB_ATOMIC_PARAMETER_TYPE_NATIVE desired) {\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) desired_pun;\
  memcpy(&desired_pun, &desired, sizeof(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE));\
  return EMBB_CAT2(embb_internal__atomic_compare_and_swap_, EMBB_ATOMIC_PARAMETER_TYPE_SIZE)((EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) volatile *) \
  (&(variable->internal_variable)), (EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) volatile *) expected, desired_pun); \
  }

#undef EMBB_ATOMIC_METHOD_TO_GENERATE
#define EMBB_ATOMIC_METHOD_TO_GENERATE COMPARE_AND_SWAP_METHOD
#include <embb/base/c/internal/atomic/generate_atomic_implementation_template.h>
#undef EMBB_ATOMIC_METHOD_TO_GENERATE

#endif //DOXYGEN

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_COMPARE_AND_SWAP_H_

