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

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <embb_mtapi_network_socket.h>
#include <embb/base/c/internal/config.h>
#include <string.h>
#ifdef _WIN32
#include <WinSock2.h>
#else
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

int embb_mtapi_network_socket_initialize(
  embb_mtapi_network_socket_t * that) {
  that->handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (INVALID_SOCKET == that->handle) {
    return 0;
  } else {
    return 1;
  }
}

void embb_mtapi_network_socket_finalize(
  embb_mtapi_network_socket_t * that) {
  if (INVALID_SOCKET != that->handle) {
#ifdef _WIN32
    closesocket(that->handle);
#else
    close(that->handle);
#endif
    that->handle = INVALID_SOCKET;
  }
}

int embb_mtapi_network_socket_bind_and_listen(
  embb_mtapi_network_socket_t * that,
  char const * host,
  uint16_t port,
  uint16_t max_connections) {
  struct sockaddr_in in_addr;

  // bind & listen
  memset(&in_addr, 0, sizeof(in_addr));
  in_addr.sin_family = AF_INET;
  in_addr.sin_addr.s_addr = (NULL == host) ?
    htonl(INADDR_ANY) : inet_addr(host);
  in_addr.sin_port = htons(port);

  if (SOCKET_ERROR == bind(that->handle, (struct sockaddr *) &in_addr,
    sizeof(in_addr))) {
    return 0;
  }

  if (SOCKET_ERROR == listen(that->handle, max_connections)) {
    return 0;
  }

  return 1;
}

int embb_mtapi_network_socket_accept(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_socket_t * sock) {
  sock->handle = accept(that->handle, NULL, NULL);
  if (INVALID_SOCKET == sock->handle) {
    return 0;
  } else {
    return 1;
  }
}

int embb_mtapi_network_socket_connect(
  embb_mtapi_network_socket_t * that,
  const char * host,
  uint16_t port) {
  struct sockaddr_in addr;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(host);
  addr.sin_port = htons(port);

  if (SOCKET_ERROR == connect(that->handle, (struct sockaddr *)&addr,
    sizeof(addr))) {
#ifdef _WIN32
    int err = WSAGetLastError();
    if (WSAEWOULDBLOCK != err)
#else
    if (EAGAIN != errno)
#endif
      return 0;
  }

  return 1;
}

int embb_mtapi_network_socket_select(
  embb_mtapi_network_socket_t * sockets,
  int count,
  int timeout
  ) {
  fd_set read_set;
  embb_mtapi_network_socket_t max_fd = { 0 };
  int err;
  int ii;
  struct timeval tv;
  tv.tv_sec = timeout / 1000;
  tv.tv_usec = timeout % 1000;

  FD_ZERO(&read_set);
  for (ii = 0; ii < count; ii++) {
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4548)
#endif
    FD_SET(sockets[ii].handle, &read_set);
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
    if (sockets[ii].handle > max_fd.handle)
      max_fd.handle = sockets[ii].handle;
  }

  if (timeout >= 0) {
    err = select((int)max_fd.handle + 1, &read_set, NULL, NULL, &tv);
  } else {
    err = select((int)max_fd.handle + 1, &read_set, NULL, NULL, NULL);
  }
  if (0 == err) {
    // timeout
    return -1;
  }
  if (SOCKET_ERROR == err) {
    return -1;
  }

  for (ii = 0; ii < count; ii++) {
    if (FD_ISSET(sockets[ii].handle, &read_set)) {
      return ii;
    }
  }

  return -1;
}

int embb_mtapi_network_socket_sendbuffer(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer) {
  char * buf = (char*)(buffer->data);
  int cnt = 0;
  int result = send(that->handle, buf, buffer->size, 0);
  while (result > 0) {
    buf += result;
    cnt += result;
    if (cnt == buffer->size)
      break;
    result = send(that->handle, buf, buffer->size - cnt, 0);
  }
  if (cnt == buffer->size) {
    return buffer->size;
  } else {
    return 0;
  }
}

int embb_mtapi_network_socket_recvbuffer_sized(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer,
  int size) {
  int err;
  /*
#ifdef _WIN32
  u_long bytes_available = 0;
  if (0 != ioctlsocket(that->handle, FIONREAD, &bytes_available))
#else
  int bytes_available = 0;
  if (0 != ioctl(that->handle, FIONREAD, &bytes_available))
#endif
    return 0;
  */
  if (buffer->capacity < size)
    return 0;
  /*
  if (size > (int)bytes_available)
    return 0;
  */
  if (0 < size) {
    char * buf = (char*)(buffer->data);
    int cnt = 0;
    err = recv(that->handle, buf, size, 0);
    while (err > 0) {
      cnt += err;
      if (cnt == size)
        break;
      buf += err;
      err = recv(that->handle, buf, size - cnt, 0);
    }
    if (err > 0)
      err = cnt;
  } else {
    err = 0;
  }
  if (err != size)
    return 0;
  buffer->size = size;
  return buffer->size;
}

int embb_mtapi_network_socket_recvbuffer(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer) {
  return
    embb_mtapi_network_socket_recvbuffer_sized(that, buffer, buffer->capacity);
}
