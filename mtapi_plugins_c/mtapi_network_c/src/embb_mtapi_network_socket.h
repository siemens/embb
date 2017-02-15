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

#ifndef MTAPI_PLUGINS_C_MTAPI_NETWORK_C_SRC_EMBB_MTAPI_NETWORK_SOCKET_H_
#define MTAPI_PLUGINS_C_MTAPI_NETWORK_C_SRC_EMBB_MTAPI_NETWORK_SOCKET_H_

#include <stdint.h>
#include <embb_mtapi_network_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif


struct embb_mtapi_network_socket_struct {
#ifdef _WIN32
#  ifdef _WIN64
  uint64_t handle;
#  else
  uint32_t handle;
#  endif
#else
  int handle;
#endif
};

typedef struct embb_mtapi_network_socket_struct embb_mtapi_network_socket_t;

int embb_mtapi_network_socket_initialize(
  embb_mtapi_network_socket_t * that
);

void embb_mtapi_network_socket_finalize(
  embb_mtapi_network_socket_t * that
);

int embb_mtapi_network_socket_bind_and_listen(
  embb_mtapi_network_socket_t * that,
  char const * host,
  uint16_t port,
  uint16_t max_connections
);

int embb_mtapi_network_socket_accept(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_socket_t * sock
);

int embb_mtapi_network_socket_connect(
  embb_mtapi_network_socket_t * that,
  const char * host,
  uint16_t port
);

int embb_mtapi_network_socket_select(
  embb_mtapi_network_socket_t * sockets,
  int count,
  int timeout
);

int embb_mtapi_network_socket_sendbuffer(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer
);

int embb_mtapi_network_socket_recvbuffer(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer
);

int embb_mtapi_network_socket_recvbuffer_sized(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer,
  int size
);

#ifdef __cplusplus
}
#endif

#endif // MTAPI_PLUGINS_C_MTAPI_NETWORK_C_SRC_EMBB_MTAPI_NETWORK_SOCKET_H_
