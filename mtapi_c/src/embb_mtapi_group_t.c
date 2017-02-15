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

#include <embb/base/c/base.h>
#include <embb/mtapi/c/mtapi.h>

#include <embb/base/c/internal/unused.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_group_t.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_job_t.h>
#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_thread_context_t.h>
#include <embb_mtapi_task_context_t.h>
#include <embb_mtapi_pool_template-inl.h>


mtapi_group_hndl_t MTAPI_GROUP_NONE = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };


/* ---- POOL STORAGE FUNCTIONS --------------------------------------------- */

embb_mtapi_pool_implementation(group)


/* ---- CLASS MEMBERS ------------------------------------------------------ */

void embb_mtapi_group_initialize(embb_mtapi_group_t * that) {
  assert(MTAPI_NULL != that);

  that->group_id = MTAPI_GROUP_ID_NONE;
  embb_atomic_init_int(&that->deleted, MTAPI_FALSE);
  embb_atomic_init_int(&that->num_tasks, 0);
  embb_mtapi_task_queue_initialize(&that->queue);
}

void embb_mtapi_group_finalize(embb_mtapi_group_t * that) {
  assert(MTAPI_NULL != that);

  embb_atomic_store_int(&that->deleted, MTAPI_TRUE);
  embb_atomic_destroy_int(&that->deleted);
  embb_atomic_store_int(&that->num_tasks, 0);
  embb_atomic_destroy_int(&that->num_tasks);
  embb_mtapi_task_queue_finalize(&that->queue);
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

mtapi_group_hndl_t mtapi_group_create(
  MTAPI_IN mtapi_group_id_t group_id,
  MTAPI_IN mtapi_group_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  mtapi_group_hndl_t group_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };
  embb_mtapi_group_t* group = NULL;

  embb_mtapi_log_trace("mtapi_group_create() called\n");

  if (embb_mtapi_node_is_initialized()) {
    group = embb_mtapi_group_pool_allocate(node->group_pool);
    if (MTAPI_NULL != group) {
      embb_mtapi_group_initialize(group);
      group->group_id = group_id;
      if (MTAPI_NULL != attributes) {
        group->attributes = *attributes;
        local_status = MTAPI_SUCCESS;
      } else {
        mtapi_groupattr_init(&group->attributes, &local_status);
      }
      if (MTAPI_SUCCESS == local_status) {
        group_hndl = group->handle;
      } else {
        embb_mtapi_group_finalize(group);
        embb_mtapi_group_pool_deallocate(node->group_pool, group);
      }
    } else {
      local_status = MTAPI_ERR_GROUP_LIMIT;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return group_hndl;
}

void mtapi_group_set_attribute(
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_OUT void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  embb_mtapi_group_t* local_group;

  embb_mtapi_log_trace("mtapi_group_set_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (embb_mtapi_group_pool_is_handle_valid(node->group_pool, group)) {
      local_group = embb_mtapi_group_pool_get_storage_for_handle(
        node->group_pool, group);
      mtapi_groupattr_set(&local_group->attributes, attribute_num,
        attribute, attribute_size, &local_status);
    } else {
      local_status = MTAPI_ERR_GROUP_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_group_get_attribute(
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_OUT void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(attribute_num);
  EMBB_UNUSED(attribute_size);

  embb_mtapi_log_trace("mtapi_group_get_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_group_pool_is_handle_valid(node->group_pool, group)) {
      /* the following is not needed for now, since there are no attributes

      embb_mtapi_group_t* local_group =
        embb_mtapi_group_pool_get_storage_for_handle(
          node->group_pool, group); */

      if (MTAPI_NULL == attribute) {
        local_status = MTAPI_ERR_PARAMETER;
      } else {
        /* switch is not needed for now, since there are no attributes
        switch (attribute_num) {
        default: */
          local_status = MTAPI_ERR_ATTR_NUM;
        /*  break;
        }*/
      }
    } else {
      local_status = MTAPI_ERR_GROUP_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_group_wait_all(
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_group_wait_all() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_group_pool_is_handle_valid(node->group_pool, group)) {
      embb_mtapi_thread_context_t * context = NULL;
      embb_mtapi_group_t* local_group =
        embb_mtapi_group_pool_get_storage_for_handle(
          node->group_pool, group);

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

      /* wait for all tasks to arrive in the queue */
      local_status = MTAPI_SUCCESS;
      while (embb_atomic_load_int(&local_group->num_tasks)) {
        embb_mtapi_task_t* local_task;

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

        /* fetch and delete all available tasks */
        local_task = embb_mtapi_task_queue_pop_front(&local_group->queue);
        while (MTAPI_NULL != local_task) {
          if (MTAPI_SUCCESS != local_task->error_code) {
            local_status = local_task->error_code;
          }
          embb_mtapi_task_delete(local_task, node->task_pool);
          embb_atomic_fetch_and_add_int(&local_group->num_tasks, -1);

          local_task = embb_mtapi_task_queue_pop_front(&local_group->queue);
        }

        /* do other work if applicable */
        embb_mtapi_scheduler_execute_task_or_yield(
          node->scheduler,
          node,
          context);
      }
      if (MTAPI_TIMEOUT != local_status) {
        /* group becomes invalid, so delete it */
        mtapi_group_delete(group, MTAPI_NULL);
      }
    } else {
      local_status = MTAPI_ERR_GROUP_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  embb_mtapi_log_trace("mtapi_group_wait_all() returns\n");
}

void mtapi_group_wait_any(
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_OUT void** result,
  MTAPI_IN mtapi_timeout_t timeout,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  void* local_result = MTAPI_NULL;

  embb_mtapi_log_trace("mtapi_group_wait_any() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_group_pool_is_handle_valid(node->group_pool, group)) {
      embb_mtapi_group_t* local_group =
        embb_mtapi_group_pool_get_storage_for_handle(
          node->group_pool, group);

      embb_mtapi_task_t* local_task;
      /* are there any tasks left? */
      if (0 == embb_atomic_load_int(&local_group->num_tasks)) {
        /* group becomes invalid, so delete it */
        mtapi_group_delete(group, &local_status);
        local_status = MTAPI_GROUP_COMPLETED;
      } else {
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

        /* wait for any task to arrive */
        local_status = MTAPI_SUCCESS;
        local_task = embb_mtapi_task_queue_pop_front(&local_group->queue);
        while (MTAPI_NULL == local_task) {
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

          /* try to pop a task from the group queue */
          local_task = embb_mtapi_task_queue_pop_front(&local_group->queue);
        }
        /* was there a timeout, or is there a result? */
        if (MTAPI_NULL != local_task) {
          local_result = local_task->result_buffer;

          /* return error code set by the task */
          local_status = local_task->error_code;

          /* delete task */
          embb_mtapi_task_delete(local_task, node->task_pool);
          embb_atomic_fetch_and_add_int(&local_group->num_tasks, -1);
        }
      }
    } else {
      local_status = MTAPI_ERR_GROUP_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  /* store result */
  if (MTAPI_NULL != result) {
      *result = local_result;
  }

  mtapi_status_set(status, local_status);
  embb_mtapi_log_trace("mtapi_group_wait_any() returns\n");
}

void mtapi_group_delete(
  MTAPI_IN mtapi_group_hndl_t group,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
    if (embb_mtapi_group_pool_is_handle_valid(node->group_pool, group)) {
      embb_mtapi_group_t* local_group =
        embb_mtapi_group_pool_get_storage_for_handle(
          node->group_pool, group);

      if (embb_atomic_load_int(&local_group->deleted)) {
        local_status = MTAPI_ERR_GROUP_INVALID;
      } else {
        embb_mtapi_group_finalize(local_group);
        embb_mtapi_group_pool_deallocate(node->group_pool, local_group);
        local_status = MTAPI_SUCCESS;
      }
    } else {
      local_status = MTAPI_ERR_GROUP_INVALID;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
