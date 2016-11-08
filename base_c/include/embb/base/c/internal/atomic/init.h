/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_INIT_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_INIT_H_

#ifndef DOXYGEN

#include <embb/base/c/internal/config.h>
#include <embb/base/c/internal/atomic/atomic_sizes.h>
#include <embb/base/c/internal/macro_helper.h>
#include <embb/base/c/internal/atomic/atomic_variables.h>
#include <embb/base/c/internal/atomic/store.h>
#include <string.h>

/*
* See file and_assign.h for a detailed (and operation independent) description
* of the following macro.
*/
#define EMBB_ATOMIC_INTERNAL_DEFINE_INIT_METHOD(EMBB_ATOMIC_PARAMETER_TYPE_NATIVE, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX, EMBB_ATOMIC_PARAMETER_TYPE_SIZE) \
  EMBB_PLATFORM_INLINE void EMBB_CAT2(embb_atomic_init_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)(\
  EMBB_CAT2(embb_atomic_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)* variable, \
  EMBB_ATOMIC_PARAMETER_TYPE_NATIVE value) { \
  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex); \
  EMBB_ATOMIC_INIT_MARKER(variable); \
  EMBB_CAT2(embb_atomic_store_, EMBB_ATOMIC_PARAMETER_ATOMIC_TYPE_SUFFIX)(variable, value); \
  }

#undef EMBB_ATOMIC_METHOD_TO_GENERATE
#define EMBB_ATOMIC_METHOD_TO_GENERATE INIT_METHOD
#include <embb/base/c/internal/atomic/generate_atomic_implementation_template.h>
#undef EMBB_ATOMIC_METHOD_TO_GENERATE

#endif //DOXYGEN

#endif //EMBB_BASE_C_INTERNAL_ATOMIC_INIT_H_

