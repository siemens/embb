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

#ifndef EMBB_TASKS_ACTION_H_
#define EMBB_TASKS_ACTION_H_

#include <embb/base/function.h>
#include <embb/tasks/task_context.h>
#include <embb/tasks/execution_policy.h>

namespace embb {
namespace tasks {

/**
  * A function to be spawned as a Task.
  *
  * \ingroup CPP_TASKS
  */
class Action {
 public:
  /**
    * Constructs an empty Action.
    */
  Action()
    : function_()
    , execution_policy_() {
    // empty
  }

  /**
    * Constructs an Action from a function object.
    *
    * \tparam Function Function object
    */
  template <typename Function>
  Action(
    Function func                      /**< [in] Function object */
    )
    : function_(func)
    , execution_policy_() {
    // empty
  }

  /**
    * Constructs an Action from a function object and an Affinity.
    *
    * \tparam Function Function object
    */
  template <typename Function>
  Action(
    Function func,                     /**< [in] Function object */
    ExecutionPolicy execution_policy   /**< [in] Execution policy */
    )
    : function_(func)
    , execution_policy_(execution_policy) {
    // empty
  }

  /**
    * Executes the Action in a given TaskContext.
    */
  void operator() (
    TaskContext & context              /**< [in, out] Context the operator
                                            is executed in */
    ) {
    function_(context);
  }

  /**
    * Returns the ExecutionPolicy specified during creation.
    * \return The ExecutionPolicy of the Action
    * \waitfree
    */
  ExecutionPolicy GetExecutionPolicy() const {
    return execution_policy_;
  }

 private:
  embb::base::Function<void, TaskContext &> function_;
  ExecutionPolicy execution_policy_;
};

} // namespace tasks
} // namespace embb

#endif // EMBB_TASKS_ACTION_H_
