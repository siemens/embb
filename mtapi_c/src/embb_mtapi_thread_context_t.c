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

#include <embb_mtapi_log.h>
#include <embb_mtapi_alloc.h>
#include <embb_mtapi_task_queue_t.h>
#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_thread_context_t.h>


/* ---- CLASS MEMBERS ------------------------------------------------------ */

mtapi_boolean_t embb_mtapi_thread_context_initialize(
  embb_mtapi_thread_context_t* that,
  embb_mtapi_node_t* node,
  mtapi_uint_t worker_index,
  mtapi_uint_t core_num,
  embb_thread_priority_t priority) {
  mtapi_uint_t ii;
  mtapi_boolean_t result = MTAPI_TRUE;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != node);

  that->node = node;
  that->worker_index = worker_index;
  that->core_num = core_num;
  that->priorities = node->attributes.max_priorities;
  that->is_initialized = MTAPI_FALSE;
  that->thread_priority = priority;
  that->is_main_thread = (worker_index == 0) ?
    node->attributes.reuse_main_thread : MTAPI_FALSE;

  embb_atomic_init_int(&that->run, 0);
  embb_atomic_init_int(&that->is_sleeping, 0);

  that->queue = (embb_mtapi_task_queue_t**)embb_mtapi_alloc_allocate(
    sizeof(embb_mtapi_task_queue_t*)*that->priorities);
  if (that->queue == NULL) {
    that->private_queue = NULL;
    return MTAPI_FALSE;
  }
  for (ii = 0; ii < that->priorities; ii++) {
    that->queue[ii] = (embb_mtapi_task_queue_t*)
      embb_mtapi_alloc_allocate(sizeof(embb_mtapi_task_queue_t));
    if (that->queue[ii] != NULL) {
      embb_mtapi_task_queue_initialize(that->queue[ii]);
    } else {
      result = MTAPI_FALSE;
    }
  }
  if (!result) {
    return MTAPI_FALSE;
  }

  that->private_queue = (embb_mtapi_task_queue_t**)embb_mtapi_alloc_allocate(
    sizeof(embb_mtapi_task_queue_t*)*that->priorities);
  if (that->private_queue == NULL) {
    return MTAPI_FALSE;
  }
  for (ii = 0; ii < that->priorities; ii++) {
    that->private_queue[ii] = (embb_mtapi_task_queue_t*)
      embb_mtapi_alloc_allocate(sizeof(embb_mtapi_task_queue_t));
    if (that->private_queue[ii] != NULL) {
      embb_mtapi_task_queue_initialize(that->private_queue[ii]);
    } else {
      result = MTAPI_FALSE;
    }
  }
  if (!result) {
    return MTAPI_FALSE;
  }

  embb_mutex_init(&that->work_available_mutex, EMBB_MUTEX_PLAIN);
  embb_condition_init(&that->work_available);

  that->is_initialized = MTAPI_TRUE;

  return MTAPI_TRUE;
}

mtapi_boolean_t embb_mtapi_thread_context_start(
  embb_mtapi_thread_context_t* that,
  embb_mtapi_scheduler_t * scheduler) {
  int err;
  embb_mtapi_scheduler_worker_func_t * worker_func;
  embb_core_set_t core_set;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != scheduler);

  worker_func = embb_mtapi_scheduler_worker_func(scheduler);

  /* pin thread to core */
  embb_core_set_init(&core_set, 0);
  embb_core_set_add(&core_set, that->core_num);

  /* create thread */
  if (that->is_main_thread) {
    /* reuse main thread */
    that->thread = embb_thread_current();
    err = embb_tss_create(&that->tss_id);
    if (EMBB_SUCCESS != err) {
      /* report error to scheduler */
      embb_atomic_store_int(&that->run, -1);
      return MTAPI_FALSE;
    }
    embb_tss_set(&(that->tss_id), that);
    embb_atomic_store_int(&that->run, 1);
  } else {
    err = embb_thread_create_with_priority(
      &that->thread, &core_set, that->thread_priority, worker_func, that);
    if (EMBB_SUCCESS != err) {
      embb_mtapi_log_error(
        "embb_mtapi_ThreadContext_initializeWithNodeAndCoreNumber() could not "
        "create thread %d on core %d\n", that->worker_index, that->core_num);
      return MTAPI_FALSE;
    }
    /* wait for worker to come up */
    while (0 == embb_atomic_load_int(&that->run)) {
      embb_thread_yield();
    }
  }

  if (0 < embb_atomic_load_int(&that->run)) {
    return MTAPI_TRUE;
  } else {
    return MTAPI_FALSE;
  }
}

void embb_mtapi_thread_context_stop(embb_mtapi_thread_context_t* that) {
  int result;
  if (0 < embb_atomic_load_int(&that->run)) {
    embb_atomic_store_int(&that->run, 0);
    embb_condition_notify_one(&that->work_available);
    if (MTAPI_FALSE == that->is_main_thread) {
      embb_thread_join(&(that->thread), &result);
    }
  }
}

void embb_mtapi_thread_context_finalize(embb_mtapi_thread_context_t* that) {
  mtapi_uint_t ii;

  assert(MTAPI_NULL != that);

  embb_mtapi_log_trace("embb_mtapi_thread_context_finalize() called\n");

  if (that->is_initialized) {
    if (that->is_main_thread) {
      embb_tss_delete(&that->tss_id);
    }
    embb_condition_destroy(&that->work_available);
    embb_mutex_destroy(&that->work_available_mutex);
  }

  if (that->queue != NULL) {
    for (ii = 0; ii < that->priorities; ii++) {
      if (that->queue[ii] != NULL) {
        embb_mtapi_task_queue_finalize(that->queue[ii]);
        embb_mtapi_alloc_deallocate(that->queue[ii]);
        that->queue[ii] = MTAPI_NULL;
      }
    }
    embb_mtapi_alloc_deallocate(that->queue);
    that->queue = MTAPI_NULL;
  }

  if (that->private_queue != NULL) {
    for (ii = 0; ii < that->priorities; ii++) {
      if (that->private_queue[ii] != NULL) {
        embb_mtapi_task_queue_finalize(that->private_queue[ii]);
        embb_mtapi_alloc_deallocate(that->private_queue[ii]);
        that->private_queue[ii] = MTAPI_NULL;
      }
    }
    embb_mtapi_alloc_deallocate(that->private_queue);
    that->private_queue = MTAPI_NULL;
  }

  embb_atomic_destroy_int(&that->is_sleeping);
  embb_atomic_destroy_int(&that->run);

  that->priorities = 0;
  that->is_initialized = MTAPI_FALSE;

  that->node = MTAPI_NULL;
}

mtapi_boolean_t embb_mtapi_thread_context_process_tasks(
  embb_mtapi_thread_context_t* that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data) {
  mtapi_uint_t ii;
  mtapi_boolean_t result = MTAPI_TRUE;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != process);

  for (ii = 0; ii < that->priorities; ii++) {
    embb_mtapi_task_queue_process(
      that->private_queue[ii], process, user_data);
    embb_mtapi_task_queue_process(
      that->queue[ii], process, user_data);
  }

  return result;
}
