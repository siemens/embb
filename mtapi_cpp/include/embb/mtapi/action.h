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

#ifndef EMBB_MTAPI_ACTION_H_
#define EMBB_MTAPI_ACTION_H_

#include <embb/base/function.h>
#include <embb/mtapi/taskcontext.h>
#include <embb/mtapi/affinity.h>

namespace embb {
namespace mtapi {

/**
  * A function to be spawned as a Task.
  *
  * \ingroup CPP_MTAPI
  */
class Action {
 public:
  /**
    * Constructs an empty Action.
    */
  Action()
    : function_()
    , affinity_() {
    // empty
  }

  /**
    * Constructs an Action from any entity that provides an
    * operator() (TaskContext &).
    */
  template <typename Function>
  Action(
    Function func                      /**< [in] Anything that provides an
                                            operator() (TaskContext &). */
    )
    : function_(func)
    , affinity_() {
    // empty
  }

  /**
    * Constructs an Action from any entity that provides an
    * operator() (TaskContext &) and an Affinity.
    */
  template <typename Function>
  Action(
    Function func,                     /**< [in] Anything that provides an
                                            operator() (TaskContext &). */
    Affinity affinity                  /**< [in] Core affinity */
    )
    : function_(func)
    , affinity_(affinity) {
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
    * Returns the Affinity specified during creation.
    * \return The Affinity of the Action
    * \waitfree
    */
  Affinity GetAffinity() const {
    return affinity_;
  }

 private:
  embb::base::Function<void, TaskContext &> function_;
  Affinity affinity_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_ACTION_H_
