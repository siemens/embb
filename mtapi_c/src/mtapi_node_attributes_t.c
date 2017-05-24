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
#include <embb/base/c/core_set.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_attr.h>


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_nodeattr_init(
  MTAPI_OUT mtapi_node_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_nodeattr_init() called\n");

  if (MTAPI_NULL != attributes) {
    attributes->max_tasks = MTAPI_NODE_MAX_TASKS_DEFAULT;
    attributes->type = MTAPI_NODE_TYPE_SMP;
    attributes->max_actions = MTAPI_NODE_MAX_ACTIONS_DEFAULT;
    attributes->max_groups = MTAPI_NODE_MAX_GROUPS_DEFAULT;
    attributes->max_queues = MTAPI_NODE_MAX_QUEUES_DEFAULT;
    attributes->queue_limit = MTAPI_NODE_QUEUE_LIMIT_DEFAULT;
    attributes->max_jobs = MTAPI_NODE_MAX_JOBS_DEFAULT;
    attributes->max_actions_per_job = MTAPI_NODE_MAX_ACTIONS_PER_JOB_DEFAULT;
    attributes->max_priorities = MTAPI_NODE_MAX_PRIORITIES_DEFAULT;
    attributes->reuse_main_thread = MTAPI_TRUE;
    attributes->worker_priorities = NULL;

    embb_core_set_init(&attributes->core_affinity, 1);
    attributes->num_cores = embb_core_set_count(&attributes->core_affinity);

    assert(embb_core_set_count(&attributes->core_affinity) ==
      embb_core_count_available());

    local_status = MTAPI_SUCCESS;
  } else {
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_nodeattr_set(
  MTAPI_INOUT mtapi_node_attributes_t* attributes,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_nodeattr_set() called\n");

  if (MTAPI_NULL != attributes) {
    if (MTAPI_ATTRIBUTE_POINTER_AS_VALUE != attribute_size &&
      MTAPI_NULL == attribute) {
      local_status = MTAPI_ERR_PARAMETER;
    } else {
      switch (attribute_num) {
      case MTAPI_NODE_CORE_AFFINITY:
        if (MTAPI_NODE_CORE_AFFINITY_SIZE == attribute_size) {
          attributes->core_affinity = *(embb_core_set_t*)attribute;
          attributes->num_cores =
            embb_core_set_count(&attributes->core_affinity);
          local_status = MTAPI_SUCCESS;
        } else {
          local_status = MTAPI_ERR_ATTR_SIZE;
        }
        break;

      case MTAPI_NODE_NUMCORES:
        local_status = MTAPI_ERR_ATTR_READONLY;
        break;

      case MTAPI_NODE_TYPE:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->type, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_TASKS:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_tasks, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_ACTIONS:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_actions, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_GROUPS:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_groups, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_QUEUES:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_queues, attribute, attribute_size);
        break;

      case MTAPI_NODE_QUEUE_LIMIT:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->queue_limit, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_JOBS:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_jobs, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_ACTIONS_PER_JOB:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_actions_per_job, attribute, attribute_size);
        break;

      case MTAPI_NODE_MAX_PRIORITIES:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->max_priorities, attribute, attribute_size);
        break;

      case MTAPI_NODE_REUSE_MAIN_THREAD:
        local_status = embb_mtapi_attr_set_mtapi_boolean_t(
          &attributes->reuse_main_thread, attribute, attribute_size);
        break;

      case MTAPI_NODE_WORKER_PRIORITIES:
        local_status = MTAPI_SUCCESS;
        attributes->worker_priorities =
          (mtapi_worker_priority_entry_t*)attribute;
        break;

      default:
        /* attribute unknown */
        local_status = MTAPI_ERR_ATTR_NUM;
        break;
      }
    }
  } else {
    /* this should not happen, if someone calls set, a valid action_attributes
    pointer should be supplied */
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}
