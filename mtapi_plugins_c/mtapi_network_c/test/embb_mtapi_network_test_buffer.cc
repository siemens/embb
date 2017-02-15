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

#include <embb_mtapi_network_test_buffer.h>

#include <embb_mtapi_network_buffer.h>

#include <embb/base/c/memory_allocation.h>


NetworkBufferTest::NetworkBufferTest() {
  CreateUnit("mtapi network buffer test").Add(
    &NetworkBufferTest::TestBasic, this);
}

void NetworkBufferTest::TestBasic() {
  embb_mtapi_network_buffer_t buffer;
  int err;

  embb_mtapi_network_buffer_initialize(&buffer, 1024);

  err = embb_mtapi_network_buffer_push_back_int8(&buffer, -1);
  PT_EXPECT(err == 1);
  err = embb_mtapi_network_buffer_push_back_int16(&buffer, -2);
  PT_EXPECT(err == 2);

  int8_t val8 = 0;
  err = embb_mtapi_network_buffer_pop_front_int8(&buffer, &val8);
  PT_EXPECT(err == 1);
  PT_EXPECT(val8 == -1);

  int32_t val32 = 0;
  err = embb_mtapi_network_buffer_pop_front_int32(&buffer, &val32);
  PT_EXPECT(err == 0);
  PT_EXPECT(val32 == 0);

  int16_t val16 = 0;
  err = embb_mtapi_network_buffer_pop_front_int16(&buffer, &val16);
  PT_EXPECT(err == 2);
  PT_EXPECT(val16 == -2);

  embb_mtapi_network_buffer_finalize(&buffer);

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}
