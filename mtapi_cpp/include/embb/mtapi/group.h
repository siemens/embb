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

#ifndef EMBB_MTAPI_GROUP_H_
#define EMBB_MTAPI_GROUP_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/job.h>
#include <embb/mtapi/task.h>
#include <embb/mtapi/task_attributes.h>
#include <embb/mtapi/group_attributes.h>

namespace embb {

namespace base {

class Allocation;

} // namespace base

namespace mtapi {

/**
 * Represents a facility to wait for multiple related
 * \link Task Tasks\endlink.
 *
 * \ingroup CPP_MTAPI
 */
class Group {
 public:
  /**
   * Constructs a Group object with default attributes.
   * Requires an initialized Node.
   */
  Group() {
    Create(MTAPI_GROUP_ID_NONE, MTAPI_DEFAULT_GROUP_ATTRIBUTES);
  }

  /**
   * Constructs a Group object using the given Attributes.
   * Requires an initialized Node.
   */
  Group(
    GroupAttributes const & attr       /**< The GroupAttributes to use. */
    ) {
    Create(MTAPI_GROUP_ID_NONE, &attr.GetInternal());
  }

  /**
   * Constructs a Group object with default attributes and the given ID.
   * Requires an initialized Node.
   */
  Group(
    mtapi_group_id_t id                /**< A user defined ID of the Group. */
    ) {
    Create(id, MTAPI_DEFAULT_GROUP_ATTRIBUTES);
  }

  /**
   * Constructs a Group object with given attributes and ID.
   * Requires an initialized Node.
   */
  Group(
    mtapi_group_id_t id,               /**< A user defined ID of the Group. */
    GroupAttributes const & attr       /**< The GroupAttributes to use. */
    ) {
    Create(id, &attr.GetInternal());
  }

  /**
   * Destroys a Group object.
   */
  ~Group() {
    // delete the group, ignore status
    mtapi_group_delete(handle_, MTAPI_NULL);
  }

  /**
   * Starts a new Task in this Group.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Start(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    Job const & job,                   /**< The Job to execute. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    TaskAttributes const & attributes  /**< Attributes of the Task */
    ) {
    return Start(task_id,
      job.GetInternal(),
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      &attributes.GetInternal());
  }

  /**
   * Starts a new Task in this Group.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Start(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    Job const & job,                   /**< The Job to execute. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results                      /**< Pointer to the results. */
    ) {
    return Start(task_id,
      job.GetInternal(),
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
  }

  /**
   * Starts a new Task in this Group.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Start(
    Job const & job,                   /**< The Job to execute. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    TaskAttributes const & attributes  /**< Attributes of the Task */
    ) {
    return Start(MTAPI_TASK_ID_NONE,
      job.GetInternal(),
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      &attributes.GetInternal());
  }

  /**
   * Starts a new Task in this Group.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Start(
    Job const & job,                   /**< The Job to execute. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results                      /**< Pointer to the results. */
    ) {
    return Start(MTAPI_TASK_ID_NONE,
      job.GetInternal(),
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      MTAPI_DEFAULT_TASK_ATTRIBUTES);
  }

  /**
   * Waits for any Task in the Group to finish for \c timeout milliseconds and
   * retrieves the result buffer given in Start().
   * \return The status of the Task that finished execution, \c MTAPI_TIMEOUT
   *         or \c MTAPI_ERR_*
   * \threadsafe
   */
  mtapi_status_t WaitAny(
    mtapi_timeout_t timeout,           /**< [in] Timeout duration in
                                            milliseconds */
    void ** result                     /**< [out] The result buffer given in
                                            Node::Start, Group::Start or
                                            Queue::Enqueue */
    ) {
    mtapi_status_t status;
    mtapi_group_wait_any(handle_, result, timeout, &status);
    needs_delete_ = status != MTAPI_GROUP_COMPLETED;
    return status;
  }

  /**
   * Waits for any Task in the Group to finish and
   * retrieves the result buffer given in Start().
   * \return The status of the Task that finished execution or \c MTAPI_ERR_*
   * \threadsafe
   */
  mtapi_status_t WaitAny(
    void ** result                     /**< [out] The result buffer given in
                                            Node::Start, Group::Start or
                                            Queue::Enqueue */
    ) {
    return WaitAny(MTAPI_INFINITE, result);
  }

  /**
   * Waits for any Task in the Group to finish for \c timeout milliseconds.
   * \return The status of the Task that finished execution
   * \threadsafe
   */
  mtapi_status_t WaitAny(
    mtapi_timeout_t timeout            /**< [in] Timeout duration in
                                       milliseconds */
                                       ) {
    return WaitAny(timeout, MTAPI_NULL);
  }

  /**
   * Waits for any Task in the Group to finish.
   * \return The status of the Task that finished execution
   * \threadsafe
   */
  mtapi_status_t WaitAny() {
    return WaitAny(MTAPI_INFINITE, MTAPI_NULL);
  }

  /**
   * Waits for all Task in the Group to finish for \c timeout milliseconds.
   * \return \c MTAPI_SUCCESS, \c MTAPI_TIMEOUT, \c MTAPI_ERR_* or the status
   *         of any failed Task
   * \threadsafe
   */
  mtapi_status_t WaitAll(
    mtapi_timeout_t timeout            /**< [in] Timeout duration in
                                            milliseconds */
  ) {
    mtapi_status_t status;
    mtapi_group_wait_all(handle_, timeout, &status);
    needs_delete_ = status != MTAPI_SUCCESS;
    return status;
  }

  /**
   * Waits for all Task in the Group to finish.
   * \return \c MTAPI_SUCCESS, \c MTAPI_TIMEOUT, \c MTAPI_ERR_* or the status
   *         of any failed Task
   * \threadsafe
   */
  mtapi_status_t WaitAll() {
    return WaitAll(MTAPI_INFINITE);
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns The internal mtapi_group_hndl_t.
   * \waitfree
   */
  mtapi_group_hndl_t GetInternal() const {
    return handle_;
  }

  friend class embb::base::Allocation;

 private:
  // not copyable
  Group(Group const & other);
  void operator=(Group const & other);

  void Create(
    mtapi_group_id_t group_id,
    mtapi_group_attributes_t const * attributes
    ) {
    needs_delete_ = false;
    mtapi_status_t status;
    handle_ = mtapi_group_create(group_id, attributes, &status);
    internal::CheckStatus(status);
    needs_delete_ = true;
  }

  Task Start(
    mtapi_task_id_t task_id,
    mtapi_job_hndl_t job,
    const void * arguments,
    mtapi_size_t arguments_size,
    void * results,
    mtapi_size_t results_size,
    mtapi_task_attributes_t const * attributes
    ) {
    mtapi_status_t status;
    mtapi_task_hndl_t task_hndl =
      mtapi_task_start(task_id, job, arguments, arguments_size,
      results, results_size, attributes, handle_,
      &status);
    internal::CheckStatus(status);
    return Task(task_hndl);
  }

  mtapi_group_hndl_t handle_;
  bool needs_delete_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_GROUP_H_
