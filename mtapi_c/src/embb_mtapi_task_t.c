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
#include <mtapi_status_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_job_t.h>
#include <embb_mtapi_queue_t.h>
#include <embb_mtapi_task_queue_t.h>
#include <embb_mtapi_thread_context_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_group_t.h>
#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_attr.h>
#include <embb_mtapi_task_context_t.h>


/* ---- POOL STORAGE FUNCTIONS --------------------------------------------- */

#include <embb_mtapi_pool_template-inl.h>
embb_mtapi_pool_implementation(task)


/* ---- CLASS MEMBERS ------------------------------------------------------ */

embb_mtapi_task_t* embb_mtapi_task_new(embb_mtapi_task_pool_t* pool) {
  embb_mtapi_task_t* that;

  assert(MTAPI_NULL != pool);

  that = embb_mtapi_task_pool_allocate(pool);
  if (MTAPI_NULL != that) {
    embb_mtapi_task_initialize(that);
  }

  return that;
}

void embb_mtapi_task_delete(
  embb_mtapi_task_t* that,
  embb_mtapi_task_pool_t* pool) {
  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != pool);

  embb_mtapi_task_finalize(that);
  embb_mtapi_task_pool_deallocate(pool, that);
}

void embb_mtapi_task_initialize(embb_mtapi_task_t* that) {
  assert(MTAPI_NULL != that);

  that->action.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  that->job.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  embb_atomic_init_int(&that->state, MTAPI_TASK_ERROR);
  that->task_id = MTAPI_TASK_ID_NONE;
  that->group.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  that->queue.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  that->error_code = MTAPI_SUCCESS;
  that->next = MTAPI_NULL;
  embb_atomic_init_unsigned_int(&that->current_instance, 0);
  embb_atomic_init_unsigned_int(&that->instances_todo, 0);
}

void embb_mtapi_task_finalize(embb_mtapi_task_t* that) {
  assert(MTAPI_NULL != that);

  that->action.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  that->job.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  embb_atomic_destroy_int(&that->state);
  that->task_id = MTAPI_TASK_ID_NONE;
  that->group.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  that->queue.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
  that->error_code = MTAPI_SUCCESS;
  that->next = MTAPI_NULL;
  embb_atomic_destroy_unsigned_int(&that->current_instance);
  embb_atomic_destroy_unsigned_int(&that->instances_todo);
}

mtapi_boolean_t embb_mtapi_task_execute(
  embb_mtapi_task_t* that,
  embb_mtapi_task_context_t * context,
  mtapi_task_state_t * new_task_state) {
  unsigned int todo = that->attributes.num_instances;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != context);

  embb_mtapi_task_set_state(that, MTAPI_TASK_RUNNING);

  /* is the associated action valid? */
  if (embb_mtapi_action_pool_is_handle_valid(
    context->thread_context->node->action_pool, that->action)) {
    /* fetch action and execute */
    embb_mtapi_action_t* local_action =
      embb_mtapi_action_pool_get_storage_for_handle(
      context->thread_context->node->action_pool, that->action);

    /* only continue if there was no error so far */
    if (that->error_code == MTAPI_SUCCESS) {
      local_action->action_function(
        that->arguments,
        that->arguments_size,
        that->result_buffer,
        that->result_size,
        local_action->node_local_data,
        local_action->node_local_data_size,
        context);
    }
    embb_atomic_memory_barrier();
    todo = embb_atomic_fetch_and_add_unsigned_int(
      &that->instances_todo, (unsigned int)-1);

    if (todo == 1) {
      /* task has completed */
      if (MTAPI_SUCCESS == that->error_code) {
        *new_task_state = MTAPI_TASK_COMPLETED;
      } else if (MTAPI_ERR_ACTION_CANCELLED == that->error_code) {
        *new_task_state = MTAPI_TASK_CANCELLED;
      } else {
        *new_task_state = MTAPI_TASK_ERROR;
      }
    }
  } else {
    /* action was deleted, task did not complete */
    that->error_code = MTAPI_ERR_ACTION_DELETED;
    *new_task_state = MTAPI_TASK_ERROR;
    todo = 1;
  }

  if (todo == 1) {
    return MTAPI_TRUE;
  } else {
    return MTAPI_FALSE;
  }
}

void embb_mtapi_task_set_state(
  embb_mtapi_task_t* that,
  mtapi_task_state_t state) {
  assert(MTAPI_NULL != that);

  embb_atomic_store_int(&that->state, state);
}

static mtapi_task_hndl_t embb_mtapi_task_start(
  MTAPI_IN mtapi_task_id_t task_id,
  MTAPI_IN mtapi_job_hndl_t job,
  MTAPI_IN void* arguments,
  MTAPI_IN mtapi_size_t arguments_size,
  MTAPI_OUT void* result_buffer,
  MTAPI_IN mtapi_size_t result_size,
  MTAPI_IN mtapi_task_attributes_t* attributes,
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_task_hndl_t task_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };

  embb_mtapi_log_trace("embb_mtapi_task_start() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_job_is_handle_valid(node, job)) {
      embb_mtapi_job_t* local_job =
        embb_mtapi_job_get_storage_for_id(node, job.id);
      embb_mtapi_task_t* task = embb_mtapi_task_pool_allocate(node->task_pool);
      if (MTAPI_NULL != task) {
        mtapi_uint_t action_index;

        embb_mtapi_task_initialize(task);
        embb_mtapi_task_set_state(task, MTAPI_TASK_PRENATAL);
        task->task_id = task_id;
        task->job = job;
        task->arguments = arguments;
        task->arguments_size = arguments_size;
        task->result_buffer = result_buffer;
        task->result_size = result_size;

        if (MTAPI_NULL != attributes) {
          task->attributes = *attributes;
        } else {
          mtapi_taskattr_init(&task->attributes, &local_status);
        }

        embb_atomic_store_unsigned_int(
          &task->instances_todo, task->attributes.num_instances);

        if (embb_mtapi_group_pool_is_handle_valid(node->group_pool, group)) {
          embb_mtapi_group_t* local_group =
            embb_mtapi_group_pool_get_storage_for_handle(
            node->group_pool, group);
          task->group = group;
          embb_atomic_fetch_and_add_int(&local_group->num_tasks, 1);
        } else {
          task->group.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
        }

        if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
          embb_mtapi_queue_t* local_queue =
            embb_mtapi_queue_pool_get_storage_for_handle(
            node->queue_pool, queue);
          task->queue = queue;
          embb_mtapi_queue_task_started(local_queue);
        } else {
          task->queue.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
        }

        /* load balancing: choose action with minimum tasks */
        action_index = 0;
        for (mtapi_uint_t ii = 0; ii < local_job->num_actions; ii++) {
          if (embb_mtapi_action_pool_is_handle_valid(
            node->action_pool, local_job->actions[ii])) {
            embb_mtapi_action_t * act_m =
              embb_mtapi_action_pool_get_storage_for_handle(
              node->action_pool, local_job->actions[action_index]);
            embb_mtapi_action_t * act_i =
              embb_mtapi_action_pool_get_storage_for_handle(
              node->action_pool, local_job->actions[ii]);
            if (embb_atomic_load_int(&act_m->num_tasks) >
              embb_atomic_load_int(&act_i->num_tasks)) {
              action_index = ii;
            }
          }
        }
        if (embb_mtapi_action_pool_is_handle_valid(
          node->action_pool, local_job->actions[action_index])) {
          task->action = local_job->actions[action_index];
          embb_mtapi_task_set_state(task, MTAPI_TASK_CREATED);
          task_hndl = task->handle;
          local_status = MTAPI_SUCCESS;
        } else {
          local_status = MTAPI_ERR_ACTION_INVALID;
        }

        /* check priority for validity */
        if (node->attributes.max_priorities <= task->attributes.priority) {
          local_status = MTAPI_ERR_PARAMETER;
        }

        if (MTAPI_SUCCESS == local_status) {
          embb_mtapi_scheduler_t * scheduler = node->scheduler;
          mtapi_boolean_t was_scheduled;
          embb_mtapi_action_t * local_action =
            embb_mtapi_action_pool_get_storage_for_handle(
              node->action_pool, task->action);
          int num_instances = (int)task->attributes.num_instances;

          /* num_instances more tasks in flight for action */
          embb_atomic_fetch_and_add_int(
            &local_action->num_tasks, num_instances);

          embb_mtapi_task_set_state(task, MTAPI_TASK_SCHEDULED);

          if (local_action->is_plugin_action) {
            /* schedule plugin task */
            mtapi_status_t plugin_status = MTAPI_ERR_UNKNOWN;
            local_action->plugin_task_start_function(
              task_hndl, &plugin_status);
            was_scheduled = (MTAPI_SUCCESS == plugin_status) ?
              MTAPI_TRUE : MTAPI_FALSE;
          } else {
            /* schedule local task */
            was_scheduled =
              embb_mtapi_scheduler_schedule_task(scheduler, task);
          }

          if (was_scheduled) {
            /* if task is detached, do not return a handle, it will be deleted
               on completion */
            if (task->attributes.is_detached) {
              task_hndl.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
            }

            local_status = MTAPI_SUCCESS;
          } else {
            /* task could not be pushed */
            local_status = MTAPI_ERR_TASK_LIMIT;
            embb_mtapi_task_set_state(task, MTAPI_TASK_ERROR);
            /* num_instances tasks not in flight for action */
            embb_atomic_fetch_and_add_int(
              &local_action->num_tasks, -num_instances);
          }
        }

        if (MTAPI_SUCCESS != local_status) {
          embb_mtapi_task_delete(task, node->task_pool);
          task_hndl.id = EMBB_MTAPI_IDPOOL_INVALID_ID;
        }
      } else {
        local_status = MTAPI_ERR_TASK_LIMIT;
      }
    } else {
      local_status = MTAPI_ERR_JOB_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return task_hndl;
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

mtapi_task_hndl_t mtapi_task_start(
  MTAPI_IN mtapi_task_id_t task_id,
  MTAPI_IN mtapi_job_hndl_t job,
  MTAPI_IN void* arguments,
  MTAPI_IN mtapi_size_t arguments_size,
  MTAPI_OUT void* result_buffer, /* pointer to result buffer */
  MTAPI_IN mtapi_size_t result_size,   /* size of one result */
  MTAPI_IN mtapi_task_attributes_t* attributes,
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_queue_hndl_t queue_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };

  embb_mtapi_log_trace("mtapi_task_start() called\n");

  return embb_mtapi_task_start(
    task_id,
    job,
    arguments,
    arguments_size,
    result_buffer,
    result_size,
    attributes,
    group,
    queue_hndl,
    status);
}

mtapi_task_hndl_t mtapi_task_enqueue(
  MTAPI_IN mtapi_task_id_t task_id,
  MTAPI_IN mtapi_queue_hndl_t queue,
  MTAPI_IN void* arguments,
  MTAPI_IN mtapi_size_t arguments_size,
  MTAPI_OUT void* result_buffer, /* pointer to result buffer */
  MTAPI_IN mtapi_size_t result_size,   /* size of one result */
  MTAPI_IN mtapi_task_attributes_t* attributes,
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_task_hndl_t task_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };

  embb_mtapi_log_trace("mtapi_task_enqueue() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_queue_pool_is_handle_valid(node->queue_pool, queue)) {
      embb_mtapi_queue_t* local_queue =
        embb_mtapi_queue_pool_get_storage_for_handle(node->queue_pool, queue);
      if ((MTAPI_TRUE == embb_atomic_load_char(&local_queue->enabled)) ||
        local_queue->attributes.retain) {
        mtapi_task_attributes_t local_attributes;
        if (MTAPI_NULL != attributes) {
          local_attributes = *attributes;
        } else {
          mtapi_taskattr_init(&local_attributes, MTAPI_NULL);
        }
        local_attributes.priority = local_queue->attributes.priority;

        task_hndl = embb_mtapi_task_start(
          task_id,
          local_queue->job_handle,
          arguments,
          arguments_size,
          result_buffer,
          result_size,
          &local_attributes,
          group,
          queue,
          &local_status);
      } else {
        local_status = MTAPI_ERR_QUEUE_DISABLED;
      }
    } else {
      local_status = MTAPI_ERR_QUEUE_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return task_hndl;
}

void mtapi_task_get_attribute(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_OUT void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_task_get_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t* local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      if (MTAPI_NULL == attribute) {
        local_status = MTAPI_ERR_PARAMETER;
      } else {
        switch (attribute_num) {
        case MTAPI_TASK_DETACHED:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_task->attributes.is_detached, attribute, attribute_size);
          break;

        case MTAPI_TASK_INSTANCES:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_task->attributes.num_instances, attribute, attribute_size);
          break;

        case MTAPI_TASK_PRIORITY:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_task->attributes.priority, attribute, attribute_size);
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

void mtapi_task_wait(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_task_wait() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t* local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);
      if (embb_mtapi_scheduler_wait_for_task(local_task, timeout)) {
        local_status = local_task->error_code;

        /* delete task if it is not in a group, otherwise the group will take
        care of deletion */
        if (MTAPI_FALSE == embb_mtapi_group_pool_is_handle_valid(
          node->group_pool, local_task->group)) {
          embb_mtapi_task_delete(local_task, node->task_pool);
        }
      } else {
        local_status = MTAPI_TIMEOUT;
      }
    } else {
      local_status = MTAPI_ERR_TASK_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_task_cancel() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t* local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      /* call plugin action cancel function */
      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        embb_mtapi_action_t* local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, local_task->action);
        if (local_action->is_plugin_action) {
          local_action->plugin_task_cancel_function(task, &local_status);
        } else {
          local_task->error_code = MTAPI_ERR_ACTION_CANCELLED;
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_CANCELLED);
          local_status = MTAPI_SUCCESS;
        }
      } else {
        local_task->error_code = MTAPI_ERR_ACTION_CANCELLED;
        embb_mtapi_task_set_state(local_task, MTAPI_TASK_CANCELLED);
        local_status = MTAPI_SUCCESS;
      }
    } else {
      local_status = MTAPI_ERR_TASK_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
