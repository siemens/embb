#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <embb_mtapi_network_socket.h>
#include <string.h>
#ifdef _WIN32
#include <WinSock2.h>
#else
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <sys/time.h>
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
    if (WSAEWOULDBLOCK != WSAGetLastError())
#else
    if (EAGAIN != errno)
#endif
      return 0;
  }

  return 1;
}

int embb_mtapi_network_socket_select(
  embb_mtapi_network_socket_t * that,
  int timeout
  ) {
  fd_set read_set;
  int max_fd;
  int err;
  struct timeval tv;
  tv.tv_sec = timeout / 1000;
  tv.tv_usec = timeout % 1000;

  FD_ZERO(&read_set);
  FD_SET(that->handle, &read_set);
  max_fd = that->handle;

  if (timeout >= 0) {
    err = select(max_fd + 1, &read_set, NULL, NULL, &tv);
  } else {
    err = select(max_fd + 1, &read_set, NULL, NULL, NULL);
  }
  if (0 == err) {
    // timeout
    return 0;
  }
  if (SOCKET_ERROR == err) {
    return 0;
  }

  if (FD_ISSET(that->handle, &read_set)) {
    return 1;
  }

  return 0;
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
  int err;
#ifdef _WIN32
  u_long bytes_available = 0;
  if (0 != ioctlsocket(that->handle, FIONREAD, &bytes_available))
#else
  int bytes_available = 0;
  if (0 != ioctl(that->handle, FIONREAD, &bytes_available))
#endif
    return 0;
  if (buffer->capacity > (int)bytes_available)
    return 0;
  err = recv(that->handle, buffer->data, buffer->capacity, 0);
  if (err != buffer->capacity)
    return 0;
  buffer->size = buffer->capacity;
  return buffer->size;
}
