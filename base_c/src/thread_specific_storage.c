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

#include <embb/base/c/thread_specific_storage.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/internal/thread_index.h>
#include <embb/base/c/memory_allocation.h>
#include <stdlib.h>
#include <assert.h>

int embb_tss_create(embb_tss_t* tss) {
  if (tss == NULL) {
    return EMBB_ERROR;
  }
  tss->size = embb_thread_get_max_count();
  tss->values = (void**) embb_alloc_cache_aligned(tss->size * sizeof(void*));
  if (tss->values == NULL) {
    return EMBB_NOMEM;
  }
  for (unsigned int i = 0; i < tss->size; i++) {
    tss->values[i] = NULL;
  }
  return EMBB_SUCCESS;
}

int embb_tss_set(embb_tss_t* tss, void* value) {
  if (tss == NULL) {
    return EMBB_ERROR;
  }
  unsigned int index = 0;
  int status = embb_internal_thread_index(&index);
  if ((status != EMBB_SUCCESS) || (index >= tss->size)) {
    return EMBB_ERROR;
  }
  tss->values[index] = value;
  return EMBB_SUCCESS;
}

void* embb_tss_get(const embb_tss_t* tss) {
  if (tss == NULL) {
    return NULL;
  }
  if (tss->values == NULL) {
    return NULL;
  }
  unsigned int index = 0;
  int status = embb_internal_thread_index(&index);
  if ((status != EMBB_SUCCESS) || (index >= tss->size)) {
    return NULL;
  }
  return tss->values[index];
}

void embb_tss_delete(embb_tss_t* tss) {
  assert(tss != NULL);
  if (tss->values != NULL) {
    embb_free_aligned(tss->values);
  }
}
