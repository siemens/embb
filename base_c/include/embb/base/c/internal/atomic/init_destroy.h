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

#include <embb/base/c/internal/config.h>
#include <embb/base/c/internal/atomic/store.h>
#include <embb/base/c/internal/atomic/atomic_sizes.h>
#include <embb/base/c/internal/macro_helper.h>
#include <embb/base/c/internal/atomic/atomic_variables.h>
#include <string.h>

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_INIT_DESTROY_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_INIT_DESTROY_H_

#ifdef EMBB_THREADING_ANALYSIS_MODE

#include <stdio.h>
#include <stdlib.h>
#include <embb/base/c/errors.h>

EMBB_PLATFORM_INLINE void embb_atomic_internal_check_init_flag(int flag) {
  if (flag != EMBB_ATOMIC_INTERNAL_INITIALIZED_VALUE) {
    fprintf(stderr, "Exit program due to not initialized or not properly "
                    "synchronized initialization of atomic variable");
    exit(EMBB_ERROR);
  }
}
#endif


// embb_mutex_init():

#ifdef EMBB_THREADING_ANALYSIS_MODE

#define EMBB_ATOMIC_INTERNAL_DEFINE_INIT_METHOD(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) \
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_atomic_init_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)( \
      volatile EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)* variable, \
      EMBB_ATOMIC_PARAMETER_TYPE_NATIVE initial_value) { \
	  embb_mutex_init(&(((EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)*)variable)->mutex), EMBB_MUTEX_PLAIN); \
    variable->init_flag = EMBB_ATOMIC_INTERNAL_INITIALIZED_VALUE; \
    variable->internal_variable = initial_value; \
  }

#else // EMBB_THREADING_ANALYSIS_MODE

#define EMBB_ATOMIC_INTERNAL_DEFINE_INIT_METHOD(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) \
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_atomic_init_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)( \
      EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX) volatile* variable, \
      EMBB_ATOMIC_PARAMETER_TYPE_NATIVE initial_value) { \
    memcpy(&(variable->internal_variable), &initial_value, sizeof(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE)); \
  }

#endif // else EMBB_THREADING_ANALYSIS_MODE

#undef EMBB_ATOMIC_METHOD_TO_GENERATE
#define EMBB_ATOMIC_METHOD_TO_GENERATE INIT_METHOD
#include <embb/base/c/internal/atomic/generate_atomic_implementation_template.h>
#undef EMBB_ATOMIC_METHOD_TO_GENERATE



// embb_mutex_destroy():

#ifdef EMBB_THREADING_ANALYSIS_MODE

#define EMBB_ATOMIC_INTERNAL_DEFINE_DESTROY_METHOD(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) \
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_atomic_destroy_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)(EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)* variable) \
  { \
    embb_mutex_destroy(&(variable->mutex)); \
    variable->init_flag = EMBB_ATOMIC_INTERNAL_INITIALIZED_VALUE - 1; \
  }

#else // EMBB_THREADING_ANALYSIS_MODE

#define EMBB_ATOMIC_INTERNAL_DEFINE_DESTROY_METHOD(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) \
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_atomic_destroy_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)(EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)* variable) \
  {}

#endif // else EMBB_THREADING_ANALYSIS_MODE

#undef EMBB_ATOMIC_METHOD_TO_GENERATE
#define EMBB_ATOMIC_METHOD_TO_GENERATE DESTROY_METHOD
#include <embb/base/c/internal/atomic/generate_atomic_implementation_template.h>
#undef EMBB_ATOMIC_METHOD_TO_GENERATE


#endif // EMBB_BASE_C_INTERNAL_ATOMIC_INIT_DESTROY_H_
