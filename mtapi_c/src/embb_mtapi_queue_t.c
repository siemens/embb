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

#include <embb/base/c/internal/unused.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_queue_t.h>
#include <embb_mtapi_group_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_job_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_pool_template-inl.h>
#include <embb_mtapi_task_queue_t.h>
#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_thread_context_t.h>
#include <embb_mtapi_attr.h>


/* ---- POOL STORAGE FUNCTIONS --------------------------------------------- */

embb_mtapi_pool_implementation(queue)


/* ---- CLASS MEMBERS ------------------------------------------------------ */

void embb_mtapi_queue_initialize(embb_mtapi_queue_t* that) {
  assert(MTAPI_NULL != that);

  mtapi_queueattr_init(&that->attributes, MTAPI_NULL);
  that->queue_id = MTAPI_QUEUE_ID_NONE;
  embb_atomic_init_int(&that->ordered_task_executing, 0);
  embb_atomic_init_char(&that->enabled, MTAPI_FALSE);
  embb_atomic_init_int(&that->num_tasks, 0);
  that->job_handle.id = 0;
  that->job_handle.tag = 0;
  embb_mtapi_task_queue_initialize(&that->retained_tasks);
  embb_mtapi_task_queue_initialize(&that->ordered_tasks);
}

void embb_mtapi_queue_initialize_with_attributes_and_job(
  embb_mtapi_queue_t* that,
  mtapi_queue_attributes_t* attributes,
  mtapi_job_hndl_t job) {
  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != attributes);

  that->attributes = *attributes;
  that->queue_id = MTAPI_QUEUE_ID_NONE;
  embb_atomic_init_int(&that->ordered_task_executing, 0);
  embb_atomic_init_char(&that->enabled, MTAPI_TRUE);
  embb_atomic_init_int(&that->num_tasks, 0);
  that->job_handle = job;
  embb_mtapi_task_queue_initialize(&that->retained_tasks);
  embb_mtapi_task_queue_initialize(&that->ordered_tasks);
}

void embb_mtapi_queue_finalize(embb_mtapi_queue_t* that) {
  assert(MTAPI_NULL != that);

  embb_mtapi_task_queue_finalize(&that->ordered_tasks);
  embb_mtapi_task_queue_finalize(&that->retained_tasks);
  that->job_handle.id = 0;
  that->job_handle.tag = 0;
  embb_atomic_destroy_int(&that->num_tasks);
  embb_atomic_destroy_char(&that->enabled);
  embb_atomic_destroy_int(&that->ordered_task_executing);
  that->queue_id = MTAPI_QUEUE_ID_NONE;
}

void embb_mtapi_queue_task_started(embb_mtapi_queue_t* that) {
  assert(MTAPI_NULL != that);
  embb_atomic_fetch_and_add_int(&that->num_tasks, 1);
}

void embb_mtapi_queue_task_finished(embb_mtapi_queue_t* that) {
  assert(MTAPI_NULL != that);
  embb_atomic_fetch_and_add_int(&that->num_tasks, -1);
}

int embb_mtapi_queue_ordered_task_start(
  embb_mtapi_queue_t* that) {
  int expected = 0;
  return embb_atomic_compare_and_swap_int(
    &that->ordered_task_executing, &expected, 1);
}

void embb_mtapi_queue_ordered_task_finish(
  embb_mtapi_queue_t* that) {
  embb_atomic_store_int(&that->ordered_task_executing, 0);
}

static mtapi_boolean_t embb_mtapi_queue_delete_visitor(
  embb_mtapi_task_t * task,
  void * user_data) {
  embb_mtapi_queue_t * queue = (embb_mtapi_queue_t*)user_data;

  assert(MTAPI_NULL != queue);
  assert(MTAPI_NULL != task);

  if (task->queue.id == queue->handle.id &&
      task->queue.tag == queue->handle.tag) {
    /* task is scheduled and needs to be cancelled */
    embb_mtapi_task_set_state(task, MTAPI_TASK_CANCELLED);
    task->error_code = MTAPI_ERR_QUEUE_DELETED;
  }

  /* do not remove task from queue */
  return MTAPI_TRUE;
}

static mtapi_boolean_t embb_mtapi_queue_disable_visitor(
  embb_mtapi_task_t * task,
  void * user_data) {
  embb_mtapi_queue_t * queue = (embb_mtapi_queue_t*)user_data;
  /* do not remove task from queue by default */
  mtapi_boolean_t result = MTAPI_TRUE;

  assert(MTAPI_NULL != queue);
  assert(MTAPI_NULL != task);

  if (task->queue.id == queue->handle.id &&
      task->queue.tag == queue->handle.tag) {
    if (queue->attributes.retain) {
      /* task is scheduled and needs to be retained */
      embb_mtapi_task_set_state(task, MTAPI_TASK_RETAINED);
      embb_mtapi_task_queue_push_back(&queue->retained_tasks, task);
      /* remove task from queue */
      result = MTAPI_FALSE;
    } else {
      /* task is scheduled and needs to be cancelled */
      embb_mtapi_task_set_state(task, MTAPI_TASK_CANCELLED);
      task->error_code = MTAPI_ERR_QUEUE_DISABLED;
    }
  }

  return result;
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

mtapi_queue_hndl_t mtapi_queue_create(
  MTAPI_IN mtapi_queue_id_t queue_id,
  MTAPI_IN mtapi_job_hndl_t job,
  MTAPI_IN mtapi_queue_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  embb_mtapi_queue_t* queue = MTAPI_NULL;
  mtapi_queue_hndl_t queue_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };
  mtapi_queue_attributes_t attr;

  embb_mtapi_log_trace("mtapi_queue_create() called\n");

  if (embb_mtapi_node_is_initialized()) {
    queue = embb_mtapi_queue_pool_allocate(node->queue_pool);
    if (MTAPI_NULL != queue) {
      if (MTAPI_NULL != attributes) {
        attr = *attributes;
        local_status = MTAPI_SUCCESS;
      } else {
        mtapi_queueattr_init(&attr, &local_status);
      }
      if (MTAPI_SUCCESS == local_status) {
        if (embb_mtapi_job_is_handle_valid(node, job)) {
          embb_mtapi_queue_initialize_with_attributes_and_job(
            queue, &attr, job);
          /* for an ordered queue, initialize affinity */
          if (queue->attributes.ordered) {
            mtapi_affinity_init(
              &queue->ordered_affinity,
              MTAPI_FALSE, MTAPI_NULL);
            mtapi_affinity_set(
              &queue->ordered_affinity,
              queue->handle.id % node->scheduler->worker_count,
              MTAPI_TRUE, MTAPI_NULL);
          }
          queue->queue_id = queue_id;
          queue_hndl = queue->handle;
        } else {
          embb_mtapi_queue_pool_deallocate(node->queue_pool, queue);
          local_status = MTAPI_ERR_JOB_INVALID;
        }
      } else {
        embb_mtapi_queue_pool_deallocate(node->queue_pool, queue);
      }
    } else {
      local_status = MTAPI_ERR_QUEUE_LIMIT;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return queue_hndl;
}

void mtapi_queue_set_attribute(
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  embb_mtapi_queue_t* local_queue;

  embb_mtapi_log_trace("mtapi_queue_set_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
      local_queue = embb_mtapi_queue_pool_get_storage_for_handle(
        node->queue_pool, queue);
      mtapi_queueattr_set(&local_queue->attributes, attribute_num,
        attribute, attribute_size, &local_status);
    } else {
      local_status = MTAPI_ERR_QUEUE_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_queue_get_attribute(
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_OUT void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  embb_mtapi_queue_t* local_queue;

  embb_mtapi_log_trace("mtapi_queue_get_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
      local_queue = embb_mtapi_queue_pool_get_storage_for_handle(
        node->queue_pool, queue);

      if (MTAPI_NULL == attribute) {
        local_status = MTAPI_ERR_PARAMETER;
      } else {
        switch (attribute_num) {
        case MTAPI_QUEUE_GLOBAL:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_queue->attributes.global, attribute, attribute_size);
          break;

        case MTAPI_QUEUE_PRIORITY:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_queue->attributes.priority, attribute, attribute_size);
          break;

        case MTAPI_QUEUE_LIMIT:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_queue->attributes.limit, attribute, attribute_size);
          break;

        case MTAPI_QUEUE_ORDERED:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_queue->attributes.ordered, attribute, attribute_size);
          break;

        case MTAPI_QUEUE_RETAIN:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_queue->attributes.retain, attribute, attribute_size);
          break;

        case MTAPI_QUEUE_DOMAIN_SHARED:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_queue->attributes.domain_shared, attribute, attribute_size);
          break;

        default:
          local_status = MTAPI_ERR_ATTR_NUM;
          break;
        }
      }
    } else {
      local_status = MTAPI_ERR_QUEUE_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

mtapi_queue_hndl_t mtapi_queue_get(
  MTAPI_IN mtapi_queue_id_t queue_id,
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_queue_hndl_t queue_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(domain_id);

  embb_mtapi_log_trace("mtapi_queue_get() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    mtapi_uint_t ii;

    local_status = MTAPI_ERR_QUEUE_INVALID;
    for (ii = 0; ii < node->attributes.max_queues; ii++) {
      if (queue_id == node->queue_pool->storage[ii].queue_id) {
        queue_hndl = node->queue_pool->storage[ii].handle;
        local_status = MTAPI_SUCCESS;
        break;
      }
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return queue_hndl;
}

void mtapi_queue_delete(
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_queue_delete() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
      embb_mtapi_queue_t* local_queue =
        embb_mtapi_queue_pool_get_storage_for_handle(
          node->queue_pool, queue);
      embb_mtapi_thread_context_t * context = NULL;

      embb_duration_t wait_duration;
      embb_time_t start_time;
      embb_time_t end_time;
      if (MTAPI_INFINITE < timeout) {
        embb_duration_set_milliseconds(
          &wait_duration, (unsigned long long)timeout);
        embb_time_now(&start_time);
        embb_time_in(&end_time, &wait_duration);
      }

      /* find out on which thread we are */
      context = embb_mtapi_scheduler_get_current_thread_context(
        node->scheduler);

      if (local_queue->attributes.retain) {
        /* reschedule retained tasks */
        embb_mtapi_task_t * task =
          embb_mtapi_task_queue_pop_front(&local_queue->retained_tasks);
        while (MTAPI_NULL != task) {
          embb_mtapi_task_set_state(task, MTAPI_TASK_SCHEDULED);
          embb_mtapi_scheduler_schedule_task(node->scheduler, task);
          task = embb_mtapi_task_queue_pop_front(&local_queue->retained_tasks);
        }
      }

      /* cancel all tasks */
      embb_mtapi_scheduler_process_tasks(
        node->scheduler, embb_mtapi_queue_delete_visitor, local_queue);

      /* wait for tasks in queue to finish */
      local_status = MTAPI_SUCCESS;
      while (0 != embb_atomic_load_int(&local_queue->num_tasks)) {
        if (MTAPI_INFINITE < timeout) {
          embb_time_t current_time;
          embb_time_now(&current_time);
          if (embb_time_compare(&current_time, &start_time) < 0) {
            /* time has moved backwards, maybe a wraparound or jitter
               move end_time backward to avoid endeless loop */
            start_time = current_time;
            embb_time_in(&end_time, &wait_duration);
          }
          if (embb_time_compare(&current_time, &end_time) > 0) {
            /* timeout! */
            local_status = MTAPI_TIMEOUT;
            break;
          }
        }

        /* do other work if applicable */
        embb_mtapi_scheduler_execute_task_or_yield(
          node->scheduler,
          node,
          context);
      }

      if (MTAPI_SUCCESS == local_status) {
        /* delete queue */
        embb_mtapi_queue_finalize(local_queue);
        embb_mtapi_queue_pool_deallocate(node->queue_pool, local_queue);
      }
    } else {
      local_status = MTAPI_ERR_QUEUE_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_queue_disable(
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_queue_disable() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
      embb_mtapi_queue_t* local_queue =
        embb_mtapi_queue_pool_get_storage_for_handle(
          node->queue_pool, queue);
      embb_atomic_store_char(&local_queue->enabled, MTAPI_FALSE);

      /* cancel or retain all tasks scheduled via queue */
      embb_mtapi_scheduler_process_tasks(
        node->scheduler, embb_mtapi_queue_disable_visitor, local_queue);

      /* cancel all tasks held back the ordered queue if it is not retaining */
      if (local_queue->attributes.ordered && !local_queue->attributes.retain) {
        embb_mtapi_action_t * local_action = MTAPI_NULL;
        embb_mtapi_task_t * task =
          embb_mtapi_task_queue_pop_front(&local_queue->ordered_tasks);
        while (MTAPI_NULL != task) {
          /* get action for task */
          if (embb_mtapi_action_pool_is_handle_valid(
            node->action_pool, task->action)) {
            local_action =
              embb_mtapi_action_pool_get_storage_for_handle(
                node->action_pool, task->action);
          }
          /* set state to cancelled */
          task->error_code = MTAPI_ERR_ACTION_CANCELLED;
          embb_mtapi_scheduler_finalize_task(task, node, MTAPI_TASK_CANCELLED);
          /* remove task from action */
          if (MTAPI_NULL != local_action) {
            embb_atomic_fetch_and_add_int(&local_action->num_tasks, -1);
          }
          /* get next task */
          task = embb_mtapi_task_queue_pop_front(&local_queue->ordered_tasks);
        }
      }

      /* if queue is not retaining, wait for all tasks to finish */
      if (MTAPI_FALSE == local_queue->attributes.retain) {
        /* find out on which thread we are */
        embb_mtapi_thread_context_t * context =
          embb_mtapi_scheduler_get_current_thread_context(node->scheduler);

        embb_duration_t wait_duration;
        embb_time_t start_time;
        embb_time_t end_time;
        if (MTAPI_INFINITE < timeout) {
          embb_duration_set_milliseconds(
            &wait_duration, (unsigned long long)timeout);
          embb_time_now(&start_time);
          embb_time_in(&end_time, &wait_duration);
        }

        /* wait for tasks in queue to finish */
        local_status = MTAPI_SUCCESS;
        while (0 != embb_atomic_load_int(&local_queue->num_tasks)) {
          if (MTAPI_INFINITE < timeout) {
            embb_time_t current_time;
            embb_time_now(&current_time);
            if (embb_time_compare(&current_time, &start_time) < 0) {
              /* time has moved backwards, maybe a wraparound or jitter
                 move end_time backward to avoid endeless loop */
              start_time = current_time;
              embb_time_in(&end_time, &wait_duration);
            }
            if (embb_time_compare(&current_time, &end_time) > 0) {
              /* timeout! */
              local_status = MTAPI_TIMEOUT;
              break;
            }
          }

          /* do other work if applicable */
          embb_mtapi_scheduler_execute_task_or_yield(
            node->scheduler,
            node,
            context);
        }
      } else {
        local_status = MTAPI_SUCCESS;
      }
    } else {
      local_status = MTAPI_ERR_QUEUE_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_queue_enable(
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_queue_enable() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
      embb_mtapi_queue_t* local_queue =
        embb_mtapi_queue_pool_get_storage_for_handle(
          node->queue_pool, queue);
      embb_atomic_store_char(&local_queue->enabled, MTAPI_TRUE);
      local_status = MTAPI_SUCCESS;
      if (local_queue->attributes.retain) {
        /* reschedule retained tasks */
        embb_mtapi_task_t * task =
          embb_mtapi_task_queue_pop_front(&local_queue->retained_tasks);
        while (MTAPI_NULL != task) {
          embb_mtapi_task_set_state(task, MTAPI_TASK_SCHEDULED);
          embb_mtapi_scheduler_schedule_task(node->scheduler, task);
          task = embb_mtapi_task_queue_pop_front(&local_queue->retained_tasks);
        }
      }
    } else {
      local_status = MTAPI_ERR_QUEUE_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
