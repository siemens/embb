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

#ifndef EMBB_BASE_C_INTERNAL_BITSET_H_
#define EMBB_BASE_C_INTERNAL_BITSET_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <embb/base/c/internal/config.h>

EMBB_PLATFORM_INLINE void embb_bitset_set(
  uint64_t * that,
  unsigned int bit
  ) {
  assert(NULL != that);
  assert(64 > bit);
  *that |= (1ull << bit);
}

EMBB_PLATFORM_INLINE void embb_bitset_set_n(
  uint64_t * that,
  unsigned int count) {
  assert(NULL != that);
  assert(0 < count);
  assert(64 >= count);
  if (64 == count) {
    *that = ~0ull;
  } else {
    *that = (1ull << count) - 1ull;
  }
}

EMBB_PLATFORM_INLINE void embb_bitset_clear(
  uint64_t * that,
  unsigned int bit
  ) {
  assert(NULL != that);
  assert(64 > bit);
  *that &= ~(1ull << bit);
}

EMBB_PLATFORM_INLINE void embb_bitset_clear_all(
  uint64_t * that
  ) {
  assert(NULL != that);
  *that = 0ull;
}

EMBB_PLATFORM_INLINE unsigned int embb_bitset_is_set(
  uint64_t const * that,
  unsigned int bit
  ) {
  return (unsigned int)((*that & (1ull << bit)) ? 1 : 0);
}

EMBB_PLATFORM_INLINE void embb_bitset_intersect(
  uint64_t * that,
  uint64_t mask
  ) {
  assert(NULL != that);
  *that &= mask;
}

EMBB_PLATFORM_INLINE void embb_bitset_union(
  uint64_t * that,
  uint64_t mask
  ) {
  assert(NULL != that);
  *that |= mask;
}

EMBB_PLATFORM_INLINE unsigned int embb_bitset_count(
  uint64_t const * that
  ) {
  unsigned int count = 0;
  uint64_t mask = 1ull;
  for (unsigned int ii = 0; ii < 64; ii++) {
    if ((*that & mask) > 0) {
      count++;
    }
    mask <<= 1;
  }
  return count;
}

#endif // EMBB_BASE_C_INTERNAL_BITSET_H_
