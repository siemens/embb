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

#include <embb/mtapi/c/mtapi.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>
#include <embb_mtapi_attr.h>


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_taskattr_init(
  MTAPI_OUT mtapi_task_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_taskattr_init() called\n");

  if (MTAPI_NULL != attributes) {
    attributes->num_instances = 1;
    attributes->is_detached = MTAPI_FALSE;
    attributes->priority = 0;
    attributes->complete_func = MTAPI_NULL;
    attributes->user_data = MTAPI_NULL;
    attributes->problem_size = 1;
    mtapi_affinity_init(&attributes->affinity, MTAPI_TRUE, &local_status);
  } else {
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_taskattr_set(
  MTAPI_INOUT mtapi_task_attributes_t* attributes,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_taskattr_set() called\n");

  if (MTAPI_NULL != attributes) {
    if (MTAPI_ATTRIBUTE_POINTER_AS_VALUE != attribute_size &&
      MTAPI_NULL == attribute) {
      local_status = MTAPI_ERR_PARAMETER;
    } else {
      switch (attribute_num) {
      case MTAPI_TASK_DETACHED:
        local_status = embb_mtapi_attr_set_mtapi_boolean_t(
          &attributes->is_detached, attribute, attribute_size);
        break;

      case MTAPI_TASK_INSTANCES:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->num_instances, attribute, attribute_size);
        break;

      case MTAPI_TASK_PRIORITY:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->priority, attribute, attribute_size);
        break;

      case MTAPI_TASK_AFFINITY:
        local_status = embb_mtapi_attr_set_mtapi_affinity_t(
          &attributes->affinity, attribute, attribute_size);
        break;

      case MTAPI_TASK_USER_DATA:
        attributes->user_data = (void*)attribute;
        local_status = MTAPI_SUCCESS;
        break;

      case MTAPI_TASK_COMPLETE_FUNCTION:
        memcpy(&attributes->complete_func, &attribute, sizeof(void*));
        local_status = MTAPI_SUCCESS;
        break;

      case MTAPI_TASK_PROBLEM_SIZE:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->problem_size, attribute, attribute_size);
        break;

      default:
        /* attribute unknown */
        local_status = MTAPI_ERR_ATTR_NUM;
        break;
      }
    }
  } else {
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}
