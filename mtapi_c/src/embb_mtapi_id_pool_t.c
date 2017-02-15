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

#include <embb_mtapi_alloc.h>
#include <embb_mtapi_log.h>
#include <embb_mtapi_id_pool_t.h>
#include <embb/base/c/mutex.h>

void embb_mtapi_id_pool_initialize(
  embb_mtapi_id_pool_t * that,
  mtapi_uint_t capacity) {
  mtapi_uint_t ii;

  that->id_buffer = (mtapi_uint_t*)
    embb_mtapi_alloc_allocate(sizeof(mtapi_uint_t)*(capacity + 1));
  if (NULL != that->id_buffer) {
    that->capacity = capacity;
    that->id_buffer[0] = EMBB_MTAPI_IDPOOL_INVALID_ID;
    for (ii = 1; ii <= capacity; ii++) {
      that->id_buffer[ii] = ii;
    }
    that->ids_available = capacity;
  } else {
    that->capacity = 0;
    that->ids_available = 0;
  }
  that->put_id_position = 0;
  that->get_id_position = 1;
  embb_spin_init(&that->lock);
}

void embb_mtapi_id_pool_finalize(embb_mtapi_id_pool_t * that) {
  that->capacity = 0;
  that->ids_available = 0;
  that->get_id_position = 0;
  that->put_id_position = 0;
  embb_mtapi_alloc_deallocate(that->id_buffer);
  that->id_buffer = NULL;
  embb_spin_destroy(&that->lock);
}

mtapi_uint_t embb_mtapi_id_pool_allocate(embb_mtapi_id_pool_t * that) {
  mtapi_uint_t id = EMBB_MTAPI_IDPOOL_INVALID_ID;

  assert(MTAPI_NULL != that);

  if (embb_spin_lock(&that->lock) == EMBB_SUCCESS) {
    if (0 < that->ids_available) {
      /* take away one id */
      that->ids_available--;

      /* acquire position to fetch id from */
      mtapi_uint_t id_position = that->get_id_position;
      that->get_id_position++;
      if (that->capacity < that->get_id_position) {
        that->get_id_position = 0;
      }

      /* fetch id */
      id = that->id_buffer[id_position];

      /* make id entry invalid just in case */
      that->id_buffer[id_position] = EMBB_MTAPI_IDPOOL_INVALID_ID;
    }
    embb_spin_unlock(&that->lock);
  }

  return id;
}

void embb_mtapi_id_pool_deallocate(
  embb_mtapi_id_pool_t * that,
  mtapi_uint_t id) {
  assert(MTAPI_NULL != that);

  if (embb_spin_lock(&that->lock) == EMBB_SUCCESS) {
    if (that->capacity > that->ids_available) {
      /* acquire position to put id to */
      mtapi_uint_t id_position = that->put_id_position;
      that->put_id_position++;
      if (that->capacity < that->put_id_position) {
        that->put_id_position = 0;
      }

      /* put id back into buffer */
      that->id_buffer[id_position] = id;

      /* make it available */
      that->ids_available++;
    }
    embb_spin_unlock(&that->lock);
  } else {
    embb_mtapi_log_error(
      "could not acquire lock in embb_mtapi_IdPool_deallocate\n");
  }
}
