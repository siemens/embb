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

#include <embb/base/c/counter.h>
#include <embb/base/c/errors.h>
#include <embb/base/c/internal/unused.h>
#include <assert.h>

int embb_counter_init(embb_counter_t* counter) {
  if (counter == NULL) {
    return EMBB_ERROR;
  }
  embb_atomic_init_unsigned_int(&(counter->value), 0);
  return EMBB_SUCCESS;
}

unsigned int embb_counter_get(embb_counter_t* counter) {
  assert(counter != NULL);
  return embb_atomic_load_unsigned_int(&(counter->value));
}

void embb_counter_reset(embb_counter_t* counter) {
  assert(counter != NULL);
  embb_atomic_store_unsigned_int(&(counter->value), 0);
}

unsigned int embb_counter_increment(embb_counter_t* counter) {
  assert(counter != NULL);
  return embb_atomic_fetch_and_add_unsigned_int(&(counter->value), 1);
}

unsigned int embb_counter_decrement(embb_counter_t* counter) {
  assert(counter != NULL);
  return embb_atomic_fetch_and_add_unsigned_int(&(counter->value),
                                                (unsigned int)-1);
}

void embb_counter_destroy(embb_counter_t* counter) {
  assert(counter != NULL);
  embb_atomic_destroy_unsigned_int(&(counter->value));
}


