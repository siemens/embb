#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <embb_mtapi_network_socket.h>
#include <string.h>
#include <WinSock2.h>

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
    closesocket(that->handle);
    that->handle = INVALID_SOCKET;
  }
}

int embb_mtapi_network_socket_setnonblock(
  embb_mtapi_network_socket_t * that) {
  u_long mode = 1;
  if (SOCKET_ERROR == ioctlsocket(that->handle, FIONBIO, &mode)) {
    return 0;
  } else {
    return 1;
  }
}

int embb_mtapi_network_socket_bind_and_listen(
  embb_mtapi_network_socket_t * that,
  char const * host,
  uint16_t port) {
  struct sockaddr_in in_addr;
  int reuseaddr_on = 1;

  // addr reuse
  if (SOCKET_ERROR == setsockopt(that->handle, SOL_SOCKET, SO_REUSEADDR,
    (const char *)&reuseaddr_on, sizeof(reuseaddr_on))) {
    return 0;
  }

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

  if (SOCKET_ERROR == listen(that->handle, 5)) {
    return 0;
  }

  // set non-block
  return embb_mtapi_network_socket_setnonblock(that);
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
    if (WSAEWOULDBLOCK != WSAGetLastError())
      return 0;
  }

  return 1;
}

int embb_mtapi_network_socket_sendbuffer(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer) {
  int result = send(that->handle, buffer->data, buffer->size, 0);
  if (result == buffer->size) {
    return buffer->size;
  } else {
    return 0;
  }
}

int embb_mtapi_network_socket_recvbuffer(
  embb_mtapi_network_socket_t * that,
  embb_mtapi_network_buffer_t * buffer) {
  u_long bytes_available = 0;
  if (0 != ioctlsocket(that->handle, FIONREAD, &bytes_available))
    return 0;
  if (buffer->capacity > (int)bytes_available)
    return 0;
  int result = recv(that->handle, buffer->data, buffer->capacity, 0);
  if (result != buffer->capacity)
    return 0;
  buffer->size = buffer->capacity;
  return buffer->size;
}
