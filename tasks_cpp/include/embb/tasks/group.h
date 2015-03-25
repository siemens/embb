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

#ifndef EMBB_TASKS_GROUP_H_
#define EMBB_TASKS_GROUP_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/tasks/action.h>
#include <embb/tasks/task.h>

namespace embb {

namespace base {

class Allocation;

} // namespace base

namespace tasks {

/**
  * Represents a facility to wait for multiple related
  * \link Task Tasks\endlink.
  *
  * \ingroup CPP_TASKS
  */
class Group {
 public:
  /**
    * Runs an Action within the Group.
    * \return A Task identifying the Action to run
    * \throws ErrorException if the Task object could not be constructed.
    * \threadsafe
    */
  Task Spawn(
    Action action                      /**< [in] The Action to run */
    );

  /**
    * Runs an Action within the Group. The \c id is returned by WaitAny().
    * \return A Task identifying the Action to run
    * \throws ErrorException if the Task object could not be constructed.
    * \threadsafe
    */
  Task Spawn(
    mtapi_task_id_t id,                /**< [in] The id to return by
                                            WaitAny() */
    Action action                      /**< [in] The Action to run */
    );

  /**
    * Waits for any Task in the Group to finish for \c timeout milliseconds.
    * \return The status of the Task that finished execution
    * \threadsafe
    */
  mtapi_status_t WaitAny(
    mtapi_timeout_t timeout            /**< [in] Timeout duration in
                                            milliseconds */
    );

  /**
    * Waits for any Task in the Group to finish for \c timeout milliseconds and
    * retrieves the id given in Spawn().
    * \return The status of the Task that finished execution, \c MTAPI_TIMEOUT
    *         or \c MTAPI_ERR_*
    * \threadsafe
    */
  mtapi_status_t WaitAny(
    mtapi_timeout_t timeout,           /**< [in] Timeout duration in
                                            milliseconds */
    mtapi_task_id_t & id               /**< [out] The id given to Spawn() */
    );

  /**
    * Waits for all Task in the Group to finish for \c timeout milliseconds.
    * \return \c MTAPI_SUCCESS, \c MTAPI_TIMEOUT, \c MTAPI_ERR_* or the status
    *         of any failed Task
    * \threadsafe
    */
  mtapi_status_t WaitAll(
    mtapi_timeout_t timeout            /**< [in] Timeout duration in
                                            milliseconds */
    );

  friend class embb::base::Allocation;
  friend class Node;
  friend class Queue;

 private:
  Group(Group const & group);
  Group();
  ~Group();

  void Create();

  mtapi_group_hndl_t handle_;
};

} // namespace tasks
} // namespace embb

#endif // EMBB_TASKS_GROUP_H_
