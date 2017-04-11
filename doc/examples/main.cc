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

#include <iostream>
#include <embb/base/c/thread.h>
#include <embb/mtapi/mtapi.h>

void RunMTAPI_C();
void RunMTAPI_C_Plugin();
void RunMTAPI_C_Network();
#ifdef EMBB_WITH_OPENCL
void RunMTAPI_C_OpenCL();
#endif
#ifdef EMBB_WITH_CUDA
void RunMTAPI_C_CUDA();
#endif
void RunMTAPI_CPP();
void RunDataflowLinear();
void RunDataflowNonLinear();
void RunDataflowHeterogeneous();
void RunSTLForEach();
void RunForEach();
void RunInvoke();
void RunSorting();
void RunReduce();
void RunCounting();
void RunScan();
void RunHeterogeneous();
void RunObjectPoolExamples();
void RunStackExamples();
void RunQueueExamples();

/**
 * Runs all examples and tests their correctness.
 */
int main() {
  embb_thread_set_max_count(1024);

  std::cout << "Running examples ..." << std::endl;

  std::cout << "RunMTAPI_C() ..." << std::endl;
  RunMTAPI_C();
  std::cout << "RunMTAPI_C() ... done" << std::endl;

  std::cout << "RunMTAPI_C_Plugin() ..." << std::endl;
  RunMTAPI_C_Plugin();
  std::cout << "RunMTAPI_C_Plugin() ... done" << std::endl;

  std::cout << "RunMTAPI_C_Network() ..." << std::endl;
  RunMTAPI_C_Network();
  std::cout << "RunMTAPI_C_Network() ... done" << std::endl;

#ifdef EMBB_WITH_OPENCL
  std::cout << "RunMTAPI_C_OpenCL() ..." << std::endl;
  RunMTAPI_C_OpenCL();
  std::cout << "RunMTAPI_C_OpenCL() ... done" << std::endl;
#endif

#ifdef EMBB_WITH_CUDA
  std::cout << "RunMTAPI_C_CUDA() ..." << std::endl;
  RunMTAPI_C_CUDA();
  std::cout << "RunMTAPI_C_CUDA() ... done" << std::endl;
#endif

  std::cout << "RunMTAPI_CPP() ..." << std::endl;
  RunMTAPI_CPP();
  std::cout << "RunMTAPI_CPP() ... done" << std::endl;

  embb::mtapi::Node::Initialize(1, 1);

  std::cout << "RunDataflowLinear() ..." << std::endl;
  RunDataflowLinear();
  std::cout << "RunDataflowLinear() ... done" << std::endl;

  std::cout << "RunDataflowNonLinear() ..." << std::endl;
  RunDataflowNonLinear();
  std::cout << "RunDataflowNonLinear() ... done" << std::endl;

  std::cout << "RunDataflowHeterogeneous() ..." << std::endl;
  RunDataflowHeterogeneous();
  std::cout << "RunDataflowHeterogeneous() ... done" << std::endl;

  std::cout << "RunSTLForEach() ..." << std::endl;
  RunSTLForEach();
  std::cout << "RunSTLForEach() ... done" << std::endl;

  std::cout << "RunForEach() ..." << std::endl;
  RunForEach();
  std::cout << "RunForEach() ... done" << std::endl;

  std::cout << "RunInvoke() ..." << std::endl;
  RunInvoke();
  std::cout << "RunInvoke() ... done" << std::endl;
  
  std::cout << "RunSorting() ... " << std::endl;
  RunSorting();
  std::cout << "RunSorting() ... done" << std::endl;

  std::cout << "RunReduce() ... " << std::endl;
  RunReduce();
  std::cout << "RunReduce() ... done" << std::endl;

  std::cout << "RunCounting() ..." << std::endl;
  RunCounting();
  std::cout << "RunCounting() ... done" << std::endl;

  std::cout << "RunScan() ..." << std::endl;
  RunScan();
  std::cout << "RunScan() ... done" << std::endl;

  std::cout << "RunHeterogeneous() ..." << std::endl;
  RunHeterogeneous();
  std::cout << "RunHeterogeneous() ... done" << std::endl;

  std::cout << "RunObjectPoolExamples() ..." << std::endl;
  RunObjectPoolExamples();
  std::cout << "RunObjectPoolExamples() ... done" << std::endl;

  std::cout << "RunStackExamples() ..." << std::endl;
  RunStackExamples();
  std::cout << "RunStackExamples() ... done" << std::endl;

  std::cout << "RunQueueExamples() ..." << std::endl;
  RunQueueExamples();
  std::cout << "RunQueueExamples() ... done" << std::endl;

  std::cout << "Running examples ... done" << std::endl;
}
