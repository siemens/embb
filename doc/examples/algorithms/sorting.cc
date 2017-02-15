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
#include <vector>

/**
 * Example using embb::algorithms::QuickSort and MergeSort.
 *
 * Sorting a range of values.
 */
void RunSorting() {
  // snippet_begin:range_define
  std::vector<int> range;
  // snippet_end
  range.push_back(4);
  range.push_back(2);
  range.push_back(3);
  range.push_back(5);
  range.push_back(1);

  // snippet_begin:quick_sort
  using embb::algorithms::QuickSort;
  QuickSort(range.begin(), range.end());
  // snippet_end
  for (size_t i = 0; i < range.size(); i++) {
    assert(range[i] == static_cast<int>(i) + 1);
  }

  // snippet_begin:custom_quick_sort
  QuickSort(range.begin(), range.end(), std::greater<int>());
  // snippet_end
  for (size_t i = 0; i < range.size(); i++) {
    assert(range[i] == static_cast<int>(range.size() - i));
  }

  // snippet_begin:merge_sort_preallocated
  using embb::algorithms::MergeSort;
  std::vector<int> temporary_range(range.size());
  MergeSort(range.begin(), range.end(), temporary_range.begin());
  // snippet_end
  for (size_t i = 0; i < range.size(); i++) {
    assert(range[i] == static_cast<int>(i) + 1);
  }
}



