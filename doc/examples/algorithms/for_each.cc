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

static void CheckResults(const std::vector<int>& range) {
  int i = 1;
  for (const int& value : range) {
    assert(value == i * 2);
    EMBB_UNUSED_IN_RELEASE(value);
    i++;
  }
}

/**
 * Example using embb::algorithms::ForEach.
 *
 * Traversing and modifying a sequence with a for-each loop.
 */
void RunForEach() {
  std::vector<int> range(5);
  for (size_t i=0; i < range.size(); i++) {
    range[i] = static_cast<int>(i) + 1;
  }

  // snippet_begin:doubling
  using embb::algorithms::ForEach;
  ForEach(range.begin(), range.end(),
          [] (int& to_double) { to_double *= 2; });
  // snippet_end
  CheckResults(range);

  // snippet_begin:loop_doubling
  using embb::algorithms::ForLoop;
  ForLoop(0, int(range.size()),
    [&](int to_double) { range[size_t(to_double)] = (to_double + 1) * 2; });
  // snippet_end
  CheckResults(range);

  // snippet_begin:zip_setup
  std::vector<int> input_range(5);
  for (size_t i=0; i < input_range.size(); i++) {
    input_range[i] = static_cast<int>(i) + 1;
  }
  std::vector<int> output_range(5);
  // snippet_end
  // snippet_begin:zip_doubling
  using embb::algorithms::Zip;
  using embb::algorithms::ZipPair;
  ForEach(Zip(input_range.begin(), output_range.begin()),
          Zip(input_range.end(), output_range.end()),
          [] (ZipPair<int&, int&> pair) {
            pair.Second() = pair.First() * 2;
          });
  // snippet_end
  CheckResults(output_range);
}
