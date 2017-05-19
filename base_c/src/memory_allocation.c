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

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/internal/config.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/internal/unused.h>
#include <stdlib.h>
#include <assert.h>

#ifdef EMBB_DEBUG

static EMBB_BASE_BASIC_TYPE_ATOMIC_4 embb_bytes_allocated = 0;

enum {
  // Make the marking unlikely to be something else
  INVALID_ALLOCATION = 0x91919191,
  ALIGNED_ALLOCATION = 0x99AABB11,
  UNALIGNED_ALLOCATION = 0x11AABB99
};

void* embb_alloc(size_t bytes) {
  size_t bytes_to_allocate = 2 * sizeof(size_t)+bytes;
  void* allocated = malloc(bytes_to_allocate);

  if (allocated == NULL)
    return NULL;

  embb_internal__atomic_fetch_and_add_4(&embb_bytes_allocated,
    (EMBB_BASE_BASIC_TYPE_SIZE_4)bytes_to_allocate);

  size_t* x_as_size_type = (size_t*)allocated;

  x_as_size_type[1] = UNALIGNED_ALLOCATION;
  x_as_size_type[0] = (size_t)bytes_to_allocate;

  // Return the pointer to the payload
  return (void*)((size_t*)allocated + 2);
}

void embb_free(void * ptr) {
  assert(ptr != NULL);

  size_t * alloc_type = (size_t*)ptr - 1;
  size_t * bytes_allocated = (size_t*)ptr - 2;

  // Check whether this memory was allocated unaligned
  assert((*alloc_type) == UNALIGNED_ALLOCATION);

  (*alloc_type) = (size_t)INVALID_ALLOCATION;

  embb_internal__atomic_fetch_and_add_4(&embb_bytes_allocated,
    (EMBB_BASE_BASIC_TYPE_SIZE_4)(0 - (size_t)(*bytes_allocated)));

  free((size_t*)ptr - 2);
}

void* embb_alloc_aligned(size_t alignment, size_t size) {
  // In debug mode, we count the number of allocated bytes to be able to detect
  // memory leaks. For that purpose, we allocate more memory than necessary for
  // the payload. In the extra memory before the payload, we store a pointer to
  // the original allocated block, a flag which indicates whether the allocation
  // was aligned, and the number of allocated bytes. This requires additional
  // 3*sizeof(size_t) bytes of memory (additional_bytes).
  // To get at least n aligned sections and additional_bytes before, we need
  // the following number of bytes:
  // (n+1)*alignment + (additional_bytes-1)

  // n specifies the number of alignment blocks we need for the payload
  size_t n = (size + (alignment - 1)) / alignment;

  // additional_bytes specifies the number of bytes we need to store in addition
  // to the payload
  size_t additional_bytes = sizeof(size_t) * 3;

  size_t bytes_to_allocate = (n + 1)*alignment + (additional_bytes - 1);

  char* allocated = (char *)malloc(bytes_to_allocate);

  if (allocated == NULL)
    return NULL;

  // Get the next aligned pointer
  char* x = (char*)(((uintptr_t)(allocated+alignment)) & ~(alignment - 1));

  // If we do not have enough space before, get the next allocated position
  while ((x-allocated) < (long)additional_bytes)
    x += alignment;

  // Now check if our allocation algorithm worked correctly
  // 1. Enough space for the payload
  assert((size_t)((allocated + bytes_to_allocate) - x) >= size);

  // 2. x is aligned
  assert(((size_t)x % alignment) == 0);

  // 3. Enough space for the additional information
  assert((x - additional_bytes) >= allocated);

  // x is now the first aligned position (this is the return value)
  size_t* x_as_size_type = (size_t*)x;

  x_as_size_type[-1] = (size_t)ALIGNED_ALLOCATION;
  x_as_size_type[-2] = (size_t)allocated;
  x_as_size_type[-3] = bytes_to_allocate;

  embb_internal__atomic_fetch_and_add_4(&embb_bytes_allocated,
    (EMBB_BASE_BASIC_TYPE_SIZE_4)bytes_to_allocate);

  return x;
}

void embb_free_aligned(void* ptr) {
  assert(ptr != NULL);

  size_t* ptr_conv = (size_t*)ptr;

  // If embb_free_aligned is called, the memory block should have been
  // allocated using embb_alloc_aligned.
  assert(ptr_conv[-1] == ALIGNED_ALLOCATION);

  ptr_conv[-1] = (size_t)INVALID_ALLOCATION;

  embb_internal__atomic_fetch_and_add_4(&embb_bytes_allocated,
    (EMBB_BASE_BASIC_TYPE_SIZE_4)((long)0 - ptr_conv[-3]));

  free((void*)ptr_conv[-2]);
}

size_t embb_get_bytes_allocated() {
  return (size_t)(embb_internal__atomic_load_4(&embb_bytes_allocated));
}

#else // EMBB_DEBUG

void * embb_alloc(size_t bytes) {
  return malloc(bytes);
}

void embb_free(void * ptr) {
  assert(ptr != NULL);
  free(ptr);
}

void *embb_alloc_aligned(size_t alignment, size_t size) {
  void* malloc_addr = NULL;
#ifdef EMBB_PLATFORM_COMPILER_MSVC
  /*
   * From the Documentation:
   * Allocates memory on a specified alignment boundary.
   *
   * Return:
   * A pointer to the memory block that was allocated or NULL if the operation
   * failed. The pointer is a multiple of alignment.
   */
  malloc_addr = _aligned_malloc(size, alignment);
#elif defined EMBB_PLATFORM_COMPILER_GNUC
  /*
   * From the Documentation:
   * The posix_memalign() function shall allocate size bytes aligned on a
   * boundary specified by alignment, and shall return a pointer to the
   * allocated memory in memptr. The value of alignment shall be a multiple
   * of sizeof(void *), that is also a power of two. Upon successful
   * completion, the value pointed to by memptr shall be a multiple of
   * alignment.
   */
  int status = posix_memalign(&malloc_addr, alignment, size);
  EMBB_UNUSED(status);
#endif
  return malloc_addr;
}

void embb_free_aligned(void* ptr) {
  assert(ptr != NULL);

#ifdef EMBB_PLATFORM_COMPILER_MSVC
  _aligned_free(ptr);
#else
#ifdef EMBB_PLATFORM_COMPILER_GNUC
  free(ptr);
#else
#error Unsupported compiler
#endif
#endif
}

size_t embb_get_bytes_allocated() {
  return 0;
}

#endif

void *embb_alloc_cache_aligned(size_t size) {
  return embb_alloc_aligned(EMBB_PLATFORM_CACHE_LINE_SIZE, size);
}
