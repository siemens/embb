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

#include <embb/base/c/base.h>

#include <embb/base/c/internal/unused.h>

#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_log.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_task_queue_t.h>
#include <embb_mtapi_thread_context_t.h>
#include <embb_mtapi_task_context_t.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_alloc.h>
#include <embb_mtapi_queue_t.h>


/* ---- CLASS MEMBERS ------------------------------------------------------ */

embb_mtapi_task_t * embb_mtapi_scheduler_get_private_task_from_context(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_thread_context_t * thread_context,
  mtapi_uint_t priority) {
  EMBB_UNUSED(that);

  assert(MTAPI_NULL != that);
  assert(NULL != thread_context);

  embb_mtapi_task_t * task =
    embb_mtapi_task_queue_pop(thread_context->private_queue[priority]);
  return task;
}

embb_mtapi_task_t * embb_mtapi_scheduler_get_public_task_from_context(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_thread_context_t * thread_context,
  mtapi_uint_t priority) {
  EMBB_UNUSED(that);

  embb_mtapi_task_t * task;

  assert(MTAPI_NULL != that);
  assert(NULL != thread_context);

  task = embb_mtapi_task_queue_pop(thread_context->queue[priority]);
  return task;
}

embb_mtapi_task_t * embb_mtapi_scheduler_get_next_task_vhpf(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context) {
  embb_mtapi_task_t * task = MTAPI_NULL;
  mtapi_uint_t ii = 0;
  mtapi_uint_t kk = 0;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != node);
  assert(NULL != thread_context);

  for (ii = 0;
    ii < node->attributes.max_priorities && MTAPI_NULL == task;
    ii++) {
    /* try local queues, first private. */
    task = embb_mtapi_scheduler_get_private_task_from_context(
      that, thread_context, ii);
    if (MTAPI_NULL == task) {
      /* found nothing, so local public next. */
      task = embb_mtapi_scheduler_get_public_task_from_context(
        that, thread_context, ii);
      if (MTAPI_NULL == task) {
        /* still nothing, steal from public queues of other workers.
           the process starts at the worker "after" the current worker,
           it might be better to start at a random worker
        */
        mtapi_uint_t context_index =
          (thread_context->worker_index + 1) % that->worker_count;
        for (kk = 0;
          kk < that->worker_count - 1 && MTAPI_NULL == task;
          kk++) {
          task = embb_mtapi_task_queue_pop(
            that->worker_contexts[context_index].queue[ii]);
          context_index =
            (context_index + 1) % that->worker_count;
        }
      }
    }
  }
  return task;
}

embb_mtapi_task_t * embb_mtapi_scheduler_get_next_task_lf(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context) {
  embb_mtapi_task_t * task = MTAPI_NULL;
  mtapi_uint_t prio = 0;
  mtapi_uint_t kk = 0;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != node);
  assert(NULL != thread_context);

  /* Try local queues on all priorities, first private. */
  for (prio = 0;
    MTAPI_NULL == task && prio < node->attributes.max_priorities;
    prio++) {
    task = embb_mtapi_scheduler_get_private_task_from_context(
      that, thread_context, prio);
  }

  /* found nothing, so local public next. */
  for (prio = 0;
    MTAPI_NULL == task && prio < node->attributes.max_priorities;
    prio++) {
    task = embb_mtapi_scheduler_get_public_task_from_context(
      that, thread_context, prio);
  }

  /* still nothing, steal from public queues of other workers.
     the process starts at the worker "after" the current worker,
     it might be better to start at a random worker
  */
  for (prio = 0;
    MTAPI_NULL == task && prio < node->attributes.max_priorities;
    prio++) {
    mtapi_uint_t context_index =
      (thread_context->worker_index + 1) % that->worker_count;
    for (kk = 0;
      kk < that->worker_count - 1 && MTAPI_NULL == task;
      kk++) {
      task = embb_mtapi_task_queue_pop(
        that->worker_contexts[context_index].queue[prio]);
      context_index =
        (context_index + 1) % that->worker_count;
    }
  }
  return task;
}

embb_mtapi_task_t * embb_mtapi_scheduler_get_next_task(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context) {
  embb_mtapi_task_t * task = MTAPI_NULL;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != node);
  assert(NULL != thread_context);

  switch (that->mode) {
  case WORK_STEAL_LF:
    task = embb_mtapi_scheduler_get_next_task_lf(
      that, node, thread_context);
    break;
  case WORK_STEAL_VHPF:
    task = embb_mtapi_scheduler_get_next_task_vhpf(
      that, node, thread_context);
    break;
  case NUM_SCHEDULER_MODES:
  default:
    embb_mtapi_log_error(
      "embb_mtapi_Scheduler_getNextTask() unknown scheduler mode: %d\n",
      that->mode);
  }
  return task;
}

embb_mtapi_thread_context_t * embb_mtapi_scheduler_get_current_thread_context(
  embb_mtapi_scheduler_t * that) {
  mtapi_uint_t ii = 0;
  embb_mtapi_thread_context_t * context = NULL;

  assert(MTAPI_NULL != that);

  /* find out on which thread we are */
  for (ii = 0; ii < that->worker_count; ii++) {
    context = (embb_mtapi_thread_context_t*)embb_tss_get(
      &(that->worker_contexts[ii].tss_id));
    if (NULL != context) {
      break;
    }
  }

  return context;
}

void embb_mtapi_scheduler_execute_task_or_yield(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context) {
  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != node);

  if (NULL != thread_context) {
    embb_mtapi_task_t* new_task = embb_mtapi_scheduler_get_next_task(
      that, node, thread_context);
    /* if there was work, execute it */
    if (MTAPI_NULL != new_task) {
      embb_mtapi_task_context_t task_context;
      embb_mtapi_task_context_initialize_with_thread_context_and_task(
        &task_context, thread_context, new_task);
      embb_mtapi_task_execute(new_task, &task_context);
    } else {
      embb_thread_yield();
    }
  } else {
    embb_thread_yield();
  }
}

embb_mtapi_scheduler_worker_func_t *
embb_mtapi_scheduler_worker_func(embb_mtapi_scheduler_t * that) {
  EMBB_UNUSED(that);

  assert(MTAPI_NULL != that);

  /* Currently just returns embb_mtapi_scheduler_worker,
     but could return any custom worker function, e.g. depending
     on scheduler->mode.
  */
  return &embb_mtapi_scheduler_worker;
}

int embb_mtapi_scheduler_worker(void * arg) {
  embb_mtapi_thread_context_t * thread_context =
    (embb_mtapi_thread_context_t*)arg;
  embb_mtapi_task_context_t task_context;
  embb_mtapi_node_t * node;
  embb_duration_t sleep_duration;
  int err;
  int counter = 0;

  embb_mtapi_log_trace(
    "embb_mtapi_scheduler_worker() called for thread %d on core %d\n",
    thread_context->worker_index, thread_context->core_num);

  assert(MTAPI_NULL != thread_context);

  err = embb_tss_create(&thread_context->tss_id);
  if (EMBB_SUCCESS != err) {
    /* report error to scheduler */
    embb_atomic_store_int(&thread_context->run, -1);
    return MTAPI_FALSE;
  }

  /* node is initialized here, otherwise the worker would not run */
  node = thread_context->node;

  embb_tss_set(&(thread_context->tss_id), thread_context);

  embb_duration_set_milliseconds(&sleep_duration, 10);

  /* signal that we're up & running */
  embb_atomic_store_int(&thread_context->run, 1);
  /* potentially wait for node to come up completely */
  while (MTAPI_FALSE == embb_atomic_load_int(&node->is_scheduler_running)) {
    embb_thread_yield();
  }

  /* do work while not requested to stop */
  while (embb_atomic_load_int(&thread_context->run)) {
    /* try to get work */
    embb_mtapi_task_t * task = embb_mtapi_scheduler_get_next_task(
      node->scheduler, node, thread_context);
    /* check if there was work */
    if (MTAPI_NULL != task) {
      embb_mtapi_queue_t * local_queue = MTAPI_NULL;

      /* is task associated with a queue? */
      if (embb_mtapi_queue_pool_is_handle_valid(
        node->queue_pool, task->queue)) {
        local_queue =
          embb_mtapi_queue_pool_get_storage_for_handle(
            node->queue_pool, task->queue);
      }

      switch (task->state) {
      case MTAPI_TASK_SCHEDULED:
      /* multi-instance task, another instance might be running */
      case MTAPI_TASK_RUNNING:
        /* there was work, execute it */
        embb_mtapi_task_context_initialize_with_thread_context_and_task(
          &task_context, thread_context, task);
        if (embb_mtapi_task_execute(task, &task_context)) {
          /* tell queue that a task is done */
          if (MTAPI_NULL != local_queue) {
            embb_mtapi_queue_task_finished(local_queue);
          }
        }
        counter = 0;
        break;

      case MTAPI_TASK_RETAINED:
        /* put task into queue again for later execution */
        embb_mtapi_scheduler_schedule_task(
          node->scheduler, task, 0);
        /* yield, as there may be only retained tasks in the queue */
        embb_thread_yield();
        /* task is not done, so do not notify queue */
        break;

      case MTAPI_TASK_CANCELLED:
        /* set return value to canceled */
        task->error_code = MTAPI_ERR_ACTION_CANCELLED;
        if (embb_atomic_fetch_and_add_unsigned_int(
          &task->instances_todo, (unsigned int)-1) == 0) {
          /* tell queue that a task is done */
          if (MTAPI_NULL != local_queue) {
            embb_mtapi_queue_task_finished(local_queue);
          }
        }
        break;

      case MTAPI_TASK_COMPLETED:
      case MTAPI_TASK_DELETED:
      case MTAPI_TASK_WAITING:
      case MTAPI_TASK_CREATED:
      case MTAPI_TASK_PRENATAL:
      case MTAPI_TASK_ERROR:
      case MTAPI_TASK_INTENTIONALLY_UNUSED:
      default:
        /* do nothing, although this is an error */
        break;
      }

      /* issue task complete callback if set */
      if (MTAPI_NULL != task->attributes.complete_func) {
        task->attributes.complete_func(task->handle, MTAPI_NULL);
      }
    } else if (counter < 1024) {
      /* spin and yield for a while before going to sleep */
      embb_thread_yield();
      counter++;
    } else {
      /* no work, go to sleep */
      embb_atomic_store_int(&thread_context->is_sleeping, 1);
      embb_mutex_lock(&thread_context->work_available_mutex);
      embb_condition_wait_for(
        &thread_context->work_available,
        &thread_context->work_available_mutex,
        &sleep_duration);
      embb_mutex_unlock(&thread_context->work_available_mutex);
      embb_atomic_store_int(&thread_context->is_sleeping, 0);
    }
  }

  embb_tss_delete(&(thread_context->tss_id));

  return MTAPI_TRUE;
}

mtapi_boolean_t embb_mtapi_scheduler_wait_for_task(
  embb_mtapi_task_t * task,
  mtapi_timeout_t timeout) {
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  embb_mtapi_thread_context_t * context = NULL;
  embb_duration_t wait_duration;
  embb_time_t end_time;

  assert(MTAPI_NULL != node);
  assert(MTAPI_NULL != task);

  if (MTAPI_INFINITE < timeout) {
    embb_duration_set_milliseconds(&wait_duration, (unsigned long long)timeout);
    embb_time_in(&end_time, &wait_duration);
  }

  /* find out on which thread we are */
  context = embb_mtapi_scheduler_get_current_thread_context(
    node->scheduler);

  /* now wait and schedule new tasks if we are on a worker */
  while (
    (MTAPI_TASK_SCHEDULED == task->state) ||
    (MTAPI_TASK_RUNNING == task->state) ||
    (MTAPI_TASK_RETAINED == task->state) ) {
    if (MTAPI_INFINITE < timeout) {
      embb_time_t current_time;
      embb_time_now(&current_time);
      if (embb_time_compare(&current_time, &end_time) > 0) {
        /* timeout! */
        return MTAPI_FALSE;
      }
    }

    /* do other work if applicable */
    embb_mtapi_scheduler_execute_task_or_yield(
      node->scheduler,
      node,
      context);
  }

  return MTAPI_TRUE;
}

mtapi_boolean_t embb_mtapi_scheduler_initialize(
  embb_mtapi_scheduler_t * that) {
  return embb_mtapi_scheduler_initialize_with_mode(that, WORK_STEAL_VHPF);
}

mtapi_boolean_t embb_mtapi_scheduler_initialize_with_mode(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_scheduler_mode_t mode) {
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  mtapi_uint_t ii = 0;

  embb_mtapi_log_trace("embb_mtapi_scheduler_initialize() called\n");

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != node);

  embb_atomic_store_int(&that->affine_task_counter, 0);

  /* Paranoia sanitizing of scheduler mode */
  if (mode >= NUM_SCHEDULER_MODES) {
    mode = WORK_STEAL_VHPF;
  }
  that->mode = mode;

  assert(node->attributes.num_cores ==
    embb_core_set_count(&node->attributes.core_affinity));
  that->worker_count = node->attributes.num_cores;

  that->worker_contexts = (embb_mtapi_thread_context_t*)
    embb_mtapi_alloc_allocate(
      sizeof(embb_mtapi_thread_context_t)*that->worker_count);
  for (ii = 0; ii < that->worker_count; ii++) {
    unsigned int core_num = 0;
    mtapi_uint_t ll = 0;
    mtapi_boolean_t run = MTAPI_TRUE;
    while (run) {
      if (embb_core_set_contains(&node->attributes.core_affinity, core_num)) {
        if (ll == ii) break;
        ll++;
      }
      core_num++;
    }
    embb_mtapi_thread_context_initialize_with_node_worker_and_core(
      &that->worker_contexts[ii], node, ii, core_num);
  }
  for (ii = 0; ii < that->worker_count; ii++) {
    if (MTAPI_FALSE == embb_mtapi_thread_context_start(
      &that->worker_contexts[ii], that)) {
      /* on error return false, finalize will shut everything down */
      return MTAPI_FALSE;
    }
  }
  return MTAPI_TRUE;
}

void embb_mtapi_scheduler_finalize(embb_mtapi_scheduler_t * that) {
  mtapi_uint_t ii = 0;
  embb_mtapi_log_trace("embb_mtapi_scheduler_finalize() called\n");

  assert(MTAPI_NULL != that);

  /* finalize all workers */
  for (ii = 0; ii < that->worker_count; ii++) {
    embb_mtapi_thread_context_stop(&that->worker_contexts[ii]);
  }
  for (ii = 0; ii < that->worker_count; ii++) {
    embb_mtapi_thread_context_finalize(&that->worker_contexts[ii]);
  }

  that->worker_count = 0;
  embb_mtapi_alloc_deallocate(that->worker_contexts);
  that->worker_contexts = MTAPI_NULL;
}

embb_mtapi_scheduler_t * embb_mtapi_scheduler_new() {
  embb_mtapi_scheduler_t * that =
    (embb_mtapi_scheduler_t*)embb_mtapi_alloc_allocate(
      sizeof(embb_mtapi_scheduler_t));
  if (MTAPI_NULL != that) {
    if (MTAPI_FALSE == embb_mtapi_scheduler_initialize(that)) {
      /* on error delete and return MTAPI_NULL */
      embb_mtapi_scheduler_delete(that);
      return MTAPI_NULL;
    }
  }
  return that;
}

void embb_mtapi_scheduler_delete(embb_mtapi_scheduler_t * that) {
  assert(MTAPI_NULL != that);

  embb_mtapi_scheduler_finalize(that);
  embb_mtapi_alloc_deallocate(that);
}

mtapi_boolean_t embb_mtapi_scheduler_process_tasks(
  embb_mtapi_scheduler_t* that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data) {
  mtapi_uint_t ii;
  mtapi_boolean_t result = MTAPI_TRUE;

  assert(MTAPI_NULL != that);

  for (ii = 0; ii < that->worker_count; ii++) {
    result = embb_mtapi_thread_context_process_tasks(
      &that->worker_contexts[ii], process, user_data);
    if (MTAPI_FALSE == result) {
      break;
    }
  }

  return result;
}

mtapi_boolean_t embb_mtapi_scheduler_schedule_task(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_task_t * task,
  mtapi_uint_t instance) {
  embb_mtapi_scheduler_t * scheduler = that;
  /* distribute round robin */
  mtapi_uint_t ii = (task->handle.id + instance) % scheduler->worker_count;
  mtapi_boolean_t pushed = MTAPI_FALSE;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();

  assert(MTAPI_NULL != node);

  if (embb_mtapi_action_pool_is_handle_valid(
    node->action_pool, task->action)) {
    embb_mtapi_queue_t* local_queue = MTAPI_NULL;
    /* fetch action and schedule */
    embb_mtapi_action_t* local_action =
      embb_mtapi_action_pool_get_storage_for_handle(
      node->action_pool, task->action);

    mtapi_affinity_t affinity =
      local_action->attributes.affinity & task->attributes.affinity;

    /* check if task is running from an ordered queue */
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, task->queue)) {
      local_queue = embb_mtapi_queue_pool_get_storage_for_handle(
        node->queue_pool, task->queue);
      if (local_queue->attributes.ordered) {
        /* yes, modify affinity accordingly */
        affinity = local_queue->ordered_affinity;
      }
    }

    /* check affinity */
    if (affinity == 0) {
      affinity = node->affinity_all;
    }

    /* one more task in flight for this action */
    embb_atomic_fetch_and_add_int(&local_action->num_tasks, 1);

    if (affinity == node->affinity_all) {
      /* no affinity restrictions, schedule for stealing */
      pushed = embb_mtapi_task_queue_push(
        scheduler->worker_contexts[ii].queue[task->attributes.priority],
        task);
    } else {
      mtapi_status_t affinity_status;

      /* affinity is restricted, check and adapt scheduling target */
      ii = (mtapi_uint_t)embb_atomic_fetch_and_add_int(
        &scheduler->affine_task_counter, 1);
      while (MTAPI_FALSE == mtapi_affinity_get(
        &affinity, ii, &affinity_status)) {
        ii = (ii + 1) % scheduler->worker_count;
      }
      /* schedule into private queue to disable stealing */
      pushed = embb_mtapi_task_queue_push(
        scheduler->worker_contexts[ii].private_queue[task->attributes.priority],
        task);
    }

    if (pushed) {
      /* signal the worker thread a task was pushed to */
      if (embb_atomic_load_int(&scheduler->worker_contexts[ii].is_sleeping)) {
        embb_condition_notify_one(
          &scheduler->worker_contexts[ii].work_available);
      }
    } else {
      /* task could not be launched */
      embb_atomic_fetch_and_add_int(&local_action->num_tasks, -1);
    }
  }

  return pushed;
}
