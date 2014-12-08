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

#ifndef EMBB_MTAPI_C_MTAPI_NETWORK_H_
#define EMBB_MTAPI_C_MTAPI_NETWORK_H_


#include <embb/mtapi/c/mtapi_ext.h>


#ifdef __cplusplus
extern "C" {
#endif


void mtapi_network_plugin_initialize(
  MTAPI_IN char * host,
  MTAPI_IN mtapi_uint16_t port,
  MTAPI_IN mtapi_uint16_t max_connections,
  MTAPI_IN mtapi_size_t buffer_size,
  MTAPI_OUT mtapi_status_t* status
);

void mtapi_network_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status
);

mtapi_action_hndl_t mtapi_network_action_create(
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_IN mtapi_job_id_t local_job_id,
  MTAPI_IN mtapi_job_id_t remote_job_id,
  MTAPI_IN char * host,
  MTAPI_IN mtapi_uint16_t port,
  MTAPI_OUT mtapi_status_t* status
);


#ifdef __cplusplus
}
#endif


#endif // EMBB_MTAPI_C_MTAPI_NETWORK_H_
