/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#include <embb_mtapi_spinlock_t.h>

void embb_mtapi_spinlock_initialize(embb_mtapi_spinlock_t * that) {
  embb_atomic_store_int(that, 0);
}

void embb_mtapi_spinlock_finalize(embb_mtapi_spinlock_t * that) {
  embb_atomic_store_int(that, 0);
}

embb_atomic_int embb_mtapi_spinlock_spins = { 0 };

mtapi_boolean_t embb_mtapi_spinlock_acquire(embb_mtapi_spinlock_t * that) {
  int expected = 0;
  while (0 == embb_atomic_compare_and_swap_int(that, &expected, 1)) {
    /* empty */
    embb_atomic_fetch_and_add_int(&embb_mtapi_spinlock_spins, 1);
    expected = 0;
  }
  return MTAPI_TRUE;
}

mtapi_boolean_t embb_mtapi_spinlock_acquire_with_spincount(
  embb_mtapi_spinlock_t * that,
  mtapi_uint_t max_spin_count) {
  int expected = 0;
  mtapi_uint_t spin_count = max_spin_count;
  while (0 == embb_atomic_compare_and_swap_int(that, &expected, 1)) {
    embb_atomic_fetch_and_add_int(&embb_mtapi_spinlock_spins, 1);
    spin_count--;
    if (0 == spin_count) {
      return MTAPI_FALSE;
    }
    expected = 0;
  }

  return MTAPI_TRUE;
}

mtapi_boolean_t embb_mtapi_spinlock_release(embb_mtapi_spinlock_t * that) {
  int expected = 1;
  return embb_atomic_compare_and_swap_int(that, &expected, 0) ?
    MTAPI_TRUE : MTAPI_FALSE;
}
