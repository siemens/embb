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

#ifndef EMBB_TASKS_NODE_H_
#define EMBB_TASKS_NODE_H_

#include <list>
#include <embb/base/core_set.h>
#include <embb/mtapi/c/mtapi.h>
#include <embb/tasks/action.h>
#include <embb/tasks/task.h>
#include <embb/tasks/continuation.h>
#include <embb/tasks/group.h>
#include <embb/tasks/queue.h>

namespace embb {

namespace base {

class Allocation;

} // namespace base

namespace tasks {

/**
  * A singleton representing the MTAPI runtime.
  *
  * \ingroup CPP_TASKS
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
    embb::base::CoreSet const & core_set,
                                       /**< [in] A set of cores MTAPI should
                                            use for its worker threads */
    mtapi_uint_t max_tasks,            /**< [in] Maximum number of concurrent
                                            \link Task Tasks \endlink */
    mtapi_uint_t max_groups,           /**< [in] Maximum number of concurrent
                                            \link Group Groups \endlink */
    mtapi_uint_t max_queues,           /**< [in] Maximum number of concurrent
                                            \link Queue Queues \endlink */
    mtapi_uint_t queue_limit,          /**< [in] Maximum Queue capacity */
    mtapi_uint_t max_priorities        /**< [in] Maximum number of priorities,
                                            priorities will be between 0 and
                                            max_priorities-1 */
    );

  /**
    * Checks if runtime is initialized.
    * \return \c true if the Node singleton is already initialized, false
    *         otherwise
    * \waitfree
    */
  static bool IsInitialized();

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
    * Creates a Group to launch \link Task Tasks \endlink in.
    * \return A reference to the created Group
    * \throws ErrorException if the Group object could not be constructed.
    * \threadsafe
    * \memory Allocates some memory depending on the configuration of the
    *         runtime.
    */
  Group & CreateGroup();

  /**
    * Destroys a Group. \link Task Tasks \endlink running in the Group will
    * finish execution.
    * \threadsafe
    */
  void DestroyGroup(
    Group & group                      /**< [in,out] The Group to destroy */
    );

  /**
    * Creates a Queue for stream processing. The queue might execute its 
    * \link Task Tasks \endlink either in order or unordered.
    * \return A reference to the new Queue
    * \throws ErrorException if the Queue object could not be constructed.
    * \threadsafe
    * \memory Allocates some memory depending on the configuration of the
    *         runtime.
    */
  Queue & CreateQueue(
    mtapi_uint_t priority,             /**< [in] Priority of the Queue */
    bool ordered                       /**< [in] \c true if the Queue should be
                                            ordered, otherwise \c false */
    );

  /**
    * Destroys a Queue. Running \link Task Tasks \endlink will be canceled.
    * \threadsafe
    */
  void DestroyQueue(
    Queue & queue                      /**< [in,out] The Queue to destroy */
    );

  /**
    * Runs an Action.
    * \return A Task identifying the Action to run
    * \throws ErrorException if the Task object could not be constructed.
    * \threadsafe
    */
  Task Spawn(
    Action action                      /**< [in] The Action to execute */
    );

  /**
    * Creates a Continuation.
    * \return A Continuation chain
    * \threadsafe
    */
  Continuation First(
    Action action                      /**< [in] The first Action of the
                                            Continuation chain */
    );

  friend class embb::base::Allocation;

 private:
  Node(Node const & node);
  Node(
    mtapi_domain_t domain_id,
    mtapi_node_t node_id,
    mtapi_node_attributes_t * attr);
  ~Node();

  static void action_func(
    const void* args,
    mtapi_size_t args_size,
    void* result_buffer,
    mtapi_size_t result_buffer_size,
    const void* node_local_data,
    mtapi_size_t node_local_data_size,
    mtapi_task_context_t * context);

  mtapi_uint_t core_count_;
  mtapi_uint_t worker_thread_count_;
  mtapi_action_hndl_t action_handle_;
  std::list<Queue*> queues_;
  std::list<Group*> groups_;
};

} // namespace tasks
} // namespace embb

#endif // EMBB_TASKS_NODE_H_
