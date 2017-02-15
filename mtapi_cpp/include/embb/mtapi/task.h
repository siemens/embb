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

#ifndef EMBB_MTAPI_TASK_H_
#define EMBB_MTAPI_TASK_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/internal/check_status.h>

namespace embb {
namespace mtapi {

/**
 * A Task represents a running Action of a specific Job.
 *
 * \ingroup CPP_MTAPI
 */
class Task {
 public:
  /**
   * Constructs an invalid Task.
   * \waitfree
   */
  Task() {
    handle_.id = 0;
    handle_.tag = 0;
  }

  /**
   * Copies a Task.
   * \waitfree
   */
  Task(
    Task const & other                 /**< The task to copy. */
    ) : handle_(other.handle_) {
    // emtpy
  }

  /**
   * Copies a Task.
   * \waitfree
   */
  void operator=(
    Task const & other                 /**< The task to copy. */
    ) {
    handle_ = other.handle_;
  }

  /**
   * Destroys a Task.
   * \waitfree
   */
  ~Task() {
    // empty
  }

  /**
   * Waits for Task to finish for \c timeout milliseconds.
   * \return The status of the finished Task, \c MTAPI_TIMEOUT or
   * \c MTAPI_ERR_*
   * \threadsafe
   */
  mtapi_status_t Wait(
    mtapi_timeout_t timeout          /**< [in] Timeout duration in
                                          milliseconds */
    ) {
    mtapi_status_t status;
    mtapi_task_wait(handle_, timeout, &status);
    return status;
  }

  /**
   * Waits for Task to finish.
   * \return The status of the finished Task or \c MTAPI_ERR_*
   * \threadsafe
   */
  mtapi_status_t Wait() {
    mtapi_status_t status;
    mtapi_task_wait(handle_, MTAPI_INFINITE, &status);
    return status;
  }

  /**
   * Signals the Task to cancel computation.
   * \waitfree
   */
  void Cancel() {
    mtapi_status_t status;
    mtapi_task_cancel(handle_, &status);
    internal::CheckStatus(status);
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns The internal mtapi_task_hndl_t.
   * \waitfree
   */
  mtapi_task_hndl_t GetInternal() const {
    return handle_;
  }

 private:
  explicit Task(mtapi_task_hndl_t handle)
    : handle_(handle) {
    // empty
  }

  mtapi_task_hndl_t handle_;

  friend class Node;
  friend class Group;
  friend class Queue;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_TASK_H_
