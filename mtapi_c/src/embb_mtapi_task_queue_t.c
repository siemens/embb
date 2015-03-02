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

#include <assert.h>

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_log.h>
#include <embb_mtapi_task_queue_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_alloc.h>


/* ---- CLASS MEMBERS ------------------------------------------------------ */

void embb_mtapi_task_queue_initialize(embb_mtapi_task_queue_t* that) {
  assert(MTAPI_NULL != that);

  that->task_buffer = MTAPI_NULL;
  that->tasks_available = 0;
  that->get_task_position = 0;
  that->put_task_position = 0;
  mtapi_queueattr_init(&that->attributes, MTAPI_NULL);
  embb_mtapi_spinlock_initialize(&that->lock);
}

void embb_mtapi_task_queue_initialize_with_capacity(
  embb_mtapi_task_queue_t* that,
  mtapi_uint_t capacity) {
  assert(MTAPI_NULL != that);

  that->task_buffer = (embb_mtapi_task_t **)
    embb_mtapi_alloc_allocate(sizeof(embb_mtapi_task_t *)*capacity);
  that->tasks_available = 0;
  that->get_task_position = 0;
  that->put_task_position = 0;
  mtapi_queueattr_init(&that->attributes, MTAPI_NULL);
  that->attributes.limit = capacity;
  embb_mtapi_spinlock_initialize(&that->lock);
}

void embb_mtapi_task_queue_finalize(embb_mtapi_task_queue_t* that) {
  embb_mtapi_alloc_deallocate(that->task_buffer);
  that->task_buffer = MTAPI_NULL;

  embb_mtapi_task_queue_initialize(that);

  embb_mtapi_spinlock_finalize(&that->lock);
}

embb_mtapi_task_t * embb_mtapi_task_queue_pop(embb_mtapi_task_queue_t* that) {
  embb_mtapi_task_t * task = MTAPI_NULL;

  assert(MTAPI_NULL != that);

  if (embb_mtapi_spinlock_acquire_with_spincount(&that->lock, 128)) {
    if (0 < that->tasks_available) {
      /* take away one task */
      that->tasks_available--;

      /* acquire position to fetch task from */
      mtapi_uint_t task_position = that->get_task_position;
      that->get_task_position++;
      if (that->attributes.limit <= that->get_task_position) {
        that->get_task_position = 0;
      }

      /* fetch task */
      task = that->task_buffer[task_position];

      /* make task entry invalid just in case */
      that->task_buffer[task_position] = MTAPI_NULL;
    }
    embb_mtapi_spinlock_release(&that->lock);
  }

  return task;
}

mtapi_boolean_t embb_mtapi_task_queue_push(
  embb_mtapi_task_queue_t* that,
  embb_mtapi_task_t * task) {
  mtapi_boolean_t result = MTAPI_FALSE;

  assert(MTAPI_NULL != that);

  if (embb_mtapi_spinlock_acquire(&that->lock)) {
    if (that->attributes.limit > that->tasks_available) {
      /* acquire position to put task into */
      mtapi_uint_t task_position = that->put_task_position;
      that->put_task_position++;
      if (that->attributes.limit <= that->put_task_position) {
        that->put_task_position = 0;
      }

      /* put task into buffer */
      that->task_buffer[task_position] = task;

      /* make task available */
      that->tasks_available++;

      result = MTAPI_TRUE;
    }
    embb_mtapi_spinlock_release(&that->lock);
  }

  return result;
}

mtapi_boolean_t embb_mtapi_task_queue_process(
  embb_mtapi_task_queue_t * that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data) {
  mtapi_boolean_t result = MTAPI_TRUE;
  mtapi_uint_t ii;
  mtapi_uint_t idx;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != process);

  if (embb_mtapi_spinlock_acquire(&that->lock)) {
    idx = that->get_task_position;
    for (ii = 0; ii < that->tasks_available; ii++) {
      result = process(that->task_buffer[ii], user_data);
      if (MTAPI_FALSE == result) {
        break;
      }
      idx = (idx + 1) % that->attributes.limit;
    }
    embb_mtapi_spinlock_release(&that->lock);
  }

  return result;
}
