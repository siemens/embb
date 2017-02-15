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

#include <partest/partest.h>
#include <embb/mtapi/mtapi.h>

#include <iostream>
#include <sstream>

#include <for_each_test.h>
#include <reduce_test.h>
#include <scan_test.h>
#include <count_test.h>
#include <partitioner_test.h>
#include <zip_iterator_test.h>
#include <quick_sort_test.h>
#include <merge_sort_test.h>
#include <invoke_test.h>

#include<embb/algorithms/merge_sort.h>
#include <vector>
#include <time.h>
#include <functional>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/atomic.h>


#define THIS_DOMAIN_ID 1
#define THIS_NODE_ID 1

int compute2_(int a) {
  return a * 2;
}

int compute1_() {
  return 5;
}

::std::string float_to_string(float val) {
  std::stringstream s;
  s << "Float: " << val;

  return std::string(s.str());
}

PT_MAIN("Algorithms") {
  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);

  PT_RUN(PartitionerTest);
  PT_RUN(ForEachTest);
  PT_RUN(ReduceTest);
  PT_RUN(ScanTest);
  PT_RUN(CountTest);
  PT_RUN(ZipIteratorTest);
  PT_RUN(QuickSortTest);
  PT_RUN(MergeSortTest);
  PT_RUN(InvokeTest);

  embb::mtapi::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}
