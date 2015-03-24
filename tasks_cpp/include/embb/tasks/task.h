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

#ifndef EMBB_TASKS_TASK_H_
#define EMBB_TASKS_TASK_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/tasks/action.h>

namespace embb {
namespace tasks {

/**
  * A Task represents a running Action.
  *
  * \ingroup CPP_TASKS
  */
class Task {
 public:
  /**
    * Constructs an empty Task
    */
  Task();

  /**
    * Copies a Task
    */
  Task(
    Task const & task                  /**< The task to copy. */
    );

  /**
    * Destroys a Task
    */
  ~Task();

  /**
    * Waits for Task to finish for \c timeout milliseconds.
    * \return The status of the finished Task, \c MTAPI_TIMEOUT or
    * \c MTAPI_ERR_*
    * \threadsafe
    */
  mtapi_status_t Wait(
    mtapi_timeout_t timeout          /**< [in] Timeout duration in
                                          milliseconds */
    );

  /**
    * Signals the Task to cancel computation.
    * \waitfree
    */
  void Cancel();

  friend class Group;
  friend class Queue;
  friend class Node;

 private:
  Task(
    Action action);

  Task(
    Action action,
    mtapi_group_hndl_t group);

  Task(
    mtapi_task_id_t id,
    Action action,
    mtapi_group_hndl_t group);

  Task(
    Action action,
    mtapi_queue_hndl_t queue);

  Task(
    Action action,
    mtapi_queue_hndl_t queue,
    mtapi_group_hndl_t group);

  Task(
    mtapi_task_id_t id,
    Action action,
    mtapi_queue_hndl_t queue,
    mtapi_group_hndl_t group);

  mtapi_task_hndl_t handle_;
};

} // namespace tasks
} // namespace embb

#endif // EMBB_TASKS_TASK_H_
