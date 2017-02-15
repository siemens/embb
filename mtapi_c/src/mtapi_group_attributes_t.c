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

#include <embb/base/c/internal/unused.h>

#include <embb_mtapi_log.h>
#include <mtapi_status_t.h>


/* ---- INTERFACE FUNCTIONS ------------------------------------------------ */

void mtapi_groupattr_init(
  MTAPI_OUT mtapi_group_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  embb_mtapi_log_trace("mtapi_groupattr_init() called\n");

  if (MTAPI_NULL != attributes) {
    attributes->some_value = 0;
    local_status = MTAPI_SUCCESS;
  } else {
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}

void mtapi_groupattr_set(
  MTAPI_INOUT mtapi_group_attributes_t* attributes,
  MTAPI_IN mtapi_uint_t attribute_num,
  MTAPI_IN void* attribute,
  MTAPI_IN mtapi_size_t attribute_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(attribute_num);

  embb_mtapi_log_trace("mtapi_groupattr_set() called\n");

  if (MTAPI_NULL != attributes) {
    if (MTAPI_ATTRIBUTE_POINTER_AS_VALUE != attribute_size &&
      MTAPI_NULL == attribute) {
      local_status = MTAPI_ERR_PARAMETER;
    } else {
      /* switch is not needed for now, since there are no attributes
      switch (attribute_num) {
      default:*/
        /* unknown attribute */
        local_status = MTAPI_ERR_ATTR_NUM;
      /*  break;
      }*/
    }
  } else {
    local_status = MTAPI_ERR_PARAMETER;
  }

  mtapi_status_set(status, local_status);
}
