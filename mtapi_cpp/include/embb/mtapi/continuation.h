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

#ifndef EMBB_MTAPI_CONTINUATION_H_
#define EMBB_MTAPI_CONTINUATION_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/taskcontext.h>
#include <embb/mtapi/action.h>
#include <embb/mtapi/task.h>

namespace embb {
namespace mtapi {

/**
  *  Helper struct for Continuation.
  *
  *  \ingroup CPP_MTAPI
  */
struct ContinuationStage;

/**
  * A Continuation encapsulates a chain of \link Action Actions \endlink to be
  * executed consecutively.
  *
  * \ingroup CPP_MTAPI
  */
class Continuation {
 public:
  /**
  * Copy constructor.
  */
  Continuation(
    Continuation const & cont          /**< [in] The Continuation to copy. */
    );

  /**
  * Destructor.
  */
  ~Continuation();

  /**
    * Appends an Action to the Continuation chain.
    * \returns A reference to this Continuation chain.
    * \notthreadsafe
    */
  Continuation & Then(
    Action action                      /**< [in] The Action to append to the
                                            continuation */
    );

  /**
    * Runs the Continuation chain.
    * \returns The Task representing the Continuation chain.
    * \notthreadsafe
    */
  Task Spawn();

  /**
    * Runs the Continuation chain with the specified priority.
    * \returns The Task representing the Continuation chain.
    * \notthreadsafe
    */
  Task Spawn(
    mtapi_uint_t priority              /**< [in] The priority to use */
    );

  friend class Node;

 private:
  explicit Continuation(Action action);

  void ExecuteContinuation(TaskContext & context);

  ContinuationStage * first_;
  ContinuationStage * last_;

  mtapi_uint_t priority_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_CONTINUATION_H_
