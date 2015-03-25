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

#ifndef EMBB_MTAPI_NODE_H_
#define EMBB_MTAPI_NODE_H_

#include <embb/base/memory_allocation.h>
#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/status_exception.h>
#include <embb/mtapi/node_attributes.h>
#include <embb/mtapi/task.h>
#include <embb/mtapi/task_attributes.h>
#include <embb/mtapi/job.h>

namespace embb {

namespace base {

class Allocation;

} // namespace base

namespace mtapi {

/**
 * A singleton representing the MTAPI runtime.
 *
 * \ingroup CPP_MTAPI
 */
class Node {
 public:
  /**
   * Initializes the runtime singleton using default values:
   *   - all available cores will be used
   *   - maximum number of tasks is 1024
   *   - maximum number of groups is 128
   *   - maximum number of queues is 16
   *   - maximum queue capacity is 1024
   *   - maximum number of priorities is 4.
   *
   * \notthreadsafe
   * \throws ErrorException if the singleton was already initialized or the
   *         Node could not be initialized.
   * \memory Allocates about 200kb of memory.
   */
  static void Initialize(
    mtapi_domain_t domain_id,          /**< [in] The domain id to use */
    mtapi_node_t node_id               /**< [in] The node id to use */
    ) {
    if (IsInitialized()) {
      EMBB_THROW(StatusException,
        "MTAPI: node was already initialized.");
    } else {
      NodeAttributes attributes; // default attributes
      node_instance_ = embb::base::Allocation::New<Node>(
        domain_id, node_id, attributes);
    }
  }

  /**
   * Initializes the runtime singleton.
   * \notthreadsafe
   * \throws ErrorException if the singleton was already initialized or the
   *         Node could not be initialized.
   * \memory Allocates some memory depending on the values given.
   */
  static void Initialize(
    mtapi_domain_t domain_id,          /**< [in] The domain id to use */
    mtapi_node_t node_id,              /**< [in] The node id to use */
    NodeAttributes const & attributes  /**< [in] Attributes to use */
    ) {
    if (IsInitialized()) {
      EMBB_THROW(StatusException,
        "MTAPI: node was already initialized.");
    } else {
      node_instance_ = embb::base::Allocation::New<Node>(
        domain_id, node_id, attributes);
    }
  }

  /**
   * Checks if runtime is initialized.
   * \return \c true if the Node singleton is already initialized, false
   *         otherwise
   * \waitfree
   */
  static bool IsInitialized() {
    return NULL != node_instance_;
  }

  /**
   * Gets the instance of the runtime system.
   * \return Reference to the Node singleton
   * \threadsafe
   */
  static Node & GetInstance() {
    if (IsInitialized()) {
      return *node_instance_;
    } else {
      EMBB_THROW(StatusException,
        "MTAPI: node is not initialized.");
    }
  }

  /**
   * Shuts the runtime system down.
   * \throws ErrorException if the singleton is not initialized.
   * \notthreadsafe
   */
  static void Finalize() {
    if (IsInitialized()) {
      embb::base::Allocation::Delete(node_instance_);
      node_instance_ = NULL;
    } else {
      EMBB_THROW(StatusException,
        "MTAPI: node is not initialized.");
    }
  }

  /**
   * Returns the number of available cores.
   * \return The number of available cores
   * \waitfree
   */
  mtapi_uint_t GetCoreCount() const {
    return core_count_;
  }

  /**
   * Returns the number of worker threads.
   * \return The number of worker threads.
   * \waitfree
   */
  mtapi_uint_t GetWorkerThreadCount() const {
    return worker_thread_count_;
  }

  /**
   * Starts a new Task.
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
   * Starts a new Task.
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
   * Starts a new Task.
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
   * Starts a new Task.
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

  friend class embb::base::Allocation;

 private:
  // not copyable
  Node(Node const & node);
  Node const & operator=(Node const & other);

  Node(
    mtapi_domain_t domain_id,
    mtapi_node_t node_id,
    NodeAttributes const & attr) {
    mtapi_status_t status;
    mtapi_info_t info;
    mtapi_initialize(domain_id, node_id, &attr.GetInternal(), &info, &status);
    needs_finalize_ = status == MTAPI_SUCCESS;
    internal::CheckStatus(status);

    core_count_ = info.hardware_concurrency;
    worker_thread_count_ = embb_core_set_count(
      &attr.GetInternal().core_affinity);
  }

  ~Node() {
    if (needs_finalize_) {
      mtapi_status_t status;
      mtapi_finalize(&status);
      internal::CheckStatus(status);
    }
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
      results, results_size, attributes, MTAPI_GROUP_NONE,
      &status);
    internal::CheckStatus(status);
    return Task(task_hndl);
  }

  static embb::mtapi::Node * node_instance_;

  mtapi_uint_t core_count_;
  mtapi_uint_t worker_thread_count_;
  bool needs_finalize_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_NODE_H_
