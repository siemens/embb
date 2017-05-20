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

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_XOR_ASSIGN_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_XOR_ASSIGN_H_

#ifndef DOXYGEN

/*
* See file and_assign.h for a detailed (and operation independent) description
* of the following macro.
*/
#ifdef EMBB_PLATFORM_ARCH_X86

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#define EMBB_DEFINE_XOR_ASSIGN(EMBB_PARAMETER_SIZE_BYTE, EMBB_ATOMIC_X86_SIZE_SUFFIX) \
  extern void __fastcall EMBB_CAT2(embb_internal__atomic_xor_assign_, EMBB_PARAMETER_SIZE_BYTE)_asm(\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value); \
  EMBB_PLATFORM_INLINE void __fastcall EMBB_CAT2(embb_internal__atomic_xor_assign_, EMBB_PARAMETER_SIZE_BYTE)(\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value) { \
  _ReadWriteBarrier(); \
  EMBB_CAT2(embb_internal__atomic_xor_assign_, \
  EMBB_PARAMETER_SIZE_BYTE)_asm(pointer_to_value, value); \
  _ReadWriteBarrier(); \
  }
#elif defined(EMBB_PLATFORM_COMPILER_GNUC)
#define EMBB_DEFINE_XOR_ASSIGN(EMBB_PARAMETER_SIZE_BYTE, EMBB_ATOMIC_X86_SIZE_SUFFIX) \
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_internal__atomic_xor_assign_, EMBB_PARAMETER_SIZE_BYTE)(\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value) { \
  __asm__ __volatile__("lock xor" EMBB_ATOMIC_X86_SIZE_SUFFIX " %1, %0" \
  : "+m" (*pointer_to_value), "+q" (value) \
  : \
  : "memory"); \
  }

#else
#error "No atomic fetch xor store implementation found"
#endif

/*
* The three or four macro calls below generate the methods for 1, 2, 4, and
* bytes, as stated in the macro definition.
*/
EMBB_DEFINE_XOR_ASSIGN(1, "b")
EMBB_DEFINE_XOR_ASSIGN(2, "w")
EMBB_DEFINE_XOR_ASSIGN(4, "l")
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_DEFINE_XOR_ASSIGN(8, "q")
#endif

#elif defined(EMBB_PLATFORM_ARCH_ARM)

#if defined(EMBB_PLATFORM_COMPILER_GNUC)
#define EMBB_DEFINE_XOR_ASSIGN(EMBB_PARAMETER_SIZE_BYTE, \
  EMBB_ATOMIC_ARM_SIZE_SUFFIX) \
  EMBB_PLATFORM_INLINE \
  void EMBB_CAT2(embb_internal__atomic_xor_assign_, \
  EMBB_PARAMETER_SIZE_BYTE)(\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) \
  volatile* pointer_to_value, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value) { \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) \
  result, tmp; \
  __asm__ __volatile__ ( \
  "dmb\n\t" \
  "loop_%=:\n\t" \
  "ldrex" EMBB_ATOMIC_ARM_SIZE_SUFFIX " %0, [%2]\n\t" \
  "eor %0, %0, %3\n\t" \
  "strex" EMBB_ATOMIC_ARM_SIZE_SUFFIX " %1, %0, [%2]\n\t" \
  "teq %1, #0\n\t" \
  "bne loop_%=\n\t" \
  "isb" \
  : "=&r" (result), "=&r" (tmp) \
  : "r" (pointer_to_value), "Ir" (value) \
  : "memory", "cc" ); \
  }
#else
#error "No atomic or assign implementation found"
#endif

EMBB_DEFINE_XOR_ASSIGN(1, "b")
EMBB_DEFINE_XOR_ASSIGN(2, "h")
EMBB_DEFINE_XOR_ASSIGN(4, "")

#else
#error "Unknown architecture"
#endif

#endif //DOXYGEN

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_XOR_ASSIGN_H_
