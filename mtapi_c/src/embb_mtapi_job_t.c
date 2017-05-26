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
#include <string.h>

#include <embb/base/c/internal/unused.h>

#include <embb_mtapi_job_t.h>
#include <embb_mtapi_log.h>
#include <embb_mtapi_alloc.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_attr.h>


/* ---- POOL STORAGE FUNCTIONS --------------------------------------------- */

mtapi_boolean_t embb_mtapi_job_initialize_list(embb_mtapi_node_t * node) {
  node->job_list = (embb_mtapi_job_t*)embb_mtapi_alloc_allocate(
    sizeof(embb_mtapi_job_t)*(node->attributes.max_jobs + 1));
  if (NULL == node->job_list) {
    return MTAPI_FALSE;
  }
  mtapi_uint_t ii;
  for (ii = 0; ii <= node->attributes.max_jobs; ii++) {
    embb_mtapi_job_initialize(
      &node->job_list[ii], node->attributes.max_actions_per_job);
    node->job_list[ii].handle.id = ii;
    node->job_list[ii].handle.tag = 0;
  }
  return MTAPI_TRUE;
}

void embb_mtapi_job_finalize_list(embb_mtapi_node_t * node) {
  mtapi_uint_t ii;
  for (ii = 0; ii <= node->attributes.max_jobs; ii++) {
    embb_mtapi_job_finalize(&node->job_list[ii]);
    node->job_list[ii].handle.id = 0;
  }
  embb_mtapi_alloc_deallocate(node->job_list);
  node->job_list = MTAPI_NULL;
}


/* ---- CLASS MEMBERS ------------------------------------------------------ */

mtapi_boolean_t embb_mtapi_job_is_handle_valid(
  embb_mtapi_node_t * node,
  mtapi_job_hndl_t handle) {
  assert(MTAPI_NULL != node);
  return ((0 < handle.id) &&
    (handle.id <= node->attributes.max_jobs) &&
    (node->job_list[handle.id].handle.tag == handle.tag)) ?
      MTAPI_TRUE : MTAPI_FALSE;
}

embb_mtapi_job_t * embb_mtapi_job_get_storage_for_handle(
  embb_mtapi_node_t * node,
  mtapi_job_hndl_t handle) {
  assert(MTAPI_NULL != node);
  assert(MTAPI_NULL != node->job_list);
  assert(embb_mtapi_job_is_handle_valid(node, handle));
  return &node->job_list[handle.id];
}

mtapi_boolean_t embb_mtapi_job_is_id_valid(
  embb_mtapi_node_t * node,
  mtapi_job_id_t id) {
  assert(MTAPI_NULL != node);
  return ((0 < id) && (id <= node->attributes.max_jobs)) ?
    MTAPI_TRUE : MTAPI_FALSE;
}

embb_mtapi_job_t * embb_mtapi_job_get_storage_for_id(
  embb_mtapi_node_t * node,
  mtapi_job_id_t id) {
  assert(MTAPI_NULL != node);
  assert(MTAPI_NULL != node->job_list);
  assert(embb_mtapi_job_is_id_valid(node, id));
  return &node->job_list[id];
}

void embb_mtapi_job_initialize(
  embb_mtapi_job_t * that,
  mtapi_uint_t max_actions) {
  mtapi_uint_t ii;

  assert(MTAPI_NULL != that);

  that->handle.tag = 0;

  that->domain_id = 0;
  that->node_id = 0;
  that->num_actions = 0;
  that->actions = (mtapi_action_hndl_t*)
    embb_mtapi_alloc_allocate(sizeof(mtapi_action_hndl_t)*max_actions);
  if (NULL != that->actions) {
    that->max_actions = max_actions;
    for (ii = 0; ii < max_actions; ii++) {
      that->actions[ii].id = EMBB_MTAPI_IDPOOL_INVALID_ID;
    }
  } else {
    that->max_actions = 0;
  }
  that->attributes.problem_size_func = MTAPI_NULL;
  that->attributes.default_problem_size = 1;
}

void embb_mtapi_job_finalize(embb_mtapi_job_t * that) {
  assert(MTAPI_NULL != that);

  that->handle.tag++;

  that->domain_id = 0;
  that->node_id = 0;
  that->num_actions = 0;
  that->max_actions = 0;
  embb_mtapi_alloc_deallocate(that->actions);
  that->actions = NULL;
}

mtapi_boolean_t embb_mtapi_job_add_action(
  embb_mtapi_job_t * that,
  embb_mtapi_action_t * action) {
  mtapi_boolean_t result = MTAPI_TRUE;

  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != action);

  /* check if job has enough room for another action */
  if (that->max_actions > that->num_actions) {
    that->domain_id = action->domain_id;
    that->node_id = action->node_id;
    that->actions[that->num_actions] = action->handle;
    that->num_actions++;
  } else {
    result = MTAPI_FALSE;
  }

  return result;
}

void embb_mtapi_job_remove_action(
  embb_mtapi_job_t * that,
  embb_mtapi_action_t * action) {
  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != action);
  mtapi_uint_t ii;

  for (ii = 0; ii + 1 < that->num_actions; ii++) {
    if (that->actions[ii].id == action->handle.id &&
      that->actions[ii].tag == action->handle.tag) {
      that->actions[ii] = that->actions[that->num_actions - 1];
    }
  }

  that->num_actions--;
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

mtapi_job_hndl_t mtapi_job_get(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
  embb_mtapi_job_t* job = MTAPI_NULL;
  mtapi_job_hndl_t job_hndl = { 0, EMBB_MTAPI_IDPOOL_INVALID_ID };

  EMBB_UNUSED(domain_id);

  embb_mtapi_log_trace("mtapi_job_get() called\n");

  if (embb_mtapi_node_is_initialized()) {
    if (embb_mtapi_job_is_id_valid(node, job_id)) {
      job = embb_mtapi_job_get_storage_for_id(node, job_id);
      job_hndl = job->handle;
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_JOB_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
  return job_hndl;
}

void mtapi_ext_job_set_attribute(
  MTAPI_IN mtapi_job_hndl_t job,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

  if (embb_mtapi_node_is_initialized()) {
    if (embb_mtapi_job_is_handle_valid(node, job)) {
      embb_mtapi_job_t * local_job =
        embb_mtapi_job_get_storage_for_handle(node, job);

      if (MTAPI_ATTRIBUTE_POINTER_AS_VALUE != attribute_size &&
        MTAPI_NULL == attribute) {
        local_status = MTAPI_ERR_PARAMETER;
      } else {
        switch (attribute_num) {
        case MTAPI_JOB_PROBLEM_SIZE_FUNCTION:
          memcpy(&local_job->attributes.problem_size_func,
            &attribute, sizeof(mtapi_ext_problem_size_function_t));
          local_status = MTAPI_SUCCESS;
          break;

        case MTAPI_JOB_DEFAULT_PROBLEM_SIZE:
          local_status = embb_mtapi_attr_set_mtapi_uint_t(
            &local_job->attributes.default_problem_size,
            attribute, attribute_size);
          break;

        default:
          /* attribute unknown */
          local_status = MTAPI_ERR_ATTR_NUM;
          break;
        }
      }
    } else {
      local_status = MTAPI_ERR_JOB_INVALID;
    }
  } else {
    embb_mtapi_log_error("mtapi not initialized\n");
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
