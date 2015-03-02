/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_MEMORY_BARRIER_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_MEMORY_BARRIER_H_

#include <embb/base/c/internal/config.h>

#ifndef DOXYGEN

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#include <intrin.h>
#endif

#ifdef EMBB_PLATFORM_ARCH_X86

#ifdef EMBB_PLATFORM_COMPILER_MSVC
extern void __fastcall embb_internal__atomic_memory_barrier_asm();
// Read/write barrier
EMBB_PLATFORM_INLINE void __fastcall embb_atomic_memory_barrier() {
  _ReadWriteBarrier();
  embb_internal__atomic_memory_barrier_asm();
  _ReadWriteBarrier();
}
#elif defined(EMBB_PLATFORM_COMPILER_GNUC)
// Read/write barrier
EMBB_PLATFORM_INLINE void embb_atomic_memory_barrier() {
  __asm__ __volatile__ ("mfence" : : : "memory");
}
#else
#error "No atomic fetch and store implementation found"
#endif

#elif defined(EMBB_PLATFORM_ARCH_ARM)

EMBB_PLATFORM_INLINE void embb_atomic_memory_barrier() {
  __asm__ __volatile__ ("dmb" : : : "memory");
}

#endif

#endif //DOXYGEN

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_MEMORY_BARRIER_H_
