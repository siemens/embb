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

#include <embb/base/c/internal/unused.h>

#include <embb_mtapi_log.h>
#include <embb_mtapi_alloc.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_task_context_t.h>
#include <embb_mtapi_thread_context_t.h>
#include <embb_mtapi_task_t.h>


/* ---- CLASS MEMBERS ------------------------------------------------------ */

void embb_mtapi_task_context_initialize_with_thread_context_and_task(
  embb_mtapi_task_context_t* that,
  embb_mtapi_thread_context_t* thread_context,
  embb_mtapi_task_t* task) {
  assert(MTAPI_NULL != that);
  assert(MTAPI_NULL != thread_context);
  assert(MTAPI_NULL != task);

  that->task = task;
  that->thread_context = thread_context;
  that->num_instances = task->attributes.num_instances;
  that->instance_num =
    embb_atomic_fetch_and_add_unsigned_int(&task->current_instance, 1);
}

void embb_mtapi_task_context_finalize(embb_mtapi_task_context_t* that) {
  assert(MTAPI_NULL != that);

  that->instance_num = 0;
  that->num_instances = 0;
  that->task = MTAPI_NULL;
  that->thread_context = MTAPI_NULL;
}


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_context_status_set(
  MTAPI_INOUT mtapi_task_context_t* task_context,
  MTAPI_IN mtapi_status_t error_code,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_action_status_set() called\n");

  if (MTAPI_NULL != task_context) {
    embb_mtapi_thread_context_t* local_context =
      (embb_mtapi_thread_context_t*)embb_tss_get(
        &(task_context->thread_context->tss_id));

    if (local_context == task_context->thread_context) {
      /* for remote actions the result shall be transferred to the
         waiting node at the end of the task */
      switch (error_code) {
      case MTAPI_SUCCESS:
      case MTAPI_ERR_ARG_SIZE:
      case MTAPI_ERR_RESULT_SIZE:
      case MTAPI_ERR_ACTION_CANCELLED:
      case MTAPI_ERR_ACTION_FAILED:
      case MTAPI_ERR_ACTION_DELETED:
        task_context->task->error_code = error_code;
        local_status = MTAPI_SUCCESS;
        break;
      case MTAPI_ERR_CORE_NUM:
      case MTAPI_ERR_RUNTIME_LOADBALANCING_NOTSUPPORTED:
      case MTAPI_ERR_RUNTIME_REMOTETASKS_NOTSUPPORTED:
      case MTAPI_ERR_ARG_NOT_IMPLEMENTED:
      case MTAPI_ERR_FUNC_NOT_IMPLEMENTED:
      case MTAPI_ERR_WAIT_PENDING:
      case MTAPI_ERR_BUFFER_SIZE:
      case MTAPI_ERR_UNKNOWN:
      case MTAPI_GROUP_COMPLETED:
      case MTAPI_ERR_GROUP_LIMIT:
      case MTAPI_ERR_GROUP_INVALID:
      case MTAPI_ERR_QUEUE_LIMIT:
      case MTAPI_ERR_QUEUE_DISABLED:
      case MTAPI_ERR_QUEUE_DELETED:
      case MTAPI_ERR_QUEUE_INVALID:
      case MTAPI_ERR_JOB_INVALID:
      case MTAPI_ERR_TASK_LIMIT:
      case MTAPI_ERR_TASK_INVALID:
      case MTAPI_ERR_CONTEXT_OUTOFCONTEXT:
      case MTAPI_ERR_CONTEXT_INVALID:
      case MTAPI_ERR_ACTION_DISABLED:
      case MTAPI_ERR_ACTION_NUM_INVALID:
      case MTAPI_ERR_ACTION_LIMIT:
      case MTAPI_ERR_ACTION_EXISTS:
      case MTAPI_ERR_ACTION_INVALID:
      case MTAPI_ERR_NODE_NOTINIT:
      case MTAPI_ERR_DOMAIN_INVALID:
      case MTAPI_ERR_NODE_INVALID:
      case MTAPI_ERR_NODE_INITIALIZED:
      case MTAPI_ERR_NODE_INITFAILED:
      case MTAPI_ERR_ATTR_SIZE:
      case MTAPI_ERR_ATTR_READONLY:
      case MTAPI_ERR_PARAMETER:
      case MTAPI_TIMEOUT:
      case MTAPI_ERR_ATTR_NUM:
      default:
        /* trying to set invalid error code */
        local_status = MTAPI_ERR_PARAMETER;
        break;
      }
    } else {
      local_status = MTAPI_ERR_CONTEXT_OUTOFCONTEXT;
    }
  } else {
    local_status = MTAPI_ERR_CONTEXT_INVALID;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_context_runtime_notify(
  MTAPI_IN mtapi_task_context_t* task_context,
  MTAPI_IN mtapi_notification_t notification,
  MTAPI_IN void* data,
  MTAPI_IN mtapi_size_t data_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(notification);
  EMBB_UNUSED(data);
  EMBB_UNUSED(data_size);

  embb_mtapi_log_trace("mtapi_context_runtime_notify() called\n");

  if (MTAPI_NULL != task_context) {
    mtapi_task_context_t* local_context = (mtapi_task_context_t*) embb_tss_get(
      &(task_context->thread_context->tss_id));

    if (local_context == task_context) {
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_CONTEXT_OUTOFCONTEXT;
    }
  } else {
    local_status = MTAPI_ERR_CONTEXT_INVALID;
  }

  mtapi_status_set(status, local_status);
}

mtapi_task_state_t mtapi_context_taskstate_get(
  MTAPI_IN mtapi_task_context_t* task_context,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_task_state_t task_state = MTAPI_TASK_ERROR;

  embb_mtapi_log_trace("mtapi_context_taskstate_get() called\n");

  if (MTAPI_NULL != task_context) {
    embb_mtapi_thread_context_t* local_context =
      (embb_mtapi_thread_context_t*)embb_tss_get(
        &(task_context->thread_context->tss_id));

    if (local_context == task_context->thread_context) {
      task_state = (mtapi_task_state_t)embb_atomic_load_int(
        &task_context->task->state);
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_CONTEXT_OUTOFCONTEXT;
    }
  } else {
    local_status = MTAPI_ERR_CONTEXT_INVALID;
  }

  mtapi_status_set(status, local_status);
  return task_state;
}

/* derived from OpenMP's omp_get_thread_num(); */
mtapi_uint_t mtapi_context_instnum_get(
  MTAPI_IN mtapi_task_context_t* task_context,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_uint_t instnum = 0;

  embb_mtapi_log_trace("mtapi_context_instnum_get() called\n");

  if (MTAPI_NULL != task_context) {
    embb_mtapi_thread_context_t* local_context =
      (embb_mtapi_thread_context_t*)embb_tss_get(
        &(task_context->thread_context->tss_id));

    if (local_context == task_context->thread_context) {
      instnum = task_context->instance_num;
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_CONTEXT_OUTOFCONTEXT;
    }
  } else {
    local_status = MTAPI_ERR_CONTEXT_INVALID;
  }

  mtapi_status_set(status, local_status);
  return instnum;
}

/* derived from OpenMP's omp_get_num_threads(); */
mtapi_uint_t mtapi_context_numinst_get(
  MTAPI_IN mtapi_task_context_t* task_context,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_uint_t numinst = 0;

  embb_mtapi_log_trace("mtapi_context_numinst_get() called\n");

  if (MTAPI_NULL != task_context) {
    embb_mtapi_thread_context_t* local_context =
      (embb_mtapi_thread_context_t*)embb_tss_get(
        &(task_context->thread_context->tss_id));

    if (local_context == task_context->thread_context) {
      numinst = task_context->num_instances;
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_CONTEXT_OUTOFCONTEXT;
    }
  } else {
    local_status = MTAPI_ERR_CONTEXT_INVALID;
  }

  mtapi_status_set(status, local_status);
  return numinst;
}

mtapi_uint_t mtapi_context_corenum_get(
  MTAPI_IN mtapi_task_context_t* task_context,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_uint_t corenum = 0;

  embb_mtapi_log_trace("mtapi_context_corenum_get() called\n");

  if (MTAPI_NULL != task_context) {
    embb_mtapi_thread_context_t* local_context =
      (embb_mtapi_thread_context_t*)embb_tss_get(
        &(task_context->thread_context->tss_id));

    if (local_context == task_context->thread_context) {
      corenum = task_context->thread_context->core_num;
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_CONTEXT_OUTOFCONTEXT;
    }
  } else {
    local_status = MTAPI_ERR_CONTEXT_INVALID;
  }

  mtapi_status_set(status, local_status);
  return corenum;
}
