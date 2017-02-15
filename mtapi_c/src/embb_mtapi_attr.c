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

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <embb_mtapi_attr.h>

#define embb_mtapi_attr_implementation(TYPE) \
mtapi_status_t embb_mtapi_attr_set_##TYPE(TYPE* target, \
  const void* attribute, mtapi_size_t attribute_size) { \
  assert(MTAPI_NULL != target); \
\
  if (sizeof(TYPE) == attribute_size) { \
    assert(MTAPI_NULL != attribute); \
    memcpy(target, attribute, sizeof(TYPE)); \
    return MTAPI_SUCCESS; \
  } else if (MTAPI_ATTRIBUTE_POINTER_AS_VALUE == attribute_size) { \
    *target = (TYPE)(uintptr_t)attribute; \
    return MTAPI_SUCCESS; \
  } else { \
    return MTAPI_ERR_ATTR_SIZE; \
  } \
} \
\
mtapi_status_t embb_mtapi_attr_get_##TYPE(const TYPE* source, \
  void* attribute, mtapi_size_t attribute_size) { \
  assert(MTAPI_NULL != source); \
  assert(MTAPI_NULL != attribute); \
\
  if (sizeof(TYPE) == attribute_size) { \
    memcpy(attribute, source, sizeof(TYPE)); \
    return MTAPI_SUCCESS; \
  } else { \
    return MTAPI_ERR_ATTR_SIZE; \
  } \
}

embb_mtapi_attr_implementation(mtapi_uint_t);
embb_mtapi_attr_implementation(mtapi_affinity_t);
embb_mtapi_attr_implementation(mtapi_boolean_t);
