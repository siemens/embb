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

#ifndef EMBB_BASE_C_MEMORY_ALLOCATION_H_
#define EMBB_BASE_C_MEMORY_ALLOCATION_H_

/**
* \defgroup C_BASE_ALLOC Memory Allocation
*
* \ingroup C_BASE
*
* Functions for dynamic memory allocation
*
* There are functions for aligned and unaligned memory allocation. In debug
* mode, memory usage is tracked to detect memory leaks.
*/

#include <stdlib.h>
#include <embb/base/c/internal/config.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  /**
   * Allocates \p size bytes of memory.
   *
   * Keeps track of allocated memory in debug mode.
   *
   * \return NULL in case of failure, otherwise address of allocated memory
   * block.
   *
   * \memory <tt>size+3*sizeof(size_t)</tt> bytes in debug mode, otherwise \c
   *         size bytes
   *
   * \threadsafe
   *
   * \see embb_get_bytes_allocated()
   *
   * \ingroup C_BASE_ALLOC
   */
  void* embb_alloc(
    size_t size
    /**< [IN] Size of memory block to be allocated in bytes */
    );

  /**
   * Frees memory that has been allocated by embb_alloc() for some pointer
   * \p ptr.
   *
   * Keeps track of freed memory in debug mode.
   *
   * \pre \c ptr is not NULL.
   *
   * \threadsafe
   *
   * \see embb_get_bytes_allocated()
   *
   * \ingroup C_BASE_ALLOC
   */
  void embb_free(
    void* ptr
    /**< [IN,OUT] Pointer to memory block to be freed */
    );

  /**
   * Allocates \p size bytes of memory with alignment \p alignment.
   *
   * This function can be used to align objects to certain boundaries such as
   * cache lines, memory pages, etc.
   *
   * Keeps track of allocated memory in debug mode.
   *
   * It is not required that \p size is a multiple of \p alignment as, e.g.,
   * for the \c aligned\_alloc function of the C11 standard.
   *
   * \pre The alignment has to be power of 2 and a multiple of
   *      <tt>size(void*)</tt>.
   * \post The returned pointer is a multiple of \p alignment.
   *
   * \return NULL in case of failure, otherwise address of allocated memory
   * block.
   *
   * \memory Debug mode: Let \c n be the number of aligned cells necessary to
   *         fit the payload. Then, <tt>(n+1)*alignment+3*size_of(size_t)-1</tt>
   *         bytes are allocated.<br> Release mode: \c size bytes are requested
   *         using the functions provided by the operating systems.
   *
   * \threadsafe
   *
   * \note Memory allocated using this function must be freed using
   *       embb_free_aligned().
   *
   * \see embb_alloc_cache_aligned(), embb_free_aligned(),
   *      embb_get_bytes_allocated()
   *
   * \ingroup C_BASE_ALLOC
   */
  void* embb_alloc_aligned(
    size_t alignment,
    /**< [IN] Alignment in bytes */
    size_t size
    /**< [IN] Size of memory block to be allocated in bytes */
    );

  /**
   * Allocates \p size bytes of cache-aligned memory.
   *
   * Specialized version of embb_alloc_aligned(). The alignment is chosen
   * automatically (usually 64 bytes).
   *
   * Keeps track of allocated memory in debug mode.
   *
   * \post The returned pointer is a multiple of the cache line size.
   *
   * \return NULL in case of failure, otherwise address of allocated memory
   *         block.
   *
   * \memory See embb_alloc_aligned()
   *
   * \threadsafe
   *
   * \note Memory allocated using this function must be freed using
   *       embb_free_aligned().
   *
   * \see embb_alloc_aligned(), embb_free_aligned(), embb_get_bytes_allocated()
   *
   * \ingroup C_BASE_ALLOC
   */
  void* embb_alloc_cache_aligned(
    size_t size
    /**< [IN] Size of memory block to be allocated in bytes */
    );

  /**
   * Frees memory that has been allocated by an aligned method for \c ptr.
   *
   * The available aligned methods are embb_alloc_aligned() or
   * embb_alloc_cache_aligned().
   *
   * Keeps track of freed memory in debug mode.
   *
   * \pre \c ptr is not NULL and was allocated by an aligned method.
   *
   * \threadsafe
   *
   * \see embb_alloc_aligned(), embb_alloc_cache_aligned(),
   *      embb_get_bytes_allocated()
   *
   * \ingroup C_BASE_ALLOC
   */
  void embb_free_aligned(
    void* ptr
    /**< [IN,OUT] Pointer to memory block to be freed */
    );

  /**
   * Returns the total number of bytes currently allocated.
   *
   * Only the bytes allocated by embb_alloc(), embb_alloc_aligned(), and
   * embb_alloc_cache_aligned() in debug mode are counted.
   *
   * \return Number of currently allocated bytes in debug mode, otherwise 0.
   *
   * \waitfree
   *
   * \see embb_alloc(), embb_alloc_aligned(), embb_alloc_cache_aligned()
   *
   * \ingroup C_BASE_ALLOC
   */
  size_t embb_get_bytes_allocated();
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EMBB_BASE_C_MEMORY_ALLOCATION_H_
