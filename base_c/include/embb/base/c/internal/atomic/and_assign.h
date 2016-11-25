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

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_AND_ASSIGN_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_AND_ASSIGN_H_

#ifndef DOXYGEN

/*
 * The EMBB_DEFINE_[ATOMIC METHOD] macro defines, given the size in bytes,
 * the assembler prefix for that size (e.g., "b" for 1 byte), the assembler
 * implementation for [ATOMIC METHOD]. For GCC, inline assembler is used.
 * For MSVC, an external assembler implementation is called, as Microsoft
 * doesn't allow inline assembler with 64-bit code. In that case, the
 * assembly code is linked from an external object file.
 *
 * The generated function has the following signature (in the and_assign.h file,
 * analogous for other atomic methods):
 *
 *     static inline void embb_internal__atomic_and_assign_[BYTE_SIZE]
 *     (EMBB_BASE_BASIC_TYPE_ATOMIC_[BYTE_SIZE] volatile* pointer_to_value,
 *     EMBB_BASE_BASIC_TYPE_SIZE_[BYTE_SIZE] value)
 *
 * BYTE_SIZE is the number of bytes passed to the macro.
 *
 */
#ifdef EMBB_PLATFORM_ARCH_X86

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#define EMBB_DEFINE_AND_ASSIGN(EMBB_PARAMETER_SIZE_BYTE, EMBB_ATOMIC_X86_SIZE_SUFFIX) \
  extern void __fastcall EMBB_CAT2(embb_internal__atomic_and_assign_, EMBB_PARAMETER_SIZE_BYTE)_asm( \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value); \
  EMBB_PLATFORM_INLINE void __fastcall EMBB_CAT2(embb_internal__atomic_and_assign_, EMBB_PARAMETER_SIZE_BYTE)(EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value) { \
  _ReadWriteBarrier(); \
  EMBB_CAT2(embb_internal__atomic_and_assign_, EMBB_PARAMETER_SIZE_BYTE)_asm(pointer_to_value, value); \
  _ReadWriteBarrier(); \
  }
#elif defined(EMBB_PLATFORM_COMPILER_GNUC)
#define EMBB_DEFINE_AND_ASSIGN(EMBB_PARAMETER_SIZE_BYTE, EMBB_ATOMIC_X86_SIZE_SUFFIX)\
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_internal__atomic_and_assign_, \
  EMBB_PARAMETER_SIZE_BYTE)(EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) volatile* pointer_to_value, EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) value) { \
  __asm__ __volatile__("lock and" EMBB_ATOMIC_X86_SIZE_SUFFIX " %1, %0" \
  : "+m" (*pointer_to_value), "+q" (value) \
  : \
  : "memory"); \
  }
#else
#error "No atomic fetch and store implementation found"
#endif

/*
 * The three or four macro calls below generate the methods for 1, 2, 4, and
 * bytes, as stated in the macro definition.
 */
EMBB_DEFINE_AND_ASSIGN(1, "b")
EMBB_DEFINE_AND_ASSIGN(2, "w")
EMBB_DEFINE_AND_ASSIGN(4, "l")
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_DEFINE_AND_ASSIGN(8, "q")
#endif

#elif defined(EMBB_PLATFORM_ARCH_ARM)

#if defined(EMBB_PLATFORM_COMPILER_GNUC)
#define EMBB_DEFINE_AND_ASSIGN(EMBB_PARAMETER_SIZE_BYTE, \
  EMBB_ATOMIC_ARM_SIZE_SUFFIX) \
  EMBB_PLATFORM_INLINE \
  void EMBB_CAT2(embb_internal__atomic_and_assign_, \
  EMBB_PARAMETER_SIZE_BYTE)(\
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, EMBB_PARAMETER_SIZE_BYTE) \
  volatile* pointer_to_value, \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) \
  value) { \
  EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, EMBB_PARAMETER_SIZE_BYTE) \
  tmp, result; \
  __asm__ __volatile__ ( \
  "dmb\n\t" \
  "loop_%=:\n\t" \
  "ldrex" EMBB_ATOMIC_ARM_SIZE_SUFFIX " %0, [%2]\n\t" \
  "and %0, %0, %3\n\t" \
  "strex" EMBB_ATOMIC_ARM_SIZE_SUFFIX " %1, %0, [%2]\n\t" \
  "teq %1, #0\n\t" \
  "bne loop_%=\n\t" \
  "isb" \
  : "=&r" (result), "=&r" (tmp) \
  : "r" (pointer_to_value), "Ir" (value) \
  : "memory", "cc" ); \
  }
/*    __sync_fetch_and_and(pointer_to_value, value); \
  }*/
#else
#error "No atomic fetch and store implementation found"
#endif

EMBB_DEFINE_AND_ASSIGN(1, "b")
EMBB_DEFINE_AND_ASSIGN(2, "h")
EMBB_DEFINE_AND_ASSIGN(4, "")

#else
#error "Unknown architecture"
#endif

#endif //DOXYGEN

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_AND_ASSIGN_H_
