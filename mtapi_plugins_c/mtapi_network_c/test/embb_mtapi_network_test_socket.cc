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

#include <embb_mtapi_network_test_socket.h>

#include <embb_mtapi_network.h>
#include <embb_mtapi_network_socket.h>

#include <embb/base/c/memory_allocation.h>


NetworkSocketTest::NetworkSocketTest() {
  CreateUnit("mtapi network socket test").Add(
    &NetworkSocketTest::TestBasic, this);
}

void NetworkSocketTest::TestBasic() {
  int err;
  embb_mtapi_network_socket_t server_sock;
  embb_mtapi_network_socket_t accept_sock;
  embb_mtapi_network_socket_t client_sock;
  embb_mtapi_network_buffer_t send_buffer;
  embb_mtapi_network_buffer_t recv_buffer;

  embb_mtapi_network_buffer_initialize(&send_buffer, 4);
  embb_mtapi_network_buffer_initialize(&recv_buffer, 4);

  err = embb_mtapi_network_initialize();
  PT_EXPECT(err != 0);

  err = embb_mtapi_network_socket_initialize(&server_sock);
  PT_EXPECT(err != 0);
  uint16_t port = 4700;
  do {
    port++;
    err = embb_mtapi_network_socket_bind_and_listen(
      &server_sock, "127.0.0.1", port, 5);
  } while (err == 0 && port < 4800);
  PT_EXPECT(err != 0);

  err = embb_mtapi_network_socket_select(&server_sock, 1, 1);
  PT_EXPECT(err == -1);

  err = embb_mtapi_network_socket_initialize(&client_sock);
  PT_EXPECT(err != 0);
  err = embb_mtapi_network_socket_connect(&client_sock, "127.0.0.1", port);
  PT_EXPECT(err != 0);

  err = embb_mtapi_network_socket_select(&server_sock, 1, -1);
  PT_EXPECT(err == 0);

  err = embb_mtapi_network_socket_accept(&server_sock, &accept_sock);
  PT_EXPECT(err != 0);

  err = embb_mtapi_network_socket_select(&accept_sock, 1, 1);
  PT_EXPECT(err == -1);

  err = embb_mtapi_network_buffer_push_back_int32(&send_buffer, 0x12345678);
  PT_EXPECT(err == 4);
  err = embb_mtapi_network_socket_sendbuffer(&client_sock, &send_buffer);
  PT_EXPECT(err == 4);

  err = embb_mtapi_network_socket_select(&accept_sock, 1, -1);
  PT_EXPECT(err == 0);
  err = embb_mtapi_network_socket_recvbuffer(&accept_sock, &recv_buffer);
  PT_EXPECT(err == 4);

  int32_t result = 0;
  err = embb_mtapi_network_buffer_pop_front_int32(&recv_buffer, &result);
  PT_EXPECT(err == 4);
  PT_EXPECT(result == 0x12345678);

  embb_mtapi_network_socket_finalize(&accept_sock);
  embb_mtapi_network_socket_finalize(&client_sock);
  embb_mtapi_network_socket_finalize(&server_sock);

  embb_mtapi_network_buffer_finalize(&recv_buffer);
  embb_mtapi_network_buffer_finalize(&send_buffer);

  embb_mtapi_network_finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}
