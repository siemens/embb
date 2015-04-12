/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_CPP_PERF_PERF_H_
#define EMBB_BASE_CPP_PERF_PERF_H_

#include <embb/base/perf/call_args.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

#define PT_PERF_MAIN(component) \
template <class Test> \
void PartestRunPerformanceTest(Test & test) { \
  test.Run(); \
  test.PrintReport(std::cout); \
} \
void PartestRunPerformanceTests( \
  embb::base::perf::CallArgs & perf_test_params); \
int main(int argc, char** argv) { \
  std::cout << component << ::std::endl; \
  embb::base::perf::CallArgs perf_test_params; \
  try { \
    perf_test_params.Parse(argc, argv); \
  } \
  catch (::std::runtime_error & re) { \
    ::std::cerr << re.what() << ::std::endl; \
  } \
  perf_test_params.Print(::std::cout); \
  PartestRunPerformanceTests(perf_test_params); \
} \
void PartestRunPerformanceTests( \
  embb::base::perf::CallArgs & perf_test_params)

#define PT_PERF_RUN(PT_PERF_TEST) \
( \
  (std::cout << "Running " << #PT_PERF_TEST << " ..." << std::endl), \
  PartestRunPerformanceTest<PT_PERF_TEST>(PT_PERF_TEST(perf_test_params)), \
  (std::cout << "Running " << #PT_PERF_TEST << " ..." << " done" << std::endl) \
)

#endif  // EMBB_BASE_CPP_PERF_PERF_H_