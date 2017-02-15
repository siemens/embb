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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_POOL_TEMPLATE_H_
#define MTAPI_C_SRC_EMBB_MTAPI_POOL_TEMPLATE_H_

#include <embb/mtapi/c/mtapi.h>

#include <embb_mtapi_id_pool_t.h>

#define embb_mtapi_pool(TYPE) \
\
/** \internal
TYPE pool class providing a fixed number of TYPE elements. Used to allocate
TYPE elements without using dynamic memory.

\ingroup INTERNAL
*/ \
struct embb_mtapi_##TYPE##_pool_struct \
{ \
  embb_mtapi_id_pool_t id_pool; \
  embb_mtapi_##TYPE##_t * storage; \
}; \
\
/** operator new with configurable capacity.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
embb_mtapi_##TYPE##_pool_t * embb_mtapi_##TYPE##_pool_new(\
  mtapi_uint_t capacity); \
\
/** operator delete.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
void embb_mtapi_##TYPE##_pool_delete(embb_mtapi_##TYPE##_pool_t * that); \
\
/** Constructor with configurable capacity.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
mtapi_boolean_t embb_mtapi_##TYPE##_pool_initialize(\
  embb_mtapi_##TYPE##_pool_t * that, \
  mtapi_uint_t capacity); \
\
/** Destructor.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
void embb_mtapi_##TYPE##_pool_finalize(\
  embb_mtapi_##TYPE##_pool_t * that); \
\
/** Allocate a single TYPE element in the pool.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
embb_mtapi_##TYPE##_t * embb_mtapi_##TYPE##_pool_allocate(\
  embb_mtapi_##TYPE##_pool_t * that); \
\
/** Deallocate given TYPE element in the pool.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
void embb_mtapi_##TYPE##_pool_deallocate(\
  embb_mtapi_##TYPE##_pool_t * that, \
  embb_mtapi_##TYPE##_t * object); \
\
/** Check if given pool handle is valid.
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
mtapi_boolean_t embb_mtapi_##TYPE##_pool_is_handle_valid(\
  embb_mtapi_##TYPE##_pool_t * that, \
  mtapi_##TYPE##_hndl_t handle); \
\
/** Return pointer to storage for given handle. Handle is expected to be valid,
so check it beforehand using embb_mtapi_##TYPE##_pool_is_handle_valid().
\memberof embb_mtapi_##TYPE##_pool_struct
*/ \
embb_mtapi_##TYPE##_t * embb_mtapi_##TYPE##_pool_get_storage_for_handle(\
  embb_mtapi_##TYPE##_pool_t * that, \
  mtapi_##TYPE##_hndl_t handle);

#endif // MTAPI_C_SRC_EMBB_MTAPI_POOL_TEMPLATE_H_
