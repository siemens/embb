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
#include <embb_mtapi_attr.h>


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_queueattr_init(
  MTAPI_OUT mtapi_queue_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_node_t* node = embb_mtapi_node_get_instance();

  embb_mtapi_log_trace("mtapi_queueattr_init() called\n");

  if (MTAPI_NULL != node) {
    if (MTAPI_NULL != attributes) {
      attributes->global = MTAPI_TRUE;
      attributes->priority = 0;
      attributes->limit = node->attributes.queue_limit;
      attributes->ordered = MTAPI_TRUE;
      attributes->retain = MTAPI_FALSE;
      attributes->domain_shared = MTAPI_TRUE;
      local_status = MTAPI_SUCCESS;
    } else {
      local_status = MTAPI_ERR_PARAMETER;
    }
  } else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_queueattr_set(
  MTAPI_INOUT mtapi_queue_attributes_t* attributes,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_queueattr_set() called\n");

  if (MTAPI_NULL != attributes) {
    if (MTAPI_ATTRIBUTE_POINTER_AS_VALUE != attribute_size &&
      MTAPI_NULL == attribute) {
      local_status = MTAPI_ERR_PARAMETER;
    } else {
      switch (attribute_num) {
      case MTAPI_QUEUE_GLOBAL:
        local_status = embb_mtapi_attr_set_mtapi_boolean_t(
          &attributes->global, attribute, attribute_size);
        break;

      case MTAPI_QUEUE_PRIORITY:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->priority, attribute, attribute_size);
        break;

      case MTAPI_QUEUE_LIMIT:
        local_status = embb_mtapi_attr_set_mtapi_uint_t(
          &attributes->limit, attribute, attribute_size);
        break;

      case MTAPI_QUEUE_ORDERED:
        local_status = embb_mtapi_attr_set_mtapi_boolean_t(
          &attributes->ordered, attribute, attribute_size);
        break;

      case MTAPI_QUEUE_RETAIN:
        local_status = embb_mtapi_attr_set_mtapi_boolean_t(
          &attributes->retain, attribute, attribute_size);
        break;

      case MTAPI_QUEUE_DOMAIN_SHARED:
        local_status = embb_mtapi_attr_set_mtapi_boolean_t(
          &attributes->domain_shared, attribute, attribute_size);
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
