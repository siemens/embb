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

#include <stdlib.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/memory_allocation.h>

#include <embb_mtapi_alloc.h>

static EMBB_BASE_BASIC_TYPE_ATOMIC_4 embb_mtapi_alloc_bytes_allocated = 0;

void * embb_mtapi_alloc_allocate(unsigned int bytes) {
  void * ptr = embb_alloc(bytes);
  if (ptr != NULL) {
    embb_internal__atomic_fetch_and_add_4(
      &embb_mtapi_alloc_bytes_allocated, sizeof(unsigned int)+bytes);
  }
  return ptr;
}

void embb_mtapi_alloc_deallocate(void * ptr) {
  if (ptr != NULL) {
    embb_free(ptr);
  }
}

void embb_mtapi_alloc_reset_bytes_allocated() {
  embb_internal__atomic_store_4(&embb_mtapi_alloc_bytes_allocated, 0);
}

unsigned int embb_mtapi_alloc_get_bytes_allocated() {
  return embb_internal__atomic_load_4(&embb_mtapi_alloc_bytes_allocated);
}
