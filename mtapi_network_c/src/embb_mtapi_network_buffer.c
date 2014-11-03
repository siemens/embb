#include <embb_mtapi_network_buffer.h>
#include <embb/base/c/memory_allocation.h>
#include <string.h>

void embb_mtapi_network_buffer_initialize(
  embb_mtapi_network_buffer_t * that,
  int capacity) {
  that->position = 0;
  that->size = 0;
  that->data = embb_alloc(capacity);
  if (NULL != that->data) {
    that->capacity = capacity;
  } else {
    that->capacity = 0;
  }
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
  void * rawdata) {
  if (that->size + size > that->capacity) {
    return 0;
  }
  memcpy(that->data + that->size, rawdata, size);
  that->size += size;
  return size;
}

int embb_mtapi_network_buffer_pop_front_int8(
  embb_mtapi_network_buffer_t * that,
  int8_t * value) {
  if (that->position + 1 > that->size) {
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
    return 0;
  }
  memcpy(rawdata, that->data + that->position, size);
  that->position += size;
  return size;
}
