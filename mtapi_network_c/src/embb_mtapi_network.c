#include <embb_mtapi_network.h>

#ifdef _WIN32
#include <WinSock2.h>
#endif

int embb_mtapi_network_initialize() {
#ifdef _WIN32
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
#else
  return 1;
#endif
}

void embb_mtapi_network_finalize() {
#ifdef _WIN32
  WSACleanup();
#endif
}
