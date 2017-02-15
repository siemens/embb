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

#ifndef EMBB_MTAPI_QUEUE_H_
#define EMBB_MTAPI_QUEUE_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/job.h>
#include <embb/mtapi/task.h>
#include <embb/mtapi/group.h>
#include <embb/mtapi/queue_attributes.h>
#include <embb/mtapi/task_attributes.h>

namespace embb {

namespace base {

class Allocation;

} // namespace base

namespace mtapi {

/**
 * Allows for stream processing, either ordered or unordered.
 *
 * \ingroup CPP_MTAPI
 */
class Queue {
 public:
  /**
   * Constructs an invalid Queue.
   * \waitfree
   */
  Queue() {
    handle_.id = 0;
    handle_.tag = 0;
  }

  /**
   * Copies a Queue.
   * \waitfree
   */
  Queue(
    Queue const & other                /**< The Queue to copy */
    ) : handle_(other.handle_) {
    // empty
  }

  /**
   * Copies a Queue.
   *
   * \returns Reference to this object.
   * \waitfree
   */
  Queue & operator=(
    Queue const & other                /**< The Queue to copy */
    ) {
    handle_ = other.handle_;
    return *this;
  }

  /**
   * Deletes a Queue object.
   * \threadsafe
   */
  void Delete() {
    mtapi_queue_delete(handle_, MTAPI_INFINITE, MTAPI_NULL);
  }

  /**
   * Enables the Queue. \link Task Tasks \endlink enqueued while the Queue was
   * disabled are executed.
   * \waitfree
   */
  void Enable() {
    mtapi_status_t status;
    mtapi_queue_enable(handle_, &status);
    internal::CheckStatus(status);
  }

  /**
   * Disables the Queue. Running \link Task Tasks \endlink are canceled. The
   * Queue waits for the Tasks to finish for \c timout milliseconds.
   * \waitfree
   */
  void Disable(
    mtapi_timeout_t timeout            /**< The timeout in milliseconds. */
    ) {
    mtapi_status_t status;
    mtapi_queue_disable(handle_, timeout, &status);
    internal::CheckStatus(status);
  }

  /**
   * Disables the Queue. Running \link Task Tasks \endlink are canceled. The
   * Queue waits for the Tasks to finish.
   * \waitfree
   */
  void Disable() {
    Disable(MTAPI_INFINITE);
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    TaskAttributes const & attributes, /**< Attributes of the Task */
    Group const & group                /**< The Group to start the Task in */
    ) {
    return Enqueue(task_id,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      &attributes.GetInternal(), group.GetInternal());
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    Group const & group                /**< The Group to start the Task in */
    ) {
    return Enqueue(task_id,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      MTAPI_DEFAULT_TASK_ATTRIBUTES, group.GetInternal());
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    TaskAttributes const & attributes  /**< Attributes of the Task */
    ) {
    return Enqueue(task_id,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      &attributes.GetInternal(), MTAPI_GROUP_NONE);
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results                      /**< Pointer to the results. */
    ) {
    return Enqueue(task_id,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE);
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    TaskAttributes const & attributes, /**< Attributes of the Task */
    Group const & group                /**< The Group to start the Task in */
    ) {
    return Enqueue(MTAPI_TASK_ID_NONE,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      &attributes.GetInternal(), group.GetInternal());
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    Group const & group                /**< The Group to start the Task in */
    ) {
    return Enqueue(MTAPI_TASK_ID_NONE,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      MTAPI_DEFAULT_TASK_ATTRIBUTES, group.GetInternal());
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results,                     /**< Pointer to the results. */
    TaskAttributes const & attributes  /**< Attributes of the Task */
    ) {
    return Enqueue(MTAPI_TASK_ID_NONE,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      &attributes.GetInternal(), MTAPI_GROUP_NONE);
  }

  /**
   * Enqueues a new Task.
   *
   * \returns The handle to the enqueued Task.
   * \threadsafe
   */
  template <typename ARGS, typename RES>
  Task Enqueue(
    const ARGS * arguments,            /**< Pointer to the arguments. */
    RES * results                      /**< Pointer to the results. */
    ) {
    return Enqueue(MTAPI_TASK_ID_NONE,
      arguments, internal::SizeOfType<ARGS>(),
      results, internal::SizeOfType<RES>(),
      MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE);
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns The internal mtapi_queue_hndl_t.
   * \waitfree
   */
  mtapi_queue_hndl_t GetInternal() const {
    return handle_;
  }

  friend class embb::base::Allocation;
  friend class Node;

 private:
  Queue(
    mtapi_queue_id_t queue_id,
    Job const & job,
    mtapi_queue_attributes_t const * attributes
    ) {
    mtapi_status_t status;
    handle_ = mtapi_queue_create(queue_id, job.GetInternal(),
      attributes, &status);
    internal::CheckStatus(status);
  }

  Task Enqueue(
    mtapi_task_id_t task_id,
    const void * arguments,
    mtapi_size_t arguments_size,
    void * results,
    mtapi_size_t results_size,
    mtapi_task_attributes_t const * attributes,
    mtapi_group_hndl_t group
    ) {
    mtapi_status_t status;
    mtapi_task_hndl_t task_hndl =
      mtapi_task_enqueue(task_id, handle_, arguments, arguments_size,
        results, results_size, attributes, group,
        &status);
    internal::CheckStatus(status);
    return Task(task_hndl);
  }

  mtapi_queue_hndl_t handle_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_QUEUE_H_
