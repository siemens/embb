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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_POOL_TEMPLATE_INL_H_
#define MTAPI_C_SRC_EMBB_MTAPI_POOL_TEMPLATE_INL_H_

#include <assert.h>
#include <embb/mtapi/c/mtapi.h>

#include <embb_mtapi_alloc.h>
#include <embb_mtapi_pool_template.h>

#define embb_mtapi_pool_implementation(TYPE) \
\
/* ---- POOL STORAGE FUNCTIONS ------------------------------------------- */ \
\
embb_mtapi_##TYPE##_pool_t * embb_mtapi_##TYPE##_pool_new( \
  mtapi_uint_t capacity) { \
  embb_mtapi_##TYPE##_pool_t * that = (embb_mtapi_##TYPE##_pool_t*) \
    embb_mtapi_alloc_allocate(sizeof(embb_mtapi_##TYPE##_pool_t)); \
  if (MTAPI_NULL != that) { \
    embb_mtapi_##TYPE##_pool_initialize(that, capacity); \
  } \
  return that; \
} \
\
void embb_mtapi_##TYPE##_pool_delete(embb_mtapi_##TYPE##_pool_t * that) { \
  assert(MTAPI_NULL != that); \
  embb_mtapi_##TYPE##_pool_finalize(that); \
  embb_mtapi_alloc_deallocate(that); \
} \
\
mtapi_boolean_t embb_mtapi_##TYPE##_pool_initialize( \
  embb_mtapi_##TYPE##_pool_t * that, \
  mtapi_uint_t capacity) { \
  mtapi_uint_t ii; \
  assert(MTAPI_NULL != that); \
  embb_mtapi_id_pool_initialize(&that->id_pool, capacity); \
  that->storage = (embb_mtapi_##TYPE##_t*)embb_mtapi_alloc_allocate( \
    sizeof(embb_mtapi_##TYPE##_t)*(capacity + 1)); \
  if (NULL != that->storage) { \
    for (ii = 0; ii <= capacity; ii++) { \
      that->storage[ii].handle.id = EMBB_MTAPI_IDPOOL_INVALID_ID; \
      that->storage[ii].handle.tag = 0; \
    } \
    return MTAPI_TRUE; \
  } else { \
    that->id_pool.ids_available = 0; \
    return MTAPI_FALSE; \
  } \
} \
\
void embb_mtapi_##TYPE##_pool_finalize(embb_mtapi_##TYPE##_pool_t * that) { \
  mtapi_uint_t ii; \
  assert(MTAPI_NULL != that); \
  if (NULL != that->storage) { \
    for (ii = 0; ii <= that->id_pool.capacity; ii++) { \
      if (that->storage[ii].handle.id != EMBB_MTAPI_IDPOOL_INVALID_ID) { \
        embb_mtapi_##TYPE##_finalize(&that->storage[ii]); \
      } \
    } \
  } \
  embb_mtapi_alloc_deallocate(that->storage); \
  embb_mtapi_id_pool_finalize(&that->id_pool); \
  that->storage = MTAPI_NULL; \
} \
\
embb_mtapi_##TYPE##_t * embb_mtapi_##TYPE##_pool_allocate( \
  embb_mtapi_##TYPE##_pool_t * that) { \
  mtapi_uint_t pool_id = embb_mtapi_id_pool_allocate(&that->id_pool); \
  if (EMBB_MTAPI_IDPOOL_INVALID_ID != pool_id) { \
    that->storage[pool_id].handle.id = pool_id; \
    return &that->storage[pool_id]; \
  } else { \
    return MTAPI_NULL; \
  } \
} \
\
void embb_mtapi_##TYPE##_pool_deallocate( \
  embb_mtapi_##TYPE##_pool_t * that, \
  embb_mtapi_##TYPE##_t * object) { \
  mtapi_uint_t pool_id = object->handle.id; \
  object->handle.id = EMBB_MTAPI_IDPOOL_INVALID_ID; \
  object->handle.tag++; \
  embb_mtapi_id_pool_deallocate(&that->id_pool, pool_id); \
} \
\
mtapi_boolean_t embb_mtapi_##TYPE##_pool_is_handle_valid( \
  embb_mtapi_##TYPE##_pool_t * that, \
  mtapi_##TYPE##_hndl_t handle) { \
  assert(MTAPI_NULL != that); \
  return ((0 < handle.id) && \
    (handle.id <= that->id_pool.capacity) && \
    (that->storage[handle.id].handle.tag == handle.tag)) ? \
      MTAPI_TRUE : MTAPI_FALSE; \
} \
\
embb_mtapi_##TYPE##_t * embb_mtapi_##TYPE##_pool_get_storage_for_handle( \
  embb_mtapi_##TYPE##_pool_t * that, \
  mtapi_##TYPE##_hndl_t handle) { \
  assert(MTAPI_NULL != that); \
  assert(embb_mtapi_##TYPE##_pool_is_handle_valid(that, handle)); \
  return &that->storage[handle.id]; \
}

#endif // MTAPI_C_SRC_EMBB_MTAPI_POOL_TEMPLATE_INL_H_
