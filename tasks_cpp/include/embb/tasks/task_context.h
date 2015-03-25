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

#ifndef EMBB_TASKS_TASK_CONTEXT_H_
#define EMBB_TASKS_TASK_CONTEXT_H_

#include <embb/mtapi/c/mtapi.h>

namespace embb {
namespace tasks {

/**
  * Provides information about the status of the currently running Task.
  *
  * \ingroup CPP_TASKS
  */
class TaskContext {
 public:
  /**
    * Queries whether the Task running in the TaskContext should finish.
    * \return \c true if the Task should finish, otherwise \c false
    * \notthreadsafe
    */
  bool ShouldCancel();

  /**
    * Queries the index of the worker thread the Task is running on.
    * \return The worker thread index the Task is running on
    * \notthreadsafe
    */
  mtapi_uint_t GetCurrentCoreNumber();

  /**
    * Sets the return status of the running Task. This will be returned by
    * Task::Wait() and is set to \c MTAPI_SUCCESS by default.
    * \notthreadsafe
    */
  void SetStatus(
    mtapi_status_t error_code          /**< [in] The status to return by
                                            Task::Wait(), Group::WaitAny(),
                                            Group::WaitAll() */
    );

  friend class Node;

 private:
  explicit TaskContext(mtapi_task_context_t * task_context);

  mtapi_task_context_t * context_;
};

} // namespace tasks
} // namespace embb

#endif // EMBB_TASKS_TASK_CONTEXT_H_
