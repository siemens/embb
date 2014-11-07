#include <embb_mtapi_network.h>

#include <WinSock2.h>

int embb_mtapi_network_initialize() {
  WORD ver_request;
  WSADATA wsa_data;
  int err;

  ver_request = MAKEWORD(2, 2);

  err = WSAStartup(ver_request, &wsa_data);
  if (err != 0) {
    return 0;
  } else {
    return 1;
  }
}

void embb_mtapi_network_finalize() {
  WSACleanup();
}
