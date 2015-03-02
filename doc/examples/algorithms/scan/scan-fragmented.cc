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

#include <embb/algorithms/algorithms.h>
#include <embb/base/c/internal/unused.h>
#include <vector>

void CheckResults(const std::vector<int>& results) {
  assert(results.size() == 5);
  assert(results[0] == 1);
  assert(results[1] == 3);
  assert(results[2] == 6);
  assert(results[3] == 10);
  assert(results[4] == 15);
  EMBB_UNUSED_IN_RELEASE(results);
}

/**
 * Example using embb::algorithms::Scan.
 *
 *
 */
void RunScan() {
  #include "algorithms/scan/setup-snippet.h"

  #include "algorithms/scan/sequential_prefix_sum-snippet.h"
  CheckResults(output_range);

  for (size_t i = 0; i < output_range.size(); i++) output_range[i] = 0;
  #include "algorithms/scan/prefix_sum-snippet.h"
  CheckResults(output_range);
}
