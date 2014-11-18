/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_MTAPI_C_MTAPI_OPENCL_H_
#define EMBB_MTAPI_C_MTAPI_OPENCL_H_


#include <embb/mtapi/c/mtapi_ext.h>


#ifdef __cplusplus
extern "C" {
#endif


void mtapi_opencl_plugin_initialize(
  MTAPI_OUT mtapi_status_t* status
);

void mtapi_opencl_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status
);

mtapi_action_hndl_t mtapi_opencl_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN char* kernel_source,
  MTAPI_IN char* kernel_name,
  MTAPI_IN mtapi_size_t local_work_size,
  MTAPI_IN mtapi_size_t element_size,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_OUT mtapi_status_t* status
);


#ifdef __cplusplus
}
#endif


#endif // EMBB_MTAPI_C_MTAPI_OPENCL_H_
