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

#ifndef EMBB_BASE_C_INTERNAL_THREAD_INDEX_H_
#define EMBB_BASE_C_INTERNAL_THREAD_INDEX_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Tries to return the current thread's (internal) index.
 *
 * A new index has to be obtained only on first call of the function. Later
 * calls always succeed, since they just return the index obtained in the
 * first call.
 *
 * \pre embb_internal_thread_index_create() has been called.
 * \return EMBB_SUCCESS, if an index could be obtained.
 *         EMBB_ERROR, if no more indices were available.
 * \lockfree
 */
int embb_internal_thread_index(
  unsigned int* index
  /**< [OUT] Pointer to memory location to write thread index to. */
  );

/**
 * Returns the maximum number of available thread indices.
 *
 * \lockfree
 */
int embb_internal_thread_index_max();

/**
 * Sets the maximum number of thread indices.
 *
 * Data structures rely on the maximal number of indices. Thus, it should only
 * be set at the very beginning of a program using EMBB, or in the beginning
 * of a test case.
 *
 * \notthreadsafe
 */
void embb_internal_thread_index_set_max(
  unsigned int max
  /**< [IN] Maximum number of thread indices */
  );

/**
 * Sets the thread index to zero.
 *
 * Should only be called in tests, in order to avoid a constantly growing
 * index set.
 *
 * \waitfree
 */
void embb_internal_thread_index_reset();

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

#endif /* EMBB_BASE_C_INTERNAL_THREAD_INDEX_H_ */



