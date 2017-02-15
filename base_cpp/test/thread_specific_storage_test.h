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

#ifndef BASE_CPP_TEST_THREAD_SPECIFIC_STORAGE_TEST_H_
#define BASE_CPP_TEST_THREAD_SPECIFIC_STORAGE_TEST_H_

#include <partest/partest.h>
#include <embb/base/thread_specific_storage.h>

namespace embb {
namespace base {
namespace test {

/**
 * Provides tests tls implementation.
 */
class ThreadSpecificStorageTest : public partest::TestCase {
 public:
  /**
   * Adds test units.
   */
  ThreadSpecificStorageTest();

 private:
  /**
   * Tests the correctness of the internal representation.
   */
  void TestInternalRepresentation();
  static void TestInternalRepresentationSetGet(
                  ThreadSpecificStorage<size_t>* tss,
                  size_t rank);

  /**
   * Test basic functionality.
   */
  void TestMultipleTSSVariables();

  /**
   * Type to test TSS constructors with initializers.
   */
  struct OneArgumentConstructorType {
    explicit OneArgumentConstructorType(int arg) : var(arg) {}
    int var;
  };
  /**
   * Type to test TSS constructors with initializers.
   */
  struct TwoArgumentConstructorType {
    TwoArgumentConstructorType(int arg1, int arg2) : var1(arg1), var2(arg2) {}
    int var1, var2;
  };
  /**
   * Type to test TSS constructors with initializers.
   */
  struct ThreeArgumentConstructorType {
    ThreeArgumentConstructorType(int arg1, int arg2, int arg3)
        : var1(arg1), var2(arg2), var3(arg3) {}
    int var1, var2, var3;
  };
  /**
   * Type to test TSS constructors with initializers.
   */
  struct FourArgumentConstructorType {
    FourArgumentConstructorType(int arg1, int arg2, int arg3, int arg4)
        : var1(arg1), var2(arg2), var3(arg3), var4(arg4) {}
    int var1, var2, var3, var4;
  };

  /**
   * Tests the different constructors.
   */
  void TestConstructors();

  /**
   * Used to differentiate between used and unused TSS slots.
   */
  size_t number_threads_;

  /**
   * TSS used in tests.
   */
  ThreadSpecificStorage<size_t> tss_;
};

} // namespace test
} // namespace base
} // namespace embb

#endif // BASE_CPP_TEST_THREAD_SPECIFIC_STORAGE_TEST_H_
