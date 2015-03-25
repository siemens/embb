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

#ifndef EMBB_ALGORITHMS_INVOKE_H_
#define EMBB_ALGORITHMS_INVOKE_H_

#include <embb/base/function.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace algorithms {

/**
 * \defgroup CPP_ALGORITHMS_INVOKE Invoke
 * Parallel invocation of functions.
 * \ingroup CPP_ALGORITHMS
 */


/**
 * Function type used by Invoke.
 * \ingroup CPP_ALGORITHMS_INVOKE
 */
typedef embb::base::Function<void> InvokeFunctionType;

#ifdef DOXYGEN

/**
 * Spawns one to ten function objects at once and runs them in parallel.
 *
 * Blocks until all of them are done.
 *
 * \ingroup CPP_ALGORITHMS_INVOKE
 */
template<typename Function1, ...>
void Invoke(
  Function1 func1,
  /**< [in] First function object to invoke */
  ...);

/**
* Spawns one to ten function objects at once and runs them in parallel using the
* given embb::mtapi::ExecutionPolicy.
*
* Blocks until all of them are done.
*
* \ingroup CPP_ALGORITHMS_INVOKE
*/
template<typename Function1, ...>
void Invoke(
  Function1 func1,
  /**< [in] Function object to invoke */
  ...,
  const embb::mtapi::ExecutionPolicy & policy
  /**< [in] embb::mtapi::ExecutionPolicy to use */
  );

#else // DOXYGEN

namespace internal {

/**
 * Spawns an MTAPI task on construction and waits for it on destruction.
 */
template<typename Function>
class TaskWrapper {
 public:
  /**
   * Wraps the function into an embb::tasks::Action and spawns an
   * embb::tasks::Task.
   */
  explicit TaskWrapper(
    Function function,
    const embb::tasks::ExecutionPolicy& policy)
      : function_(function), task_() {
    embb::tasks::Action action(embb::base::MakeFunction(
      *this, &TaskWrapper::Run), policy);
    task_ = embb::tasks::Node::GetInstance().Spawn(action);
  }

  /**
   * Waits until the task has finished execution.
   */
  ~TaskWrapper() {
    task_.Wait(MTAPI_INFINITE);
  }

 private:
  Function function_;
  embb::tasks::Task task_;

  void Run(embb::tasks::TaskContext&) {
    function_();
  }
};
} // namespace internal

template<typename Function1>
void Invoke(
  Function1 func1,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
}

template<typename Function1, typename Function2>
void Invoke(
  Function1 func1,
  Function2 func2,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
}

template<typename Function1, typename Function2, typename Function3>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4, typename Function5>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4, typename Function5, typename Function6>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4, typename Function5, typename Function6,
  typename Function7>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4, typename Function5, typename Function6,
  typename Function7, typename Function8>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
  internal::TaskWrapper<Function8> wrap8(func8, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4, typename Function5, typename Function6,
  typename Function7, typename Function8, typename Function9>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9,
  const embb::tasks::ExecutionPolicy& policy) {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
  internal::TaskWrapper<Function8> wrap8(func8, policy);
  internal::TaskWrapper<Function9> wrap9(func9, policy);
}

template<typename Function1, typename Function2, typename Function3,
  typename Function4, typename Function5, typename Function6,
  typename Function7, typename Function8, typename Function9,
  typename Function10>
  void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9,
  Function10 func10,
  const embb::tasks::ExecutionPolicy& policy)  {
  internal::TaskWrapper<Function1> wrap1(func1, policy);
  internal::TaskWrapper<Function2> wrap2(func2, policy);
  internal::TaskWrapper<Function3> wrap3(func3, policy);
  internal::TaskWrapper<Function4> wrap4(func4, policy);
  internal::TaskWrapper<Function5> wrap5(func5, policy);
  internal::TaskWrapper<Function6> wrap6(func6, policy);
  internal::TaskWrapper<Function7> wrap7(func7, policy);
  internal::TaskWrapper<Function8> wrap8(func8, policy);
  internal::TaskWrapper<Function9> wrap9(func9, policy);
  internal::TaskWrapper<Function10> wrap10(func10, policy);
}

template<typename Function1>
void Invoke(
  Function1 func1) {
  Invoke(func1, embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2>
void Invoke(
  Function1 func1,
  Function2 func2) {
  Invoke(func1, func2, embb::tasks::ExecutionPolicy());
}
template<typename Function1, typename Function2, typename Function3>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3) {
  Invoke(func1, func2, func3, embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4) {
  Invoke(func1, func2, func3, func4, embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5) {
  Invoke(func1, func2, func3, func4, func5, embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6) {
  Invoke(func1, func2, func3, func4, func5, func6,
    embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7) {
  Invoke(func1, func2, func3, func4, func5, func6, func7,
    embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, func8,
    embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8, typename Function9>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, func8, func9,
    embb::tasks::ExecutionPolicy());
}

template<typename Function1, typename Function2, typename Function3,
         typename Function4, typename Function5, typename Function6,
         typename Function7, typename Function8, typename Function9,
         typename Function10>
void Invoke(
  Function1 func1,
  Function2 func2,
  Function3 func3,
  Function4 func4,
  Function5 func5,
  Function6 func6,
  Function7 func7,
  Function8 func8,
  Function9 func9,
  Function10 func10) {
  Invoke(func1, func2, func3, func4, func5, func6, func7, func8, func9, func10,
    embb::tasks::ExecutionPolicy());
}

#endif // else DOXYGEN

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INVOKE_H_
