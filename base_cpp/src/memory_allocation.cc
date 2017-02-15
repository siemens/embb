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

#include <embb/base/memory_allocation.h>
#include <embb/base/c/memory_allocation.h>

size_t embb::base::Allocation::AllocatedBytes() {
  return embb_get_bytes_allocated();
}

void* embb::base::Allocation::Allocate(size_t size) {
  void* alloc;
  if ((alloc = embb_alloc(size)) == 0) {
    EMBB_THROW(embb::base::NoMemoryException, "Could not allocate memory");
  }
  return alloc;
}

void embb::base::Allocation::Free(void * ptr) {
  embb_free(ptr);
}

void* embb::base::Allocation::AllocateAligned(size_t alignment, size_t size) {
  void* alloc;
  if ((alloc = embb_alloc_aligned(alignment, size)) == 0) {
    EMBB_THROW(embb::base::NoMemoryException, "Could not allocate memory");
  }
  return alloc;
}

void embb::base::Allocation::FreeAligned(void * ptr) {
  embb_free_aligned(ptr);
}

void* embb::base::Allocation::AllocateCacheAligned(size_t size) {
  void* alloc;
  if ((alloc = embb_alloc_cache_aligned(size)) == 0) {
    EMBB_THROW(embb::base::NoMemoryException, "Could not allocate memory");
  }
  return alloc;
}
