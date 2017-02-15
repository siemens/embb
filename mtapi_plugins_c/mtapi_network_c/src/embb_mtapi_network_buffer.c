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

#include <embb_mtapi_network_buffer.h>
#include <embb/base/c/memory_allocation.h>
#include <string.h>

int embb_mtapi_network_buffer_initialize(
  embb_mtapi_network_buffer_t * that,
  int capacity) {
  int result = 1;
  that->position = 0;
  that->size = 0;
  that->data = (char*)embb_alloc((size_t)capacity);
  if (NULL != that->data) {
    that->capacity = capacity;
  } else {
    that->capacity = 0;
    result = 0;
  }
  return result;
}

void embb_mtapi_network_buffer_finalize(
  embb_mtapi_network_buffer_t * that) {
  that->position = 0;
  that->size = 0;
  that->capacity = 0;
  if (NULL != that->data) {
    embb_free(that->data);
    that->data = NULL;
  }
}

void embb_mtapi_network_buffer_clear(
  embb_mtapi_network_buffer_t * that) {
  that->position = 0;
  that->size = 0;
}

int embb_mtapi_network_buffer_push_back_int8(
  embb_mtapi_network_buffer_t * that,
  int8_t value) {
  if (that->size + 1 > that->capacity) {
    return 0;
  }
  memcpy(that->data + that->size, &value, 1);
  that->size += 1;
  return 1;
}

int embb_mtapi_network_buffer_push_back_int16(
  embb_mtapi_network_buffer_t * that,
  int16_t value) {
  if (that->size + 2 > that->capacity) {
    return 0;
  }
  memcpy(that->data + that->size, &value, 2);
  that->size += 2;
  return 2;
}

int embb_mtapi_network_buffer_push_back_int32(
  embb_mtapi_network_buffer_t * that,
  int32_t value) {
  if (that->size + 4 > that->capacity) {
    return 0;
  }
  memcpy(that->data + that->size, &value, 4);
  that->size += 4;
  return 4;
}

int embb_mtapi_network_buffer_push_back_rawdata(
  embb_mtapi_network_buffer_t * that,
  int32_t size,
  void const * rawdata) {
  if (that->size + size > that->capacity) {
    return 0;
  }
  memcpy(that->data + that->size, rawdata, (size_t)size);
  that->size += size;
  return size;
}

int embb_mtapi_network_buffer_pop_front_int8(
  embb_mtapi_network_buffer_t * that,
  int8_t * value) {
  if (that->position + 1 > that->size) {
    *value = 0;
    return 0;
  }
  memcpy(value, that->data + that->position, 1);
  that->position += 1;
  return 1;
}

int embb_mtapi_network_buffer_pop_front_int16(
  embb_mtapi_network_buffer_t * that,
  int16_t * value) {
  if (that->position + 2 > that->size) {
    *value = 0;
    return 0;
  }
  memcpy(value, that->data + that->position, 2);
  that->position += 2;
  return 2;
}

int embb_mtapi_network_buffer_pop_front_int32(
  embb_mtapi_network_buffer_t * that,
  int32_t * value) {
  if (that->position + 4 > that->size) {
    *value = 0;
    return 0;
  }
  memcpy(value, that->data + that->position, 4);
  that->position += 4;
  return 4;
}

int embb_mtapi_network_buffer_pop_front_rawdata(
  embb_mtapi_network_buffer_t * that,
  int32_t size,
  void * rawdata) {
  if (that->position + size > that->size) {
    memset(rawdata, 0, (size_t)size);
    return 0;
  }
  memcpy(rawdata, that->data + that->position, (size_t)size);
  that->position += size;
  return size;
}
