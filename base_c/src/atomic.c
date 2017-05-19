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

#include <embb/base/c/atomic.h>
#include <embb/base/c/internal/macro_helper.h>

// Break compiling, if we assumed wrong sizes for types...
// Function should never be called!
void __embb_atomic_internal_compile_time_checks() {
  BUILD_BUG_ON(sizeof(char) != EMBB_CHAR_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(short) != EMBB_SHORT_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(unsigned short) != EMBB_UNSIGNED_SHORT_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(int) != EMBB_INT_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(unsigned int) != EMBB_UNSIGNED_INT_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(long) != EMBB_LONG_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(unsigned long) != EMBB_UNSIGNED_LONG_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(long long) != EMBB_LONG_LONG_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(unsigned long long) != EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(intptr_t) != EMBB_INTPTR_T_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(uintptr_t) != EMBB_UINTPTR_T_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(size_t) != EMBB_SIZE_T_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(ptrdiff_t) != EMBB_PTRDIFF_T_TYPE_SIZE);
  BUILD_BUG_ON(sizeof(uintmax_t) != EMBB_UINTMAX_T_TYPE_SIZE);
}
