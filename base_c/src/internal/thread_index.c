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

#include <embb/base/c/internal/thread_index.h>
#include <embb/base/c/core_set.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/counter.h>
#include <embb/base/c/errors.h>
#include <embb/base/c/internal/platform.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

/**
 * Provides global variable for maximal number of thread indices.
 *
 * This function has local scope.
 */
static int embb_max_number_thread_indices_max = 0;
static EMBB_BASE_BASIC_TYPE_ATOMIC_4 embb_max_number_thread_indices_flag = 0;

unsigned int* embb_max_number_thread_indices() {
  EMBB_BASE_BASIC_TYPE_SIZE_4 compare_to = 0;
  if (embb_internal__atomic_load_4(
    &embb_max_number_thread_indices_flag) != 2) {
    if (embb_internal__atomic_compare_and_swap_4(
        &embb_max_number_thread_indices_flag, &compare_to, 1)) {
      embb_max_number_thread_indices_max =
        (int)(embb_core_count_available() * 2);
      embb_internal__atomic_store_4(&embb_max_number_thread_indices_flag, 2);
    }
    while (embb_internal__atomic_load_4(
      &embb_max_number_thread_indices_flag) != 2) {}
  }
  return (unsigned int*) &embb_max_number_thread_indices_max;
}

/**
 * Provides global variable for current thread index counter.
 *
 * This function has local scope.
 */
static EMBB_BASE_BASIC_TYPE_ATOMIC_4 embb_thread_index_counter_flag = 0;
static embb_counter_t embb_thread_index_counter_index;
embb_counter_t* embb_thread_index_counter() {
  EMBB_BASE_BASIC_TYPE_SIZE_4 compare_to = 0;
  if (embb_internal__atomic_load_4(&embb_thread_index_counter_flag) != 2) {
    if (embb_internal__atomic_compare_and_swap_4(
        &embb_thread_index_counter_flag, &compare_to, 1)) {
      embb_counter_init(&embb_thread_index_counter_index);
      embb_internal__atomic_store_4(&embb_thread_index_counter_flag, 2);
    }
    while (embb_internal__atomic_load_4(
      &embb_thread_index_counter_flag) != 2) {}
  }
  return &embb_thread_index_counter_index;
}

/**
 * Tries to return the next free (internal) thread index.
 *
 * An index is only set, if there was still one available.
 *
 * This function has local scope.
 *
 * \return EMBB_SUCCESS, if a free index was available. EMBB_ERROR otherwise.
 */
int embb_try_get_next_thread_index(unsigned int* free_index) {
  assert(free_index != NULL);
  unsigned int index = embb_counter_increment(embb_thread_index_counter());
  if (index >= *embb_max_number_thread_indices()) {
    embb_counter_decrement(embb_thread_index_counter());
    return EMBB_ERROR;
  }
  *free_index = index;
  return EMBB_SUCCESS;
}

/**
 * Thread specific thread index.
 *
 * This variable has local scope.
 *
 * This index is only used in certain data structures that have to know the
 * number of threads and their indices in advance. Only a limited number of
 * indices is made available.
 */
EMBB_THREAD_SPECIFIC unsigned int embb_internal_thread_index_var = UINT_MAX;

int embb_internal_thread_index(unsigned int* index) {
  assert(index != NULL);
  if (embb_internal_thread_index_var == UINT_MAX) {
    int status =
      embb_try_get_next_thread_index(&embb_internal_thread_index_var);
    if (status == EMBB_ERROR) {
      return EMBB_ERROR;
    }
  }
  *index = embb_internal_thread_index_var;
  return EMBB_SUCCESS;
}

int embb_internal_thread_index_max() {
  return (int)(*embb_max_number_thread_indices());
}

void embb_internal_thread_index_set_max(unsigned int max) {
  *embb_max_number_thread_indices() = max;
}

/**
 * \pre the calling thread is the only active thread
 *
 * \post the thread indices count and calling thread index is reset
 */
void embb_internal_thread_index_reset() {
  /** This function is only called in tests, usually when all other threads
   * except the main thread have terminated. However, the main thread still has
   * potentially stored its old index value in its thread local storage,
   * which might be assigned additionally to another thread (as the counter is
   * reset), which may lead to hard to detect bugs. Therefore, reset the thread
   * local thread id here.
   */
  embb_internal_thread_index_var = UINT_MAX;

  embb_counter_reset(embb_thread_index_counter());
}
