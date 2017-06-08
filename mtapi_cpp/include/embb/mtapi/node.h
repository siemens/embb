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

#ifndef EMBB_MTAPI_NODE_H_
#define EMBB_MTAPI_NODE_H_

#include <embb/base/memory_allocation.h>
#include <embb/base/function.h>
#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/c/mtapi_ext.h>
#include <embb/mtapi/execution_policy.h>
#include <embb/mtapi/status_exception.h>
#include <embb/mtapi/node_attributes.h>
#include <embb/mtapi/group.h>
#include <embb/mtapi/queue.h>
#include <embb/mtapi/task.h>
#include <embb/mtapi/task_attributes.h>
#include <embb/mtapi/job.h>
#include <embb/mtapi/action.h>
#include <embb/mtapi/task_context.h>

#ifdef GetJob
#undef GetJob
#endif

#ifdef MTAPI_CPP_AUTOMATIC_INITIALIZE
#define MTAPI_CPP_AUTOMATIC_DOMAIN_ID 1
#define MTAPI_CPP_AUTOMATIC_NODE_ID 1
#endif
#define EMBB_MTAPI_FUNCTION_JOB_ID 2

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
   * Function type for simple SMP interface.
   */
  typedef embb::base::Function<void, TaskContext &> SMPFunction;

  /**
   * Destroys the runtime singleton.
   * \notthreadsafe
   */
  ~Node() {
    function_action_.Delete();
    mtapi_finalize(MTAPI_NULL);
  }

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
  );

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
    );

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
  static Node & GetInstance();

  /**
   * Shuts the runtime system down.
   * \throws ErrorException if the singleton is not initialized.
   * \notthreadsafe
   */
  static void Finalize();

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
   * Returns the number of available queues.
   * \return The number of available queues
   * \waitfree
   */
  mtapi_uint_t GetQueueCount() const {
    return queue_count_;
  }

  /**
   * Returns the number of available groups.
   * \return The number of available groups
   * \waitfree
   */
  mtapi_uint_t GetGroupCount() const {
    return group_count_;
  }

  /**
    * Returns the number of available tasks.
    * \return The number of available tasks
    * \waitfree
    */
  mtapi_uint_t GetTaskLimit() const {
    return task_limit_;
  }

  /**
   * Starts a new Task.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  Task Start(
    SMPFunction const & func           /**< Function to use for the task. */
    ) {
    Job job = GetJob(EMBB_MTAPI_FUNCTION_JOB_ID);
    void * res = NULL;
    return Start(
      job, embb::base::Allocation::New<SMPFunction>(func), res);
  }

  /**
   * Starts a new Task with a given affinity and priority.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  Task Start(
    SMPFunction const & func,          /**< Function to use for the task. */
    ExecutionPolicy const & policy     /**< Affinity and priority of the
                                            task. */
  ) {
    Job job = GetJob(EMBB_MTAPI_FUNCTION_JOB_ID);
    void * res = NULL;
    TaskAttributes task_attr;
    task_attr.SetPolicy(policy);
    return Start(
      job, embb::base::Allocation::New<SMPFunction>(func), res, task_attr);
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

  /**
   * Retrieves a handle to the Job identified by \c job_id within the domain
   * of the local Node.
   *
   * \returns The handle to the requested Job.
   * \waitfree
   */
  Job GetJob(
    mtapi_job_id_t job_id              /**< [in] The id of the job */
    ) {
    return Job(job_id, domain_id_);
  }

  /**
   * Retrieves a handle to the Job identified by \c job_id and \c domain_id.
   *
   * \returns The handle to the requested Job.
   * \waitfree
   */
  Job GetJob(
    mtapi_job_id_t job_id,             /**< [in] The id of the job */
    mtapi_domain_t domain_id           /**< [in] The domain id to use */
    ) {
    return Job(job_id, domain_id);
  }

  /**
   * Constructs an Action.
   *
   * \returns The handle to the new Action.
   * \lockfree
   */
  Action CreateAction(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func,      /**< The action function */
    const void * node_local_data,      /**< Node local data available to all
                                       Tasks using this Action */
    mtapi_size_t node_local_data_size, /**< Size of node local data */
    ActionAttributes const & attributes
                                       /**< Attributes of the Action */
    ) {
    return Action(job_id, func, node_local_data, node_local_data_size,
      &attributes.GetInternal());
  }

  /**
   * Constructs an Action.
   *
   * \returns The handle to the new Action.
   * \lockfree
   */
  Action CreateAction(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func,      /**< The action function */
    const void * node_local_data,      /**< Node local data available to all
                                       Tasks using this Action */
    mtapi_size_t node_local_data_size  /**< Size of node local data */
    ) {
    return Action(job_id, func, node_local_data, node_local_data_size,
      MTAPI_DEFAULT_ACTION_ATTRIBUTES);
  }

  /**
   * Constructs an Action.
   *
   * \returns The handle to the new Action.
   * \lockfree
   */
  Action CreateAction(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func,      /**< The action function */
    ActionAttributes const & attributes
                                       /**< Attributes of the Action */
    ) {
    return Action(job_id, func, MTAPI_NULL, 0, &attributes.GetInternal());
  }

  /**
   * Constructs an Action.
   *
   * \returns The handle to the new Action.
   * \lockfree
   */
  Action CreateAction(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func       /**< The action function */
    ) {
    return Action(job_id, func, MTAPI_NULL, 0, MTAPI_DEFAULT_ACTION_ATTRIBUTES);
  }

  /**
   * Constructs a Group object with default attributes.
   *
   * \returns The handle to the new Group.
   * \lockfree
   */
  Group CreateGroup() {
    return Group(MTAPI_GROUP_ID_NONE, MTAPI_DEFAULT_GROUP_ATTRIBUTES);
  }

  /**
   * Constructs a Group object with default attributes and the given ID.
   *
   * \returns The handle to the new Group.
   * \lockfree
   */
  Group CreateGroup(
    mtapi_group_id_t id                /**< A user defined ID of the Group. */
    ) {
    return Group(id, MTAPI_DEFAULT_GROUP_ATTRIBUTES);
  }

  /**
   * Constructs a Group object using the given Attributes.
   *
   * \returns The handle to the new Group.
   * \lockfree
   */
  Group CreateGroup(
    GroupAttributes const & group_attr /**< The GroupAttributes to use. */
    ) {
    return Group(MTAPI_GROUP_ID_NONE, &group_attr.GetInternal());
  }

  /**
   * Constructs a Group object with given attributes and ID.
   *
   * \returns The handle to the new Group.
   * \lockfree
   */
  Group CreateGroup(
    mtapi_group_id_t id,               /**< A user defined ID of the Group. */
    GroupAttributes const & group_attr /**< The GroupAttributes to use. */
    ) {
    return Group(id, &group_attr.GetInternal());
  }

  /**
   * Constructs a Queue with the given Job and default attributes.
   *
   * \returns The handle to the new Queue.
   * \lockfree
   */
  Queue CreateQueue(
    Job & job                          /**< The Job to use for the Queue. */
    ) {
    return Queue(MTAPI_QUEUE_ID_NONE, job, MTAPI_DEFAULT_QUEUE_ATTRIBUTES);
  }

  /**
   * Constructs a Queue with the given Job and QueueAttributes.
   *
   * \returns The handle to the new Queue.
   * \lockfree
   */
  Queue CreateQueue(
    Job const & job,                   /**< The Job to use for the Queue. */
    QueueAttributes const & attr       /**< The attributes to use. */
    ) {
    return Queue(MTAPI_QUEUE_ID_NONE, job, &attr.GetInternal());
  }

  friend class embb::base::Allocation;

  /**
   * Starts a new Task.
   *
   * \returns The handle to the started Task.
   * \threadsafe
   */
  Task Start(
    mtapi_task_id_t task_id,           /**< A user defined ID of the Task. */
    mtapi_job_hndl_t job,              /**< The Job to execute. */
    const void * arguments,            /**< Pointer to the arguments buffer */
    mtapi_size_t arguments_size,       /**< Size of the arguments buffer */
    void * results,                    /**< Pointer to the result buffer */
    mtapi_size_t results_size,         /**< Size of the result buffer */
    mtapi_task_attributes_t const * attributes
                                       /**< Attributes to use for the task */
    ) {
    mtapi_status_t status;
    mtapi_task_hndl_t task_hndl =
      mtapi_task_start(task_id, job, arguments, arguments_size,
      results, results_size, attributes, MTAPI_GROUP_NONE,
      &status);
    internal::CheckStatus(status);
    return Task(task_hndl);
  }

  /**
   * This function yields execution to the MTAPI scheduler for at most one task.
   * \threadsafe
   */
  void YieldToScheduler() {
    mtapi_ext_yield();
  }

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
    queue_count_ = attr.GetInternal().max_queues;
    group_count_ = attr.GetInternal().max_groups;
    task_limit_ = attr.GetInternal().max_tasks;
    mtapi_initialize(domain_id, node_id, &attr.GetInternal(), &info, &status);
    internal::CheckStatus(status);

    core_count_ = info.hardware_concurrency;
    worker_thread_count_ = embb_core_set_count(
      &attr.GetInternal().core_affinity);

    domain_id_ = domain_id;

    function_action_ =
      CreateAction(EMBB_MTAPI_FUNCTION_JOB_ID, ActionFunction);
  }

  static void ActionFunction(
    const void* args,
    mtapi_size_t /*args_size*/,
    void* /*result_buffer*/,
    mtapi_size_t /*result_buffer_size*/,
    const void* /*node_local_data*/,
    mtapi_size_t /*node_local_data_size*/,
    mtapi_task_context_t * context) {
    TaskContext task_context(context);
    embb::base::Function<void, TaskContext &> * func =
      reinterpret_cast<embb::base::Function<void, TaskContext &>*>(
        const_cast<void*>(args));
    (*func)(task_context);
    embb::base::Allocation::Delete(func);
  }

  static embb::mtapi::Node * node_instance_;

  mtapi_domain_t domain_id_;
  mtapi_uint_t core_count_;
  mtapi_uint_t worker_thread_count_;
  mtapi_uint_t queue_count_;
  mtapi_uint_t group_count_;
  mtapi_uint_t task_limit_;
  Action function_action_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_NODE_H_
