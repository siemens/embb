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

#ifndef EMBB_BASE_C_INTERNAL_CONFIG_H_
#define EMBB_BASE_C_INTERNAL_CONFIG_H_

#include <embb/base/c/internal/cmake_config.h>

/* Define names:
 * - threading platforms: EMBB_THREADING_
 * - compilers: EMBB_COMPILER_
 * - operating systems: EMBB_OS_
 */

#ifdef DOXYGEN
/* For Doxygen, simulate GNU compiler on 64 bit */
#define __GNUC__
#define __x86_64__
#endif

/* Most processors have cache lines with up to 64 Bytes, except for Itanium
 * which has 128 bytes.
 */
#if defined(_M_IA64) || defined(_IA64)
#define EMBB_PLATFORM_CACHE_LINE_SIZE 128
#else
#define EMBB_PLATFORM_CACHE_LINE_SIZE 64
#endif

/* For MSVC, if _DEBUG is set, set also EMBB_DEBUG.
 * There is no such flag for GCC. Instead, cmake sets
 * the EMBB_DEBUG flag...
 */
#ifdef _DEBUG
#define EMBB_DEBUG
#endif

#ifdef __GNUC__
#define EMBB_PLATFORM_ALIGN(size) __attribute__ ((aligned(size)))
#elif defined _MSC_VER || defined __INTEL_COMPILER
#define EMBB_PLATFORM_ALIGN(size) __declspec(align(size))
#else
#error "Unsupported compiler"
#endif

#if __GNUC__
#define EMBB_PLATFORM_INLINE static inline
#define EMBB_PLATFORM_COMPILER_GNUC
#elif _MSC_VER
#define EMBB_PLATFORM_INLINE __inline
#define EMBB_PLATFORM_COMPILER_MSVC
#else
#define EMBB_PLATFORM_INLINE inline
#define EMBB_PLATFORM_COMPILER_UNKNOWN
#endif

#if (__cplusplus >= 201103) && defined(EMBB_PLATFORM_USE_C11_CXX11)
#define EMBB_PLATFORM_ARCH_CXX11
#elif (__STDC_VERSION__ >= 201112) && defined(EMBB_PLATFORM_USE_C11_CXX11)
#define EMBB_PLATFORM_ARCH_C11
#elif defined(__x86_64__) || defined(_M_X64)
#define EMBB_PLATFORM_ARCH_X86_64
#define EMBB_PLATFORM_ARCH_X86
#define EMBB_PLATFORM_HAS_CAS_64
#elif defined(__i386) || defined(_M_IX86)
#define EMBB_PLATFORM_ARCH_X86_32
#define EMBB_PLATFORM_ARCH_X86
#elif defined(__arm__)
#define EMBB_PLATFORM_ARCH_ARM
#else
#define EMBB_PLATFORM_ARCH_UNKNOWN
#endif

#if defined(EMBB_PLATFORM_COMPILER_MSVC)
#define EMBB_PLATFORM_THREADING_WINTHREADS
#elif defined(EMBB_PLATFORM_COMPILER_GNUC)
#define EMBB_PLATFORM_THREADING_POSIXTHREADS
#else
#error "No thread implementation could be determined"
#endif

#endif /* EMBB_BASE_C_INTERNAL_CONFIG_H_ */
