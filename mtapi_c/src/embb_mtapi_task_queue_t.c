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

  that->front = MTAPI_NULL;
  that->back = MTAPI_NULL;
  mtapi_queueattr_init(&that->attributes, MTAPI_NULL);
  embb_spin_init(&that->lock);
}

void embb_mtapi_task_queue_finalize(embb_mtapi_task_queue_t* that) {
  that->front = MTAPI_NULL;
  that->back = MTAPI_NULL;
  embb_spin_destroy(&that->lock);
}

embb_mtapi_task_t * embb_mtapi_task_queue_pop_front(
  embb_mtapi_task_queue_t* that) {
  embb_mtapi_task_t * task = MTAPI_NULL;

  assert(MTAPI_NULL != that);

  if (embb_spin_try_lock(&that->lock, 128) == EMBB_SUCCESS) {
    if (MTAPI_NULL != that->front) {
      task = that->front;
      if (that->front == that->back) {
        that->front = MTAPI_NULL;
        that->back = MTAPI_NULL;
      } else {
        that->front = task->next;
      }
    }
    embb_spin_unlock(&that->lock);
  }

  return task;
}

mtapi_boolean_t embb_mtapi_task_queue_push_back(
  embb_mtapi_task_queue_t* that,
  embb_mtapi_task_t * task) {
  mtapi_boolean_t result = MTAPI_FALSE;

  assert(MTAPI_NULL != that);

  if (embb_spin_lock(&that->lock) == EMBB_SUCCESS) {
    task->next = MTAPI_NULL;
    if (MTAPI_NULL == that->front) {
      that->front = task;
    } else {
      that->back->next = task;
    }
    that->back = task;
    result = MTAPI_TRUE;
    embb_spin_unlock(&that->lock);
  }

  return result;
}

mtapi_boolean_t embb_mtapi_task_queue_push_front(
  embb_mtapi_task_queue_t* that,
  embb_mtapi_task_t * task) {
  mtapi_boolean_t result = MTAPI_FALSE;

  assert(MTAPI_NULL != that);

  if (embb_spin_lock(&that->lock) == EMBB_SUCCESS) {
    task->next = that->front;
    if (MTAPI_NULL == that->front) {
      that->back = task;
    }
    that->front = task;
    result = MTAPI_TRUE;
    embb_spin_unlock(&that->lock);
  }

  return result;
}

void embb_mtapi_task_queue_process(
  embb_mtapi_task_queue_t * that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data) {
  mtapi_boolean_t result;
  embb_mtapi_task_t * task;
  embb_mtapi_task_t * prev = MTAPI_NULL;
  embb_mtapi_task_t * next;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != process);

  if (embb_spin_lock(&that->lock) == EMBB_SUCCESS) {
    for (task = that->front; task != MTAPI_NULL; task = task->next) {
      /* store next task, might get destroyed if process requests
         removal of current task from queue */
      next = task->next;
      /* process the task */
      result = process(task, user_data);
      /* remove task from queue? */
      if (MTAPI_FALSE == result) {
        if (task == that->front) {
          that->front = next;
        }
        if (task == that->back) {
          that->back = prev;
        }
        if (prev != MTAPI_NULL) {
          prev->next = next;
        }
        /* do not update previous task, as the current one is gone */
      } else {
        /* store task as previous task */
        prev = task;
      }
    }
    embb_spin_unlock(&that->lock);
  }
}
