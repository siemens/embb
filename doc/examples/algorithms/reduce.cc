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

#include <embb/algorithms/algorithms.h>
#include <embb/base/c/internal/unused.h>
#include <vector>
#include <cassert>

/**
 * Example using embb::algorithms::Reduce.
 *
 * Summing up a range of values and more.
 */
void RunReduce() {
  // snippet_begin:range_init
  std::vector<int> range(5);
  for (size_t i = 0; i < range.size(); i++) {
    range[i] = static_cast<int>(i) + 1;
  }
  // snippet_end

  // snippet_begin:sequential
  int sum = 0;
  for (size_t i = 0; i < range.size(); i++) {
    sum += range[i];
  }
  // snippet_end
  assert(sum == 1 + 2 + 3 + 4 + 5);

  // snippet_begin:parallel
  using embb::algorithms::Reduce;
  sum = Reduce(range.begin(), range.end(), 0, std::plus<int>());
  // snippet_end
  assert(sum == 1 + 2 + 3 + 4 + 5);

  // snippet_begin:second_range_init
  std::vector<int> second_range(5);
  for (size_t i = 0; i < range.size(); i++) {
    second_range[i] = static_cast<int>(i) + 5;
  }
  // snippet_end
  // snippet_begin:dot_product
  using embb::algorithms::Zip;
  using embb::algorithms::ZipPair;
  int dot_product = Reduce(Zip(range.begin(), second_range.begin()),
                           Zip(range.end(), second_range.end()),
                           0,
                           std::plus<int>(),
                           [](const ZipPair<int&, int&>& pair) {
                             return pair.First() * pair.Second();
                           });
  // snippet_end
  assert(dot_product == 1*5 + 2*6 + 3*7 + 4*8 + 5*9);
  EMBB_UNUSED_IN_RELEASE(dot_product);
}
