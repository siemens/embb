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

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/core_set.h>

#include <mtapi_status_t.h>
#include <embb_mtapi_alloc.h>
#include <embb_mtapi_job_t.h>
#include <embb_mtapi_log.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_group_t.h>
#include <embb_mtapi_task_t.h>
#include <embb_mtapi_queue_t.h>
#include <embb_mtapi_scheduler_t.h>
#include <embb_mtapi_attr.h>

#include <embb/base/c/internal/cmake_config.h>


static embb_mtapi_node_t* embb_mtapi_node_instance = NULL;

/* ---- CLASS MEMBERS ------------------------------------------------------ */

mtapi_boolean_t embb_mtapi_node_is_initialized() {
  return (mtapi_boolean_t)(embb_mtapi_node_instance != NULL);
}

embb_mtapi_node_t* embb_mtapi_node_get_instance() {
  return embb_mtapi_node_instance;
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_initialize(
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_IN mtapi_node_t node_id,
  MTAPI_IN mtapi_node_attributes_t* attributes,
  MTAPI_OUT mtapi_info_t* mtapi_info,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node;

  embb_mtapi_log_trace(
    "mtapi_initialize() called (domain: %i, node: %i)\n", domain_id, node_id);

  /* check if node was already initialized */
  if (embb_mtapi_node_is_initialized()) {
    local_status = MTAPI_ERR_NODE_INITIALIZED;

    node = embb_mtapi_node_instance;

    /* return previously set information structure */
    if (MTAPI_NULL != mtapi_info) {
      *mtapi_info = node->info;
    }
  } else {
    embb_mtapi_alloc_reset_bytes_allocated();
    /* create node instance */
    embb_mtapi_node_instance = (embb_mtapi_node_t*)
      embb_mtapi_alloc_allocate(sizeof(embb_mtapi_node_t));
    if (NULL == embb_mtapi_node_instance) {
      /* out of memory! */
      local_status = MTAPI_ERR_UNKNOWN;
    } else {
      node = embb_mtapi_node_instance;

      node->domain_id = domain_id;
      node->node_id = node_id;

      if (MTAPI_NULL != attributes) {
        node->attributes = *attributes;
        local_status = MTAPI_SUCCESS;
      } else {
        mtapi_nodeattr_init(&node->attributes, &local_status);
      }

      if (MTAPI_SUCCESS == local_status) {
        mtapi_affinity_init(&node->affinity_all, MTAPI_TRUE, &local_status);
      }

      if (MTAPI_SUCCESS == local_status) {
        embb_atomic_init_int(&node->is_scheduler_running, MTAPI_FALSE);

        /* initialize storage */
        embb_mtapi_job_initialize_list(node);
        node->action_pool = embb_mtapi_action_pool_new(
          node->attributes.max_actions);
        node->group_pool = embb_mtapi_group_pool_new(
          node->attributes.max_groups);
        node->task_pool = embb_mtapi_task_pool_new(
          node->attributes.max_tasks);
        node->queue_pool = embb_mtapi_queue_pool_new(
          node->attributes.max_queues);
        if (MTAPI_NULL == node->job_list ||
          MTAPI_NULL == node->action_pool ||
          MTAPI_NULL == node->group_pool ||
          MTAPI_NULL == node->task_pool ||
          MTAPI_NULL == node->queue_pool) {
          mtapi_finalize(NULL);
          local_status = MTAPI_ERR_NODE_INITFAILED;
        }

        if (local_status == MTAPI_SUCCESS) {
          /* initialize scheduler for local node */
          node->scheduler = embb_mtapi_scheduler_new();
          if (MTAPI_NULL != node->scheduler) {
            /* fill information structure */
            node->info.mtapi_version = 0x1000; // mtapi version 1.0
            node->info.organization_id = MCA_ORG_ID_EMB;
            node->info.implementation_version =
              EMBB_BASE_VERSION_MAJOR * 0x1000 + EMBB_BASE_VERSION_MINOR;
            node->info.number_of_domains = ~0u;
            node->info.number_of_nodes = ~0u;
            node->info.hardware_concurrency = embb_core_count_available();
            node->info.used_memory = embb_mtapi_alloc_get_bytes_allocated();
            if (MTAPI_NULL != mtapi_info) {
              *mtapi_info = node->info;
            }

            /* initialization succeeded, tell workers to start working */
            embb_atomic_store_int(&node->is_scheduler_running, MTAPI_TRUE);
          } else {
            mtapi_finalize(MTAPI_NULL);
            local_status = MTAPI_ERR_NODE_INITFAILED;
          }
        }
      } else {
        embb_mtapi_alloc_deallocate(node);
        local_status = MTAPI_ERR_PARAMETER;
      }
    }
  }

  mtapi_status_set(status, local_status);
}

void mtapi_finalize(MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_finalize() called\n");

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t* node = embb_mtapi_node_get_instance();

    /* finalize scheduler */
    if (MTAPI_NULL != node->scheduler) {
      embb_mtapi_scheduler_delete(node->scheduler);
      node->scheduler = MTAPI_NULL;
    }

    /* finalize storage in reverse order */
    if (MTAPI_NULL != node->queue_pool) {
      embb_mtapi_queue_pool_delete(node->queue_pool);
      node->queue_pool = MTAPI_NULL;
    }

    if (MTAPI_NULL != node->task_pool) {
      embb_mtapi_task_pool_delete(node->task_pool);
      node->task_pool = MTAPI_NULL;
    }

    if (MTAPI_NULL != node->group_pool) {
      embb_mtapi_group_pool_delete(node->group_pool);
      node->group_pool = MTAPI_NULL;
    }

    if (MTAPI_NULL != node->action_pool) {
      embb_mtapi_action_pool_delete(node->action_pool);
      node->action_pool = MTAPI_NULL;
    }

    if (MTAPI_NULL != node->job_list) {
      embb_mtapi_job_finalize_list(node);
    }

    embb_atomic_destroy_int(&node->is_scheduler_running);

    /* free system instance */
    embb_mtapi_alloc_deallocate(node);
    embb_mtapi_node_instance = MTAPI_NULL;

    local_status = MTAPI_SUCCESS;
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_node_get_attribute(
  MTAPI_IN mtapi_node_t node,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_OUT void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* local_node = embb_mtapi_node_get_instance();

  embb_mtapi_log_trace("mtapi_node_get_attribute() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (local_node->node_id == node) {
      if (MTAPI_NULL != attribute) {
        switch (attribute_num) {
        case MTAPI_NODE_CORE_AFFINITY:
          if (MTAPI_NODE_CORE_AFFINITY_SIZE == attribute_size) {
            *(embb_core_set_t*)attribute =
              local_node->attributes.core_affinity;
            local_status = MTAPI_SUCCESS;
          } else {
            local_status = MTAPI_ERR_ATTR_SIZE;
          }
          break;

        case MTAPI_NODE_NUMCORES:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.num_cores, attribute, attribute_size);
          break;

        case MTAPI_NODE_TYPE:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.type, attribute, attribute_size);
          break;

        case MTAPI_NODE_MAX_TASKS:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_tasks, attribute, attribute_size);
          break;

        case MTAPI_NODE_MAX_ACTIONS:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_actions, attribute, attribute_size);
          break;

        case MTAPI_NODE_MAX_GROUPS:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_groups, attribute, attribute_size);
          break;

        case MTAPI_NODE_MAX_QUEUES:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_queues, attribute, attribute_size);
          break;

        case MTAPI_NODE_QUEUE_LIMIT:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.queue_limit, attribute, attribute_size);
          break;

        case MTAPI_NODE_MAX_JOBS:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_jobs, attribute, attribute_size);
          break;

        case MTAPI_NODE_MAX_ACTIONS_PER_JOB:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_actions_per_job, attribute,
            attribute_size);
          break;

        case MTAPI_NODE_MAX_PRIORITIES:
          local_status = embb_mtapi_attr_get_mtapi_uint_t(
            &local_node->attributes.max_priorities, attribute, attribute_size);
          break;

        default:
          local_status = MTAPI_ERR_ATTR_NUM;
          break;
        }
      } else {
        local_status = MTAPI_ERR_PARAMETER;
      }
    } else {
      local_status = MTAPI_ERR_NODE_NOTINIT;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

mtapi_domain_t mtapi_domain_id_get(
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  mtapi_domain_t domain_id = MTAPI_DOMAIN_ID_INVALID;

  embb_mtapi_log_trace("mtapi_domain_id_get() called\n");

  if (embb_mtapi_node_is_initialized()) {
    domain_id = node->domain_id;
    local_status = MTAPI_SUCCESS;
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return domain_id;
}

mtapi_node_t mtapi_node_id_get(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();
  mtapi_node_t node_id = MTAPI_NODE_ID_INVALID;

  embb_mtapi_log_trace("mtapi_domain_id_get() called\n");

  if (embb_mtapi_node_is_initialized()) {
    node_id = node->node_id;
    local_status = MTAPI_SUCCESS;
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return node_id;
}
