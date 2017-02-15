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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_ID_POOL_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_ID_POOL_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/mutex.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * IdPool class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_id_pool_struct {
  mtapi_uint_t capacity;
  mtapi_uint_t *id_buffer;
  mtapi_uint_t ids_available;
  mtapi_uint_t get_id_position;
  mtapi_uint_t put_id_position;
  embb_spinlock_t lock;
};

/**
 * IdPool type.
 * \memberof embb_mtapi_id_pool_struct
 */
typedef struct embb_mtapi_id_pool_struct embb_mtapi_id_pool_t;

#define EMBB_MTAPI_IDPOOL_INVALID_ID 0

/**
 * Constructor with configurable capacity.
 * \memberof embb_mtapi_id_pool_struct
 */
void embb_mtapi_id_pool_initialize(
  embb_mtapi_id_pool_t * that,
  mtapi_uint_t capacity);

/**
 * Destructor.
 * \memberof embb_mtapi_id_pool_struct
 */
void embb_mtapi_id_pool_finalize(embb_mtapi_id_pool_t * that);

/**
 * Allocates a single item and removes its id from the pool.
 * \memberof embb_mtapi_id_pool_struct
 */
mtapi_uint_t embb_mtapi_id_pool_allocate(embb_mtapi_id_pool_t * that);

/**
 * Dellocates a single item and puts its id back into the pool.
 * \memberof embb_mtapi_id_pool_struct
 */
void embb_mtapi_id_pool_deallocate(
  embb_mtapi_id_pool_t * that,
  mtapi_uint_t id);


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_ID_POOL_T_H_
