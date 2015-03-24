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

#ifndef EMBB_TASKS_QUEUE_H_
#define EMBB_TASKS_QUEUE_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/tasks/action.h>
#include <embb/tasks/task.h>
#include <embb/tasks/group.h>

namespace embb {

namespace base {

class Allocation;

} // namespace base

namespace tasks {

/**
  * Allows for stream processing, either ordered or unordered.
  *
  * \ingroup CPP_TASKS
  */
class Queue {
 public:
  /**
    * Enables the Queue. \link Task Tasks \endlink enqueued while the Queue was
    * disabled are executed.
    * \waitfree
    */
  void Enable();

  /**
    * Disables the Queue. Running \link Task Tasks \endlink are canceled.
    * \waitfree
    */
  void Disable();

  /**
    * Runs an Action.
    * \return A Task identifying the Action to run
    * \throws ErrorException if the Task object could not be constructed.
    * \threadsafe
    */
  Task Spawn(
    Action action                      /**< [in] The Action to run */
    );

  /**
    * Runs an Action in the specified Group
    * \return A Task identifying the Action to run
    * \throws ErrorException if the Task object could not be constructed.
    * \threadsafe
    */
  Task Spawn(
    Group const * group,               /**< [in] The Group to run the Action
                                            in */
    Action action                      /**< [in] The Action to run */
    );

  /**
    * Runs an Action in the specified Group. The \c id is returned by
    * Group::WaitAny().
    * \return A Task identifying the Action to run
    * \throws ErrorException if the Task object could not be constructed.
    * \threadsafe
    */
  Task Spawn(
    mtapi_task_id_t id,                /**< [in] The id to return in
                                            Group::WaitAny() */
    Group const * group,               /**< [in] The Group to run the Action
                                            in */
    Action action                      /**< [in] The Action to run */
    );

  friend class embb::base::Allocation;
  friend class Node;

 private:
  Queue(Queue const & taskqueue);
  Queue(mtapi_uint_t priority, bool ordered);
  ~Queue();

  mtapi_queue_hndl_t handle_;
};

} // namespace tasks
} // namespace embb

#endif // EMBB_TASKS_QUEUE_H_
