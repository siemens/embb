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

#ifndef BASE_CPP_TEST_THREAD_TEST_H_
#define BASE_CPP_TEST_THREAD_TEST_H_

#include <partest/test_case.h>
#include <string>
#include <vector>

namespace embb {
namespace base {
namespace test {

/**
 * Provides tests for class embb::base::Thread.
 */
class ThreadTest : public partest::TestCase {
 public:
  /**
   * Adds test methods.
   */
  ThreadTest();

 private:
  /**
   * Member thread start functor without arguments.
   */
  template<typename Function>
  struct MemberStart {
    Function f_;
    ThreadTest* self_;
    MemberStart(Function f, ThreadTest* self) : f_(f), self_(self) {}
    void operator()() { (self_->*f_)(); }
  };

  /**
   * Member thread start functor with one argument.
   */
  template<typename Function, typename Arg1>
  struct MemberStartArg1 {
    Function f_;
    ThreadTest* self_;
    Arg1 arg1_;
    MemberStartArg1(Function f, ThreadTest* self, Arg1 arg1)
      : f_(f), self_(self), arg1_(arg1) {}
    void operator()() { (self_->*f_)(arg1_); }
   private:
     /**
      * Deactivated to avoid MSVC warning "could not create ..."
      */
     MemberStartArg1& operator=(const MemberStartArg1&);
  };

  /**
   * Member thread start functor with two arguments.
   */
  template<typename Function, typename Arg1, typename Arg2>
  struct MemberStartArg2 {
    Function f_;
    ThreadTest* self_;
    Arg1 arg1_;
    Arg2 arg2_;
    MemberStartArg2(Function f, ThreadTest* self, Arg1 arg1, Arg2 arg2)
        : f_(f), self_(self), arg1_(arg1), arg2_(arg2) {}
    void operator()() { (self_->*f_)(arg1_, arg2_); }
   private:
    /**
     * Deactivated to avoid MSVC warning "could not create ..."
     */
    MemberStartArg2& operator=(const MemberStartArg2&);
  };

  /**
   * Sets required number of maximal threads.
   */
  void PreStartingAndJoining();

  /**
   * Tests starting and joining threads in different start methods.
   */
  void TestStartingAndJoining();

  /**
   * Tests setting thread affinities.
   */
  void TestThreadAffinities();

  /**
   * Static member thread start method without arguments.
   */
  static void StaticThreadStart();

  /**
   * Static member thread start method with one parameter.
   */
  static void StaticThreadStartArg1(int arg1);

  /**
   * Static member thread start method with two parameters.
   */
  static void StaticThreadStartArg2(int arg1, double arg2);

  /**
   * Member thread start method without arguments.
   */
  void ThreadStart();

  /**
   * Member thread start method with one argument.
   */
  void ThreadStartArg1(const std::string& arg1);

  /**
   * Member thread start method with two arguments.
   */
  void ThreadStartArg2(std::vector<int> arg1, double& arg2);

  /**
   * Number of threads used to run (some) of the tests.
   */
  size_t number_threads_;

  /**
   * Floating point comparison absolute difference.
   */
  static const double eps_;
};

} // namespace test
} // namespace base
} // namespace embb

#endif // BASE_CPP_TEST_THREAD_TEST_H_
