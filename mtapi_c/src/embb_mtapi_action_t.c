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
#include <embb/base/c/duration.h>
#include <embb/base/c/time.h>

#include <embb_mtapi_job_t.h>
#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_pool_template-inl.h>
#include <embb_mtapi_attr.h>
#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_task_t.h>


/* ---- POOL STORAGE FUNCTIONS --------------------------------------------- */

embb_mtapi_pool_implementation(action)


/* ---- CLASS MEMBERS ------------------------------------------------------ */

void embb_mtapi_action_initialize(embb_mtapi_action_t* that) {
  assert(MTAPI_NULL != that);

  that->action_function = NULL;
  that->job_id = MTAPI_JOB_ID_INVALID;
  that->domain_id = MTAPI_DOMAIN_ID_INVALID;
  that->node_id = MTAPI_NODE_ID_INVALID;
  that->enabled = MTAPI_FALSE;
  that->node_local_data = NULL;
  that->node_local_data_size = 0;
  that->plugin_data = MTAPI_NULL;
  embb_atomic_init_int(&that->num_tasks, 0);
}

void embb_mtapi_action_finalize(embb_mtapi_action_t* that) {
  assert(MTAPI_NULL != that);

  if (that->is_plugin_action) {
    // TODO(mw): check status
    that->plugin_action_finalize_function(that->handle, NULL);
  }
  that->action_function = NULL;
  that->job_id = MTAPI_JOB_ID_INVALID;
  that->domain_id = MTAPI_DOMAIN_ID_INVALID;
  that->node_id = MTAPI_NODE_ID_INVALID;
  that->enabled = MTAPI_FALSE;
  that->node_local_data = NULL;
  that->node_local_data_size = 0;
  that->plugin_data = MTAPI_NULL;
  embb_atomic_destroy_int(&that->num_tasks);
}

static mtapi_boolean_t embb_mtapi_action_delete_visitor(
  embb_mtapi_task_t * task,
  void * user_data) {
  embb_mtapi_action_t * action = (embb_mtapi_action_t*)user_data;

  assert(MTAPI_NULL != action);
  assert(MTAPI_NULL != task);

  if (
    task->action.id == action->handle.id &&
    task->action.tag == action->handle.tag) {
    /* task is scheduled and needs to be cancelled */
    embb_mtapi_task_set_state(task, MTAPI_TASK_CANCELLED);
    task->error_code = MTAPI_ERR_ACTION_DELETED;
  }

  /* do not remove task from queue */
  return MTAPI_TRUE;
}

static mtapi_boolean_t embb_mtapi_action_disable_visitor(
  embb_mtapi_task_t * task,
  void * user_data) {
  embb_mtapi_action_t * action = (embb_mtapi_action_t*)user_data;

  assert(MTAPI_NULL != action);
  assert(MTAPI_NULL != task);

  if (
    task->action.id == action->handle.id &&
    task->action.tag == action->handle.tag) {
    /* task is scheduled and needs to be cancelled */
    embb_mtapi_task_set_state(task, MTAPI_TASK_CANCELLED);
    task->error_code = MTAPI_ERR_ACTION_DISABLED;
  }

  /* do not remove task from queue */
  return MTAPI_TRUE;
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

mtapi_action_hndl_t mtapi_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN mtapi_action_function_t action_function,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_IN mtapi_action_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_action_hndl_t action_handle = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };

  embb_mtapi_log_trace("mtapi_action_create() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    /* check if job is valid */
    if (embb_mtapi_job_is_id_valid(node, job_id)) {
      embb_mtapi_job_t* job = embb_mtapi_job_get_storage_for_id(node, job_id);
      embb_mtapi_action_t* new_action =
        embb_mtapi_action_pool_allocate(node->action_pool);
      if (MTAPI_NULL != new_action) {
        new_action->domain_id = node->domain_id;
        new_action->node_id = node->node_id;
        new_action->job_id = job_id;
        new_action->node_local_data = node_local_data;
        new_action->node_local_data_size = node_local_data_size;
        new_action->enabled = MTAPI_TRUE;
        new_action->is_plugin_action = MTAPI_FALSE;
        embb_atomic_init_int(&new_action->num_tasks, 0);

        new_action->action_function = action_function;

        /* set defaults if no attributes were given */
        if (MTAPI_NULL != attributes) {
          new_action->attributes = *attributes;
          local_status = MTAPI_SUCCESS;
        } else {
          /* use the default */
          mtapi_actionattr_init(&new_action->attributes, &local_status);
        }

        /* check if affinity is sane */
        if (0 == new_action->attributes.affinity) {
          local_status = MTAPI_ERR_PARAMETER;
        }

        if (MTAPI_SUCCESS == local_status) {
          action_handle = new_action->handle;
          embb_mtapi_job_add_action(job, new_action);
        } else {
          embb_mtapi_action_finalize(new_action);
          embb_mtapi_action_pool_deallocate(node->action_pool, new_action);
        }
      } else {
        /* no more space left in action pool */
        local_status = MTAPI_ERR_ACTION_LIMIT;
      }
    } else {
      local_status = MTAPI_ERR_JOB_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return action_handle;
}


void mtapi_action_set_attribute(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_action_set_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t* local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, action);
      mtapi_actionattr_set(
        &local_action->attributes,
        attribute_num,
        attribute,
        attribute_size,
        &local_status);
    } else {
      local_status = MTAPI_ERR_ACTION_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}


void mtapi_action_get_attribute(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_OUT void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_action_get_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t* local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, action);

      if (MTAPI_NULL == attribute) {
        local_status = MTAPI_ERR_PARAMETER;
      } else {
        switch (attribute_num) {
        case MTAPI_ACTION_GLOBAL:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_action->attributes.global, attribute, attribute_size);
          break;

        case MTAPI_ACTION_AFFINITY:
          local_status = embb_mtapi_attr_get_mtapi_affinity_t(
            &local_action->attributes.affinity, attribute, attribute_size);
          break;

        case MTAPI_ACTION_DOMAIN_SHARED:
          local_status = embb_mtapi_attr_get_mtapi_boolean_t(
            &local_action->attributes.domain_shared,
            attribute,
            attribute_size);
          break;

        default:
          /* attribute unknown */
          local_status = MTAPI_ERR_ATTR_NUM;
          break;
        }
      }
    } else {
      local_status = MTAPI_ERR_ACTION_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}


void mtapi_action_delete(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_action_delete() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t* local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, action);

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

      /* cancel all tasks */
      embb_mtapi_scheduler_process_tasks(
        node->scheduler, embb_mtapi_action_delete_visitor, local_action);

      /* find out on which thread we are */
      context = embb_mtapi_scheduler_get_current_thread_context(
        node->scheduler);

      local_status = MTAPI_SUCCESS;
      while (embb_atomic_load_int(&local_action->num_tasks)) {
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
        /* delete action */
        if (embb_mtapi_job_is_id_valid(node, local_action->job_id)) {
          embb_mtapi_job_t* local_job = embb_mtapi_job_get_storage_for_id(
            node, local_action->job_id);
          embb_mtapi_job_remove_action(local_job, local_action);
        }
        embb_mtapi_action_finalize(local_action);
        embb_mtapi_action_pool_deallocate(node->action_pool, local_action);
      }
    } else {
      local_status = MTAPI_ERR_ACTION_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_action_disable(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_action_disable() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t* local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, action);
      local_action->enabled = MTAPI_FALSE;

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

      /* cancel all tasks */
      embb_mtapi_scheduler_process_tasks(
        node->scheduler, embb_mtapi_action_disable_visitor, local_action);

      /* find out on which thread we are */
      context = embb_mtapi_scheduler_get_current_thread_context(
        node->scheduler);

      local_status = MTAPI_SUCCESS;
      while (embb_atomic_load_int(&local_action->num_tasks)) {
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
      local_status = MTAPI_ERR_ACTION_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_action_enable(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_action_enable() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t* local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, action);
      local_action->enabled = MTAPI_TRUE;
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_ACTION_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
