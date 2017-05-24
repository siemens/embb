/*
 * MTAPI header contains the public MTAPI API and data type definitions.
 *
 * This file defines the MTAPI API. it has to be included by any application
 * using MTAPI.
 *
 * \copyright
 * Copyright (c) 2012, The Multicore Association.
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * (1) Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * (3) Neither the name of the Multicore Association nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * \note
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_MTAPI_C_MTAPI_H_
#define EMBB_MTAPI_C_MTAPI_H_

/**
 * \defgroup C_MTAPI MTAPI
 * \ingroup C
 *
 * Multicore Task Management API (MTAPI&reg;).
 *
 * MTAPI is an API standardized by the 
 * <a href="http://www.multicore-association.org">Multicore Association</a>
 * for leveraging task parallelism on a wide range of embedded devices
 * containing symmetric or asymmetric multicore processors.
 * A description of the basic terms and concepts is given below. More
 * information can be found on the website of the
 * <a href="http://www.multicore-association.org/workgroup/mtapi.php">Multicore
   Task Management Working Group</a>.
 *
 * ## Definitions
 *
 * <table>
 *   <tr>
 *     <td>Action</td>
 *     <td>An action is the hardware or software implementation of a job. An
 *         action implemented in software consists of the implementation of an
 *         action function with a predefined signature. Software actions are
 *         registered with the MTAPI runtime and associated with a job. While
 *         executing, an action is also associated with a task and task context.
 *         Hardware implementations of actions must be known a priori in the
 *         MTAPI runtime implementation. There is no standardized way of
 *         registering hardware actions because they are highly
 *         hardware-dependent. Hardware and software actions are referenced by
 *         handles or indirectly through job IDs and job handles.</td>
 *   </tr>
 *   <tr>
 *     <td>Action Function</td>
 *     <td>The executable function of an action, invoked by the
 *         MTAPI runtime when a task is started.</td>
 *   </tr>
 *   <tr>
 *     <td>Affinity</td>
 *     <td>Defines which cores can execute a given action function.</td>
 *   </tr>
 *   <tr>
 *     <td>Blocking</td>
 *     <td>A blocking function does not return until the function completes
 *         successfully or returns with an error.</td>
 *   </tr>
 *   <tr>
 *     <td>Core</td>
 *     <td>A core is an undividable processing element. Two cores can share
 *         resources such as memory or ALUs for hyperthreaded cores. The core
 *         notion is necessary for core affinity, but is
 *         implementation-specific.</td>
 *   </tr>
 *   <tr>
 *     <td>Domain</td>
 *     <td>An implementation of MTAPI includes one or more domains, each with
 *         one or more nodes. The concept of domains is consistent in all
 *         Multicore Association APIs. A domain is comparable to a subnet in a
 *         network or a namespace for unique names and IDs. Domains are
 *         supported by a runtime.</td>
 *   </tr>
 *   <tr>
 *     <td>Handle</td>
 *     <td>An abstract reference to an object on the same node or to an object
 *         managed by another node. A handle is valid only on the node on which
 *         it was requested and generated. A handle is opaque, that is, its
 *         underlying representation is implementation-defined. Handles can be
 *         copied, assigned, and passed as arguments, but the application
 *         should make no other assumptions about the type, representation, or
 *         contents of a handle.</td>
 *   </tr>
 *   <tr>
 *     <td>Job</td>
 *     <td>A job provides a way to reference one or more actions. Jobs are
 *         abstractions of the processing implemented in hardware or software
 *         by actions. Multiple actions can implement the same job based on
 *         different hardware resources (for instance a job can be implemented
 *         by one action on a DSP and by another action on a general purpose
 *         core, or a job can be implemented by both hardware and software
 *         actions). Each job is represented by a domain-wide job ID, or by a
 *         job handle local to a node.</td>
 *   </tr>
 *   <tr>
 *     <td>MCA</td>
 *     <td>The Multicore Association.</td>
 *   </tr>
 *   <tr>
 *     <td>MTAPI</td>
 *     <td>Multicore Task Management API, defined by The Multicore Association.
 *   </td>
 *   </tr>
 *   <tr>
 *     <td>Node</td>
 *     <td>A node represents an independent unit of execution that maps to a
 *         process, thread, thread pool, instance of an operating system,
 *         hardware accelerator, processor core, a cluster of processor cores,
 *         or other abstract processing entity with an independent program
 *         counter. Each node can belong to only one domain. The concept of
 *         nodes is consistent in all Multicore Associations APIs. Code
 *         executed on an MTAPI node shares memory (data) with any other code
 *         executed on the same node.</td>
 *   </tr>
 *   <tr>
 *     <td>Queue</td>
 *     <td>A software or hardware entity in which tasks are enqueued in a
 *         given order. The queue can ensure in-order execution of tasks.
 *         Furthermore, queues might implement other scheduling policies that
 *         can be configured by setting queue attributes.</td>
 *   </tr>
 *   <tr>
 *     <td>Reference</td>
 *     <td>A reference exists when an object or abstract entity has knowledge
 *         or access to another object, without regard to the specific means of
 *         the implementation.</td>
 *   </tr>
 *   <tr>
 *     <td>Resource</td>
 *     <td>A processing core or chip, hardware accelerator, memory region, or
 *         I/O.</td>
 *   </tr>
 *   <tr>
 *     <td>Remote Memory</td>
 *     <td>Memory that cannot be accessed using standard load and store
 *         operations. For example, host memory is remote to a GPU core.</td>
 *   </tr>
 *   <tr>
 *     <td>Runtime System</td>
 *     <td>An MTAPI runtime system (or "runtime") is the underlying
 *         implementation of MTAPI. The core of the runtime system supports
 *         task scheduling and communication with other nodes. Each MTAPI has
 *         an MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>SMP</td>
 *     <td>SMP is short for symmetric multiprocessing, in which two or more
 *         identical processing cores are connected to a shared main memory
 *         and are controlled by a single OS instance.</td>
 *   </tr>
 *   <tr>
 *     <td>Task</td>
 *     <td>A task is the invocation of an action. A task is associated with a
 *         job object, which is associated with one or more actions. A task
 *         may optionally be associated with a task group. A task has
 *         attributes and an internal state. A task begins its lifetime with a
 *         call to mtapi_task_start() or mtapi_task_enqueue(). A task is
 *         referenced by a handle of type mtapi_task_hndl_t. After a task has
 *         started, it is possible to wait for task completion from other
 *         parts of the program. Every task can run exactly once, i.e., the
 *         task cannot be started a second time. (Note that in other contexts,
 *         the term "task" has a different meaning. Some real-time operating
 *         systems use "task" for operating system threads, for example.)</td>
 *   </tr>
 *   <tr>
 *     <td>Task Context</td>
 *     <td>Information about the task, accessible by the corresponding action
 *         function; useful for action code reflection.</td>
 *   </tr>
 * </table>
 *
 * ## The MTAPI Feature Set
 *
 * MTAPI supports two programming modes derived from use cases of the working
 * group members:
 *  - __Tasks__<br>
 *    MTAPI allows a programmer to start tasks and to synchronize on task
 *    completion. Tasks are executed by the runtime system, concurrently to
 *    other tasks that have been started and have not been completed at that
 *    point in time. A task can be implemented by software or by hardware.
 *    Tasks can be started from remote nodes, i.e., the implementation can be
 *    done on one node, but the starting and synchronization of corresponding
 *    tasks can be done on other nodes. The developer decides where to deploy
 *    a task implementation. On the executing node, the runtime system selects
 *    the cores that execute a particular task. This mapping can be influenced
 *    by application-specific attributes. Tasks can start sub-tasks. MTAPI
 *    provides a basic mechanism to pass data to the node that executes a
 *    task, and back to the calling node.
 *  - __Queues__<br>
 *    Explicit queues can be used to control the task scheduling policies for
 *    related tasks. Order-preserving queues ensure that tasks are executed
 *    sequentially in queue order with no subsequent task starting until the
 *    previous one is complete. MTAPI also supports non-order-preserving
 *    queues, allowing control of the scheduling policies of tasks started via
 *    the same queue (queues may offer implementation specific scheduling
 *    policies controlled by implementation specific queue attributes). Even
 *    hardware queues can be associated with queue objects.
 *
 * MTAPI also supports the following types of tasks:
 *  - __Single tasks__<br>
 *    Single tasks are the standard case: After a task is started, the
 *    application may wait for completion of the task at a later point in
 *    time. In some cases the application waits for completion of a group of
 *    tasks. In other cases waiting is not required at all. When a
 *    software-implemented task is started, the corresponding code (action
 *    function) is executed once by the MTAPI runtime environment. When a
 *    hardware-implemented task is started, the task execution is triggered
 *    once by the MTAPI runtime system.
 *  - __Multi-instance tasks__<br>
 *    Multi-instance tasks execute the same action multiple times in parallel
 *    (similar to parallel regions in OpenMP or parallel MPI processes).
 *  - __Multiple-implementation tasks / load balancing__<br>
 *    In heterogeneous systems, there could be implementations of the same job
 *    for different types of processor cores, e.g., one general purpose
 *    implementation and a second one for a hardware accelerator. MTAPI allows
 *    attaching multiple actions to a job. The runtime system shall decide
 *    dynamically during runtime, depending on the system load, which action to
 *    utilize. Only one of the alternative actions will be executed.
 */

#include <stdint.h>
#include <embb/base/c/core_set.h>
#include <embb/base/c/thread.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- MCA ORGANIZATION IDS ----------------------------------------------- */

#define MCA_ORG_ID_PSI 0 /* PolyCore Software, Inc. */
#define MCA_ORG_ID_FSL 1 /* Freescale, Inc. */
#define MCA_ORG_ID_MGC 2 /* Mentor Graphics, Corp. */
#define MCA_ORG_ID_ADI 3 /* Analog Devices */
#define MCA_ORG_ID_SIE 4 /* Siemens */
#define MCA_ORG_ID_EMB 5 /* EMB2 project */
#define MCA_ORG_ID_TBD 6 /* TBD */


/* ---- BASIC DEFINITIONS -------------------------------------------------- */

/** marks input parameters */
#define MTAPI_IN const
/** marks output parameters */
#define MTAPI_OUT
/** marks in/out parameters */
#define MTAPI_INOUT


/* ---- BASIC DATA TYPES --------------------------------------------------- */

/* MCA type definitions */
typedef int             mca_int_t;
typedef int8_t          mca_int8_t;
typedef int16_t         mca_int16_t;
typedef int32_t         mca_int32_t;
typedef int64_t         mca_int64_t;

typedef unsigned int    mca_uint_t;
typedef uint8_t         mca_uint8_t;
typedef uint16_t        mca_uint16_t;
typedef uint32_t        mca_uint32_t;
typedef uint64_t        mca_uint64_t;

typedef unsigned char   mca_boolean_t;

typedef unsigned int    mca_domain_t;
typedef unsigned int    mca_node_t;
typedef unsigned int    mca_status_t;
typedef int             mca_timeout_t;

/* the MTAPI data types */
typedef mca_int_t       mtapi_int_t;
typedef mca_int8_t      mtapi_int8_t;
typedef mca_int16_t     mtapi_int16_t;
typedef mca_int32_t     mtapi_int32_t;
typedef mca_int64_t     mtapi_int64_t;

typedef mca_uint_t      mtapi_uint_t;
typedef mca_uint8_t     mtapi_uint8_t;
typedef mca_uint16_t    mtapi_uint16_t;
typedef mca_uint32_t    mtapi_uint32_t;
typedef mca_uint64_t    mtapi_uint64_t;

typedef mca_domain_t    mtapi_domain_t;
typedef mca_node_t      mtapi_node_t;
typedef mca_timeout_t   mtapi_timeout_t;

typedef mca_boolean_t mtapi_boolean_t;
typedef mtapi_uint_t  mtapi_size_t;

/**
 * Info structure.
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
struct mtapi_info_struct {
  mtapi_uint_t mtapi_version;          /**< The three last (rightmost) hex
                                            digits are the minor number, and
                                            those left of the minor number are
                                            the major number. */
  mtapi_uint_t organization_id;        /**< Implementation vendor or
                                            organization ID. */
  mtapi_uint_t implementation_version; /**< The three last (rightmost) hex
                                            digits are the minor number, and
                                            those left of the minor number are
                                            the major number.*/
  mtapi_uint_t number_of_domains;      /**< Number of domains allowed by the
                                            implementation.*/
  mtapi_uint_t number_of_nodes;        /**< Number of nodes allowed by the
                                            implementation.*/
  mtapi_uint_t hardware_concurrency;   /**< Number of CPU cores available. */
  mtapi_uint_t used_memory;            /**< Bytes of memory used by MTAPI. */
};

/**
 * Info type.
 * \memberof mtapi_info_struct
 */
typedef struct mtapi_info_struct mtapi_info_t;

/**
 * Core affinity type.
 * \ingroup CORE_AFFINITY_MASKS
 */
typedef mtapi_uint64_t mtapi_affinity_t;


/* ---- BASIC enumerations ------------------------------------------------- */

/**
 * Status codes returned in the status parameter of the MTAPI interface
 * functions.
 */
enum mtapi_status_enum {
  /* generic */
  MTAPI_SUCCESS,                       /**< success, no error */
  MTAPI_TIMEOUT,                       /**< timeout was reached */
  MTAPI_ERR_PARAMETER,                 /**< invalid parameter */
  MTAPI_ERR_ATTR_READONLY,             /**< tried to write a read-only
                                            attribute */
  MTAPI_ERR_ATTR_NUM,                  /**< invalid attribute number */
  MTAPI_ERR_ATTR_SIZE,                 /**< invalid attribute size */

  /* node specific */
  MTAPI_ERR_NODE_INITFAILED,           /**< general error in node
                                            initialization */
  MTAPI_ERR_NODE_INITIALIZED,          /**< \a mtapi_initialize called for a
                                            node that already had been
                                            initialized */
  MTAPI_ERR_NODE_INVALID,              /**< The node id is not valid */
  MTAPI_ERR_DOMAIN_INVALID,            /**< the domain id is not valid */
  MTAPI_ERR_NODE_NOTINIT,              /**< the node is not initialized */

  /* action specific */
  MTAPI_ERR_ACTION_INVALID,            /**< The action id is not a valid action
                                            id, i.e., no action was crated for
                                            that ID or the action has been
                                            deleted. */
  MTAPI_ERR_ACTION_EXISTS,             /**< mtapi_action_create called with an
                                            ID of an action that already had
                                            been created */
  MTAPI_ERR_ACTION_LIMIT,              /**< exceeded maximum number of actions
                                            allowed */
  MTAPI_ERR_ACTION_NUM_INVALID,        /**< The number of actions passed to
                                            mtapi_task_start or
                                            mtapi_queue_create is lower than
                                            1. */
  MTAPI_ERR_ACTION_FAILED,             /**< status that can be passed to the
                                            runtime by
                                            \a mtapi_context_status_set if the
                                            task could not be completed as
                                            intended */
  MTAPI_ERR_ACTION_CANCELLED,          /**< status that can be passed to the
                                            runtime by
                                            \a mtapi_context_status_set if the
                                            task execution is canceled */
  MTAPI_ERR_ACTION_DELETED,            /**< All actions associated with the
                                            task have been deleted before the
                                            execution of the task was started
                                            or the error code has been set in
                                            the action code to
                                            MTAPI_ERR_ACTION_DELETED by
                                            \a mtapi_action_result_set. */
  MTAPI_ERR_ACTION_DISABLED,           /**< All actions associated with the
                                            task have been disabled before the
                                            execution of the task was started
                                            or the error code has been
                                            set in the action code to
                                            MTAPI_ERR_ACTION_DISABLED by
                                            \a mtapi_action_result_set. */

  /* context specific */
  MTAPI_ERR_CONTEXT_INVALID,           /**< returned if the context found is
                                            in an invalid state */
  MTAPI_ERR_CONTEXT_OUTOFCONTEXT,      /**< returned if action code is not
                                            called in the context of a task
                                            execution. This function must be
                                            used in an action function only.
                                            The action function must be called
                                            from the MTAPI runtime system */

  /* task specific */
  MTAPI_ERR_TASK_INVALID,              /**< invalid task handle encountered */
  MTAPI_ERR_TASK_LIMIT,                /**< exceeded maximum number of tasks
                                            allowed */

  /* job specific */
  MTAPI_ERR_JOB_INVALID,               /**< invalid job handle or job ID */

  /* queue specific */
  MTAPI_ERR_QUEUE_INVALID,             /**< argument is not a valid queue
                                            handle or ID */
  MTAPI_ERR_QUEUE_DELETED,             /**< a deleted queue is passed as an
                                            argument */
  MTAPI_ERR_QUEUE_DISABLED,            /**< a disable queue is passed as an
                                            argument */
  MTAPI_ERR_QUEUE_LIMIT,               /**< exceeded maximum number of queues
                                            allowed */

  /* group specific */
  MTAPI_ERR_GROUP_INVALID,             /**< invalid group handle encountered */
  MTAPI_ERR_GROUP_LIMIT,               /**< exceeded maximum number of groups
                                            allowed */
  MTAPI_GROUP_COMPLETED,               /**< group completed, i.e., there are no
                                            more tasks to wait for in the group
                                            when waiting with
                                            \a mtapi_group_wait_any */

  /* others */
  MTAPI_ERR_UNKNOWN,                   /**< unknown error */
  MTAPI_ERR_BUFFER_SIZE,               /**< buffer size mismatch */
  MTAPI_ERR_RESULT_SIZE,               /**< result buffer size mismatch
                                            (e.g., in \a mtapi_task_wait) */
  MTAPI_ERR_ARG_SIZE,                  /**< invalid argument size */
  MTAPI_ERR_WAIT_PENDING,              /**< mtapi_*_wait called twice on a
                                            group or task which has not
                                            finished */

  /* unsupported functions */
  MTAPI_ERR_FUNC_NOT_IMPLEMENTED,      /**< The MTAPI function called is not
                                            implemented by the runtime
                                            system. */
  MTAPI_ERR_ARG_NOT_IMPLEMENTED,       /**< The MTAPI function called is
                                            implemented by the runtime, but it
                                            does not support the arguments
                                            passed. */

  /* features that may be not supported by some implementations */
  MTAPI_ERR_RUNTIME_REMOTETASKS_NOTSUPPORTED,
                                       /**< The Runtime system does not support
                                            remote tasks. This allows lighter
                                            implementations for shared memory
                                            environments. */
  MTAPI_ERR_RUNTIME_LOADBALANCING_NOTSUPPORTED,
                                       /**< This error is returned when more
                                            than one action is passed to
                                            mtapi_task_start or
                                            mtapi_queue_create and if the
                                            runtime system does not implement
                                            load balancing between nodes. This
                                            allows light MTAPI implementation
                                            for systems not having the
                                            requirement for inter-node
                                            load-balancing */

  /* core affinity specific */
  MTAPI_ERR_CORE_NUM,                  /**< This error occurs when trying to
                                            set an affinity to a non-existing
                                            core */
};
typedef enum mtapi_status_enum mtapi_status_t;
                                       /**< defines the MTAPI state codes */


/**
 * Task states.
 */
enum mtapi_task_state_enum {
  MTAPI_TASK_INTENTIONALLY_UNUSED,     /**< never used */
  MTAPI_TASK_ERROR,                    /**< indicates internal error */
  MTAPI_TASK_PRENATAL,                 /**< initialization value for newly
                                            allocated task descriptor */
  MTAPI_TASK_CREATED,                  /**< task is fully initialized */
  MTAPI_TASK_SCHEDULED,                /**< task was scheduled */
  MTAPI_TASK_RUNNING,                  /**< task is running */
  MTAPI_TASK_WAITING,                  /**< task is being waited for */
  MTAPI_TASK_RETAINED,                 /**< task was started via a queue and
                                            is retained */
  MTAPI_TASK_DELETED,                  /**< task was deleted */
  MTAPI_TASK_CANCELLED,                /**< \a MTAPI_TASK_CANCELLED is the only
                                            value specified by the MTAPI
                                            specification, the others are
                                            implementation specific and can be
                                            used for debugging purposes. */
  MTAPI_TASK_COMPLETED                 /**< task completed execution
                                            successfully */
};
typedef enum mtapi_task_state_enum mtapi_task_state_t;
                                       /**< internal task state */

/**
 * Notification types for the runtime.
 */
enum mtapi_notification_enum {
  MTAPI_NOTIF_PREFETCH,                /**< implementation specific example */
  MTAPI_NOTIF_EXECUTE_NEXT             /**< implementation specific example */
};
typedef enum mtapi_notification_enum mtapi_notification_t;
                                       /**< runtime notification */

/**
 * Enum to select default or specific worker for priority setter
 */
enum mtapi_worker_priority_type_enum {
  MTAPI_WORKER_PRIORITY_END = 0,
  MTAPI_WORKER_PRIORITY_DEFAULT = 1,
  MTAPI_WORKER_PRIORITY_WORKER = 100,
  MTAPI_WORKER_PRIORITY_WORKER_0 = MTAPI_WORKER_PRIORITY_WORKER + 0,
  MTAPI_WORKER_PRIORITY_WORKER_1 = MTAPI_WORKER_PRIORITY_WORKER + 1,
  MTAPI_WORKER_PRIORITY_WORKER_2 = MTAPI_WORKER_PRIORITY_WORKER + 2,
  MTAPI_WORKER_PRIORITY_WORKER_3 = MTAPI_WORKER_PRIORITY_WORKER + 3,
  MTAPI_WORKER_PRIORITY_WORKER_4 = MTAPI_WORKER_PRIORITY_WORKER + 4,
  MTAPI_WORKER_PRIORITY_WORKER_5 = MTAPI_WORKER_PRIORITY_WORKER + 5,
  MTAPI_WORKER_PRIORITY_WORKER_6 = MTAPI_WORKER_PRIORITY_WORKER + 6,
  MTAPI_WORKER_PRIORITY_WORKER_7 = MTAPI_WORKER_PRIORITY_WORKER + 7,
  MTAPI_WORKER_PRIORITY_WORKER_8 = MTAPI_WORKER_PRIORITY_WORKER + 8,
  MTAPI_WORKER_PRIORITY_WORKER_9 = MTAPI_WORKER_PRIORITY_WORKER + 9,
  MTAPI_WORKER_PRIORITY_WORKER_10 = MTAPI_WORKER_PRIORITY_WORKER + 10,
  MTAPI_WORKER_PRIORITY_WORKER_11 = MTAPI_WORKER_PRIORITY_WORKER + 11,
  MTAPI_WORKER_PRIORITY_WORKER_12 = MTAPI_WORKER_PRIORITY_WORKER + 12,
  MTAPI_WORKER_PRIORITY_WORKER_13 = MTAPI_WORKER_PRIORITY_WORKER + 13,
  MTAPI_WORKER_PRIORITY_WORKER_14 = MTAPI_WORKER_PRIORITY_WORKER + 14,
  MTAPI_WORKER_PRIORITY_WORKER_15 = MTAPI_WORKER_PRIORITY_WORKER + 15,
  MTAPI_WORKER_PRIORITY_WORKER_16 = MTAPI_WORKER_PRIORITY_WORKER + 16,
  MTAPI_WORKER_PRIORITY_WORKER_17 = MTAPI_WORKER_PRIORITY_WORKER + 17,
  MTAPI_WORKER_PRIORITY_WORKER_18 = MTAPI_WORKER_PRIORITY_WORKER + 18,
  MTAPI_WORKER_PRIORITY_WORKER_19 = MTAPI_WORKER_PRIORITY_WORKER + 19,
  MTAPI_WORKER_PRIORITY_WORKER_20 = MTAPI_WORKER_PRIORITY_WORKER + 20,
  MTAPI_WORKER_PRIORITY_WORKER_21 = MTAPI_WORKER_PRIORITY_WORKER + 21,
  MTAPI_WORKER_PRIORITY_WORKER_22 = MTAPI_WORKER_PRIORITY_WORKER + 22,
  MTAPI_WORKER_PRIORITY_WORKER_23 = MTAPI_WORKER_PRIORITY_WORKER + 23,
  MTAPI_WORKER_PRIORITY_WORKER_24 = MTAPI_WORKER_PRIORITY_WORKER + 24,
  MTAPI_WORKER_PRIORITY_WORKER_25 = MTAPI_WORKER_PRIORITY_WORKER + 25,
  MTAPI_WORKER_PRIORITY_WORKER_26 = MTAPI_WORKER_PRIORITY_WORKER + 26,
  MTAPI_WORKER_PRIORITY_WORKER_27 = MTAPI_WORKER_PRIORITY_WORKER + 27,
  MTAPI_WORKER_PRIORITY_WORKER_28 = MTAPI_WORKER_PRIORITY_WORKER + 28,
  MTAPI_WORKER_PRIORITY_WORKER_29 = MTAPI_WORKER_PRIORITY_WORKER + 29,
  MTAPI_WORKER_PRIORITY_WORKER_30 = MTAPI_WORKER_PRIORITY_WORKER + 30,
  MTAPI_WORKER_PRIORITY_WORKER_31 = MTAPI_WORKER_PRIORITY_WORKER + 31,
  MTAPI_WORKER_PRIORITY_WORKER_32 = MTAPI_WORKER_PRIORITY_WORKER + 32,
  MTAPI_WORKER_PRIORITY_WORKER_33 = MTAPI_WORKER_PRIORITY_WORKER + 33,
  MTAPI_WORKER_PRIORITY_WORKER_34 = MTAPI_WORKER_PRIORITY_WORKER + 34,
  MTAPI_WORKER_PRIORITY_WORKER_35 = MTAPI_WORKER_PRIORITY_WORKER + 35,
  MTAPI_WORKER_PRIORITY_WORKER_36 = MTAPI_WORKER_PRIORITY_WORKER + 36,
  MTAPI_WORKER_PRIORITY_WORKER_37 = MTAPI_WORKER_PRIORITY_WORKER + 37,
  MTAPI_WORKER_PRIORITY_WORKER_38 = MTAPI_WORKER_PRIORITY_WORKER + 38,
  MTAPI_WORKER_PRIORITY_WORKER_39 = MTAPI_WORKER_PRIORITY_WORKER + 39,
  MTAPI_WORKER_PRIORITY_WORKER_40 = MTAPI_WORKER_PRIORITY_WORKER + 40,
  MTAPI_WORKER_PRIORITY_WORKER_41 = MTAPI_WORKER_PRIORITY_WORKER + 41,
  MTAPI_WORKER_PRIORITY_WORKER_42 = MTAPI_WORKER_PRIORITY_WORKER + 42,
  MTAPI_WORKER_PRIORITY_WORKER_43 = MTAPI_WORKER_PRIORITY_WORKER + 43,
  MTAPI_WORKER_PRIORITY_WORKER_44 = MTAPI_WORKER_PRIORITY_WORKER + 44,
  MTAPI_WORKER_PRIORITY_WORKER_45 = MTAPI_WORKER_PRIORITY_WORKER + 45,
  MTAPI_WORKER_PRIORITY_WORKER_46 = MTAPI_WORKER_PRIORITY_WORKER + 46,
  MTAPI_WORKER_PRIORITY_WORKER_47 = MTAPI_WORKER_PRIORITY_WORKER + 47,
  MTAPI_WORKER_PRIORITY_WORKER_48 = MTAPI_WORKER_PRIORITY_WORKER + 48,
  MTAPI_WORKER_PRIORITY_WORKER_49 = MTAPI_WORKER_PRIORITY_WORKER + 49,
  MTAPI_WORKER_PRIORITY_WORKER_50 = MTAPI_WORKER_PRIORITY_WORKER + 50,
  MTAPI_WORKER_PRIORITY_WORKER_51 = MTAPI_WORKER_PRIORITY_WORKER + 51,
  MTAPI_WORKER_PRIORITY_WORKER_52 = MTAPI_WORKER_PRIORITY_WORKER + 52,
  MTAPI_WORKER_PRIORITY_WORKER_53 = MTAPI_WORKER_PRIORITY_WORKER + 53,
  MTAPI_WORKER_PRIORITY_WORKER_54 = MTAPI_WORKER_PRIORITY_WORKER + 54,
  MTAPI_WORKER_PRIORITY_WORKER_55 = MTAPI_WORKER_PRIORITY_WORKER + 55,
  MTAPI_WORKER_PRIORITY_WORKER_56 = MTAPI_WORKER_PRIORITY_WORKER + 56,
  MTAPI_WORKER_PRIORITY_WORKER_57 = MTAPI_WORKER_PRIORITY_WORKER + 57,
  MTAPI_WORKER_PRIORITY_WORKER_58 = MTAPI_WORKER_PRIORITY_WORKER + 58,
  MTAPI_WORKER_PRIORITY_WORKER_59 = MTAPI_WORKER_PRIORITY_WORKER + 59,
  MTAPI_WORKER_PRIORITY_WORKER_60 = MTAPI_WORKER_PRIORITY_WORKER + 60,
  MTAPI_WORKER_PRIORITY_WORKER_61 = MTAPI_WORKER_PRIORITY_WORKER + 61,
  MTAPI_WORKER_PRIORITY_WORKER_62 = MTAPI_WORKER_PRIORITY_WORKER + 62,
  MTAPI_WORKER_PRIORITY_WORKER_63 = MTAPI_WORKER_PRIORITY_WORKER + 63
};
/**
 * Enum to select default or specific worker for priority setter
 */
typedef enum mtapi_worker_priority_type_enum mtapi_worker_priority_type_t;

/**
 * Describes the default priority of all workers or the priority of a
 * specific worker.
 */
struct mtapi_worker_priority_entry_struct {
  mtapi_worker_priority_type_t type;   /**< default or specific worker */
  embb_thread_priority_t priority;     /**< priority to set */
};
/**
* Describes the default priority of all workers or the priority of a
* specific worker.
*/
typedef struct mtapi_worker_priority_entry_struct mtapi_worker_priority_entry_t;

/**
 * Node attributes, to be extended for implementation specific attributes
 */
enum mtapi_node_attributes_enum {
  MTAPI_NODE_CORE_AFFINITY,            /**< use the given cores only */
  MTAPI_NODE_NUMCORES,                 /**< number of cores provided by the
                                            node */
  MTAPI_NODE_TYPE,                     /**< the nodes type, SMP or DSP */
  MTAPI_NODE_MAX_TASKS,                /**< maximum number of tasks allowed by
                                            the node */
  MTAPI_NODE_MAX_ACTIONS,              /**< maximum number of actions allowed
                                            by the node */
  MTAPI_NODE_MAX_GROUPS,               /**< maximum number of groups allowed
                                            by the node */
  MTAPI_NODE_MAX_QUEUES,               /**< maximum number of queues allowed
                                            by the node */
  MTAPI_NODE_QUEUE_LIMIT,              /**< maximum number of elements that fit
                                            into a queue */
  MTAPI_NODE_MAX_JOBS,                 /**< maximum number of jobs allowed by
                                            the node */
  MTAPI_NODE_MAX_ACTIONS_PER_JOB,      /**< maximum number of actions in a job
                                            allowed by the node */
  MTAPI_NODE_MAX_PRIORITIES,           /**< maximum number of priorities
                                            allowed by the node */
  MTAPI_NODE_REUSE_MAIN_THREAD,        /**< reuse main thread as worker */
  MTAPI_NODE_WORKER_PRIORITIES         /**< set worker priorites */
};
/** size of the \a MTAPI_NODE_CORE_AFFINITY attribute */
#define MTAPI_NODE_CORE_AFFINITY_SIZE sizeof(embb_core_set_t)
/** size of the \a MTAPI_NODES_NUMCORES attribute */
#define MTAPI_NODE_NUMCORES_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_TYPE attribute */
#define MTAPI_NODE_TYPE_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_TASKS attribute */
#define MTAPI_NODE_MAX_TASKS_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_ACTIONS attribute */
#define MTAPI_NODE_MAX_ACTIONS_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_GROUPS attribute */
#define MTAPI_NODE_MAX_GROUPS_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_QUEUES attribute */
#define MTAPI_NODE_MAX_QUEUES_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_QUEUE_LIMIT attribute */
#define MTAPI_NODE_QUEUE_LIMIT_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_JOBS attribute */
#define MTAPI_NODE_MAX_JOBS_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_ACTIONS_PER_JOB attribute */
#define MTAPI_NODE_MAX_ACTIONS_PER_JOB_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_MAX_PRIORITIES attribute */
#define MTAPI_NODE_MAX_PRIORITIES_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_NODE_REUSE_MAIN_THREAD attribute */
#define MTAPI_NODE_REUSE_MAIN_THREAD_SIZE sizeof(mtapi_boolean_t)
/** size of the \a MTAPI_NODE_WORKER_PRIORITIES attribute */
#define MTAPI_NODE_WORKER_PRIORITIES_SIZE 0

/* example attribute value */
#define MTAPI_NODE_TYPE_SMP 1
#define MTAPI_NODE_TYPE_DSP 2

/**
 * Task handle type.
 * \memberof mtapi_task_hndl_struct
 */
typedef struct mtapi_task_hndl_struct mtapi_task_hndl_t;

/** task completion callback */
typedef void(*mtapi_task_complete_function_t)(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status);

/** task attributes */
enum mtapi_task_attributes_enum {
  MTAPI_TASK_DETACHED,                 /**< task is detached, i.e., the runtime
                                            system cared about deleting
                                            internal data structures
                                            representing the task; detached
                                            tasks cannot be accessed via task
                                            handles */
  MTAPI_TASK_INSTANCES,                /**< indicates how many parallel
                                            instances of task shall be started
                                            by MTAPI; the default case is that
                                            each task is executed exactly once,
                                            setting this value to \a n, the
                                            corresponding action code will be
                                            executed n times, if possible in
                                            parallel */
  MTAPI_TASK_PRIORITY,                 /**< the priority the task should be
                                            run at */
  MTAPI_TASK_AFFINITY,                 /**< the affinity of the task */
  MTAPI_TASK_USER_DATA,                /**< pointer to user data associated
                                            with the task */
  MTAPI_TASK_COMPLETE_FUNCTION,        /**< pointer to a function being called
                                            when the task finishes execution */
  MTAPI_TASK_PROBLEM_SIZE              /**< integer indicating the relative
                                            problem size of the task */
};
/** size of the \a MTAPI_TASK_DETACHED attribute */
#define MTAPI_TASK_DETACHED_SIZE sizeof(mtapi_boolean_t)
/** size of the \a MTAPI_TASK_INSTANCES attribute */
#define MTAPI_TASK_INSTANCES_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_TASK_PRIORITY attribute */
#define MTAPI_TASK_PRIORITY_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_TASK_AFFINITY attribute */
#define MTAPI_TASK_AFFINITY_SIZE sizeof(mtapi_affinity_t)
/** size of the \a MTAPI_TASK_PROBLEM_SIZE attribute */
#define MTAPI_TASK_PROBLEM_SIZE_SIZE sizeof(mtapi_uint_t)


/**
 * action attributes
 */
enum mtapi_action_attributes_enum {
  MTAPI_ACTION_GLOBAL,                 /**< indicates global visibility of the
                                            action */
  MTAPI_ACTION_AFFINITY,               /**< the affinity of tasks using the
                                            action */
  MTAPI_ACTION_DOMAIN_SHARED           /**< indicates domain wide visibility of
                                            the action */
};
/** size of the \a MTAPI_ACTION_GLOBAL attribute */
#define MTAPI_ACTION_GLOBAL_SIZE sizeof(mtapi_boolean_t)
/** size of the \a MTAPI_ACTION_AFFINITY attribute */
#define MTAPI_ACTION_AFFINITY_SIZE sizeof(mtapi_affinity_t)
/** size of the \a MTAPI_ACTION_DOMAIN_SHARED attribute */
#define MTAPI_ACTION_DOMAIN_SHARED_SIZE sizeof(mtapi_boolean_t)


/**
 * queue attributes
 */
enum mtapi_queue_attributes_enum {
  MTAPI_QUEUE_GLOBAL,                  /**< indicates global visibility of the
                                            queue */
  MTAPI_QUEUE_PRIORITY,                /**< the priority of tasks launched via
                                            the queue */
  MTAPI_QUEUE_LIMIT,                   /**< maximum tasks in the queue */
  MTAPI_QUEUE_ORDERED,                 /**< toggles ordered execution of tasks
                                            in the queue */
  MTAPI_QUEUE_RETAIN,                  /**< indicates whether tasks should be
                                            retained if the queue is disabled */
  MTAPI_QUEUE_DOMAIN_SHARED            /**< indicates domain wide visibility of
                                            the action */
};
/** size of the \a MTAPI_QUEUE_GLOBAL attribute */
#define MTAPI_QUEUE_GLOBAL_SIZE sizeof(mtapi_boolean_t)
/** size of the \a MTAPI_QUEUE_PRIORITY attribute */
#define MTAPI_QUEUE_PRIORITY_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_QUEUE_LIMIT attribute */
#define MTAPI_QUEUE_LIMIT_SIZE sizeof(mtapi_uint_t)
/** size of the \a MTAPI_QUEUE_ORDERED attribute */
#define MTAPI_QUEUE_ORDERED_SIZE sizeof(mtapi_boolean_t)
/** size of the \a MTAPI_QUEUE_RETAIN attribute */
#define MTAPI_QUEUE_RETAIN_SIZE sizeof(mtapi_boolean_t)
/** size of the \a MTAPI_QUEUE_DOMAIN_SHARED attribute */
#define MTAPI_QUEUE_DOMAIN_SHARED_SIZE sizeof(mtapi_boolean_t)


#define MTAPI_ATTRIBUTE_VALUE(value) ((void*)(value))
#define MTAPI_ATTRIBUTE_POINTER_AS_VALUE 0


/* ---- ATTRIBUTES --------------------------------------------------------- */

/**
 * Node attributes.
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
struct mtapi_node_attributes_struct {
  embb_core_set_t core_affinity;       /**< stores MTAPI_NODE_CORE_AFFINITY */
  mtapi_uint_t num_cores;              /**< stores MTAPI_NODE_NUMCORES */
  mtapi_uint_t type;                   /**< stores MTAPI_NODE_TYPE */
  mtapi_uint_t max_tasks;              /**< stores MTAPI_NODE_MAX_TASKS */
  mtapi_uint_t max_actions;            /**< stores MTAPI_NODE_MAX_ACTIONS */
  mtapi_uint_t max_groups;             /**< stores MTAPI_NODE_MAX_GROUPS */
  mtapi_uint_t max_queues;             /**< stores MTAPI_NODE_MAX_QUEUES */
  mtapi_uint_t queue_limit;            /**< stores MTAPI_NODE_QUEUE_LIMIT */
  mtapi_uint_t max_jobs;               /**< stores MTAPI_NODE_MAX_JOBS */
  mtapi_uint_t max_actions_per_job;    /**< stores
                                            MTAPI_NODE_MAX_ACTIONS_PER_JOB */
  mtapi_uint_t max_priorities;         /**< stores MTAPI_NODE_MAX_PRIORITIES */
  mtapi_boolean_t reuse_main_thread;   /**< stores
                                            MTAPI_NODE_REUSE_MAIN_THREAD */
  mtapi_worker_priority_entry_t * worker_priorities;
                                       /**< stores
                                            MTAPI_NODE_WORKER_PRIORITIES */
};

/**
 * Action attributes.
 * \ingroup ACTIONS
 */
struct mtapi_action_attributes_struct {
  mtapi_boolean_t global;              /**< stores MTAPI_ACTION_GLOBAL */
  mtapi_affinity_t affinity;           /**< stores MTAPI_ACTION_AFFINITY */
  mtapi_boolean_t domain_shared;       /**< stores MTAPI_ACTION_DOMAIN_SHARED*/
};

/**
 * Task attributes.
 * \ingroup TASKS
 */
struct mtapi_task_attributes_struct {
  mtapi_boolean_t is_detached;         /**< stores MTAPI_TASK_DETACHED */
  mtapi_uint_t num_instances;          /**< stores MTAPI_TASK_INSTANCES */
  mtapi_uint_t priority;               /**< stores MTAPI_TASK_PRIORITY */
  mtapi_affinity_t affinity;           /**< stores MTAPI_TASK_AFFINITY */
  void * user_data;                    /**< stores MTAPI_TASK_USER_DATA */
  mtapi_task_complete_function_t
    complete_func;                     /**< stores
                                            MTAPI_TASK_COMPLETE_FUNCTION */
  mtapi_uint_t problem_size;           /**< stores MTAPI_TASK_PROBLEM_SIZE */
};

/**
 * Queue attributes.
 * \ingroup QUEUES
 */
struct mtapi_queue_attributes_struct {
  mtapi_boolean_t global;              /**< stores MTAPI_QUEUE_GLOBAL */
  mtapi_uint_t priority;               /**< stores MTAPI_QUEUE_PRIORITY */
  mtapi_uint_t limit;                  /**< stores MTAPI_QUEUE_LIMIT */
  mtapi_boolean_t ordered;             /**< stores MTAPI_QUEUE_ORDERED */
  mtapi_boolean_t retain;              /**< stores MTAPI_QUEUE_RETAIN */
  mtapi_boolean_t domain_shared;       /**< stores MTAPI_QUEUE_DOMAIN_SHARED */
};

/**
 * Group attributes.
 * \ingroup TASK_GROUPS
 */
struct mtapi_group_attributes_struct {
  mtapi_int_t some_value;              /**< just a placeholder */
};

/**
 * Node attributes type.
 * \memberof mtapi_node_attributes_struct
 */
typedef struct mtapi_node_attributes_struct mtapi_node_attributes_t;

/**
 * Action attributes type.
 * \memberof mtapi_action_attributes_struct
 */
typedef struct mtapi_action_attributes_struct mtapi_action_attributes_t;

/**
 * Task attributes type.
 * \memberof mtapi_task_attributes_struct
 */
typedef struct mtapi_task_attributes_struct mtapi_task_attributes_t;

/**
 * Queue attributes type.
 * \memberof mtapi_queue_attributes_struct
 */
typedef struct mtapi_queue_attributes_struct mtapi_queue_attributes_t;

/**
 * Group attributes type.
 * \memberof mtapi_group_attributes_struct
 */
typedef struct mtapi_group_attributes_struct mtapi_group_attributes_t;

/** short form for using the default node attributes */
#define MTAPI_DEFAULT_NODE_ATTRIBUTES MTAPI_NULL

/** short form for using the default action attributes */
#define MTAPI_DEFAULT_ACTION_ATTRIBUTES MTAPI_NULL

/** short form for using the default task attributes */
#define MTAPI_DEFAULT_TASK_ATTRIBUTES MTAPI_NULL

/** short form for using the default queue attributes */
#define MTAPI_DEFAULT_QUEUE_ATTRIBUTES MTAPI_NULL

/** short form for using the default group attributes */
#define MTAPI_DEFAULT_GROUP_ATTRIBUTES MTAPI_NULL


/* ---- FUNCTION TYPES ----------------------------------------------------- */

/**
 * Task context type.
 * \memberof embb_mtapi_task_context_struct
 */
typedef struct embb_mtapi_task_context_struct mtapi_task_context_t;

/**
 * An action function is the executable software function that implements an
 * action.
 *
 * The runtime passes arguments to the action function when a task is started.
 * Passing arguments from one node to another node should be implemented as a
 * copy operation. Just as the arguments are passed before start of execution,
 * the result buffer is copied back to the calling node after the action
 * function terminates. In shared memory environments, the copying of data in
 * both cases is not necessary. The node-local data is data used by several
 * action functions being executed on the same node (or at least in the same
 * address space). The shared data is specified when the action is created.
 *
 * An action function can interact with the runtime environment through a task
 * context object of type mtapi_task_context_t . A task context object is
 * allocated and managed by the runtime. The runtime passes a pointer to the
 * context object when the action function is invoked. The action may then
 * query information about the execution context (e.g., its core number, the
 * number of tasks and task number in a multi-instance task, polling the task
 * state) by calling the mtapi_context_* functions. Furthermore it is possible
 * to pass information from the action function to the runtime system which
 * is executing the action function (setting the status manually, for example).
 * All of these mtapi_context_* functions are called in the context of task
 * execution.
 *
 * \ingroup ACTION_FUNCTIONS
 */
typedef void(*mtapi_action_function_t)(
  const void* args,                    /**< arguments */
  mtapi_size_t args_size,              /**< length of arguments */
  void* result_buffer,                 /**< buffer for storing results */
  mtapi_size_t result_buffer_size,     /**< length of result_buffer */
  const void* node_local_data,         /**< node-local data, shared data by
                                            several tasks executed on the same
                                            node */
  mtapi_size_t node_local_data_size,   /**< length of shared data */
  mtapi_task_context_t * context       /**< MTAPI task context provided by the
                                            runtime systems identifying the
                                            current task for calling back the
                                            runtime system from the action
                                            function */
);


/* ---- HANDLES and IDs ---------------------------------------------------- */

typedef mtapi_uint_t mtapi_action_id_t;
typedef mtapi_uint_t mtapi_job_id_t;
typedef mtapi_uint_t mtapi_queue_id_t;
typedef mtapi_uint_t mtapi_group_id_t;
typedef mtapi_uint_t mtapi_task_id_t;

/**
 * Action handle.
 * \ingroup ACTIONS
 */
struct mtapi_action_hndl_struct {
  mtapi_uint_t tag;                    /**< version of this handle */
  mtapi_action_id_t id;                /**< pool index of this handle */
};

/**
 * Action handle type.
 * \memberof mtapi_action_hndl_struct
 */
typedef struct mtapi_action_hndl_struct mtapi_action_hndl_t;

/**
 * Job handle.
 * \ingroup JOBS
 */
struct mtapi_job_hndl_struct {
  mtapi_uint_t tag;                    /**< version of this handle */
  mtapi_job_id_t id;                   /**< pool index of this handle */
};

/**
 * Job handle type.
 * \memberof mtapi_job_hndl_struct
 */
typedef struct mtapi_job_hndl_struct mtapi_job_hndl_t;

/**
 * Queue handle.
 * \ingroup QUEUES
 */
struct mtapi_queue_hndl_struct {
  mtapi_uint_t tag;                    /**< version of this handle */
  mtapi_queue_id_t id;                 /**< pool index of this handle */
};

/**
 * Queue handle type.
 * \memberof mtapi_queue_hndl_struct
 */
typedef struct mtapi_queue_hndl_struct mtapi_queue_hndl_t;

/**
 * Group handle.
 * \ingroup TASK_GROUPS
 */
struct mtapi_group_hndl_struct {
  mtapi_uint_t tag;                    /**< version of this handle */
  mtapi_group_id_t id;                 /**< pool index of this handle */
};

/**
 * Group handle type.
 * \memberof mtapi_group_hndl_struct
 */
typedef struct mtapi_group_hndl_struct mtapi_group_hndl_t;

/**
 * Task handle.
 * \ingroup TASKS
 */
struct mtapi_task_hndl_struct {
  mtapi_uint_t tag;                    /**< version of this handle */
  mtapi_task_id_t id;                  /**< pool index of this handle */
};

// was forward declared
//typedef struct mtapi_task_hndl_struct mtapi_task_hndl_t;


/* ---- BASIC CONSTANTS ---------------------------------------------------- */

#define MTAPI_TRUE  ((mtapi_boolean_t)1)
#define MTAPI_FALSE ((mtapi_boolean_t)0)

extern mtapi_group_hndl_t MTAPI_GROUP_NONE;

#define MTAPI_NULL 0

#define TEN_SECONDS 10000
#define MTAPI_INFINITE -1
#define MTAPI_NOWAIT 0

#define MTAPI_NODE_MAX_TASKS_DEFAULT 1024
#define MTAPI_NODE_MAX_ACTIONS_DEFAULT 1024
#define MTAPI_NODE_MAX_GROUPS_DEFAULT 128
#define MTAPI_NODE_MAX_QUEUES_DEFAULT 16
/** default size for MTAPI queues */
#define MTAPI_NODE_QUEUE_LIMIT_DEFAULT 1024
#define MTAPI_NODE_MAX_JOBS_DEFAULT 256
#define MTAPI_NODE_MAX_ACTIONS_PER_JOB_DEFAULT 4
#define MTAPI_NODE_MAX_PRIORITIES_DEFAULT 4

#define MTAPI_JOB_ID_INVALID 0
#define MTAPI_DOMAIN_ID_INVALID 0
#define MTAPI_NODE_ID_INVALID 0

#define MTAPI_TASK_ID_NONE 0
#define MTAPI_GROUP_ID_NONE 0
#define MTAPI_QUEUE_ID_NONE 0
#define MTAPI_ACTION_ID_NONE 0


/* ---- RUNTIME INIT & SHUTDOWN -------------------------------------------- */

/**
 * \defgroup RUNTIME_INIT_SHUTDOWN General
 *
 * \ingroup C_MTAPI
 *
 * Initialization, introspection, and finalization functions.
 *
 * All applications wishing to use MTAPI functionality must use the
 * initialization and finalization routines. After initialization, the
 * introspection functions can provide important information to MTAPI-based
 * applications.
 */

/**
 * This function initializes a node attributes object.
 *
 * A node attributes object is a container of node attributes. It is an
 * optional argument passed to mtapi_initialize() to specify non-default node
 * attributes when creating a node.
 *
 * To set node attributes to non-default values, the application must allocate
 * a node attributes object of type mtapi_node_attributes_t and initialize it
 * with a call to mtapi_nodeattr_init(). The application may call
 * mtapi_nodeattr_set() to specify attribute values. Calls to
 * mtapi_nodeattr_init() have no effect on node attributes after the node has
 * been created and initialized with mtapi_initialize(). The
 * mtapi_node_attributes_t object may safely be deleted by the application
 * after the call to mtapi_nodeattr_init().
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code             | Description
 * ---------------------- | ---------------------------------------------------
 * \c MTAPI_ERR_PARAMETER | Invalid attributes parameter.
 *
 * \see mtapi_initialize(), mtapi_nodeattr_set()
 *
 * \notthreadsafe
 * \memberof mtapi_node_attributes_struct
 */
void mtapi_nodeattr_init(
  MTAPI_OUT mtapi_node_attributes_t* attributes,
                                       /**< [out] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function sets node attribute values in a node attributes object.
 *
 * A node attributes object is a container of node attributes, optionally
 * passed to mtapi_initialize() to specify non-default node attributes when
 * creating a node.
 *
 * \c attributes is a pointer to a node attributes object that was previously
 * initialized with a call to mtapi_nodeattr_init(). Calls to
 * mtapi_nodeattr_set() have no effect on node attributes after the node has
 * been created and initialized with mtapi_initialize(). The node attributes
 * object may safely be deleted by the application after the call to
 * mtapi_initialize().
 *
 * See the table below for a list of predefined attribute numbers and the
 * sizes of the attribute values. The application must set \c attribute_size to
 * the exact size in bytes of the attribute value.
 * Additional attributes may be defined by the implementation.
 *
 * MTAPI-defined node attributes:
 * <table>
 *   <tr>
 *     <th>Attribute num</th>
 *     <th>Description</th>
 *     <th>Data Type</th>
 *     <th>Default</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_NODES_NUMCORES</td>
 *     <td>(Read-only) number of processor cores of the node.</td>
 *     <td>\c mtapi_uint_t</td>
 *     <td>(none)</td>
 *   </tr>
 * </table>
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_ATTR_READONLY | Attribute cannot be modified.
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 *
 * \see mtapi_nodeattr_init(), mtapi_initialize()
 *
 * \notthreadsafe
 * \memberof mtapi_node_attributes_struct
 */
void mtapi_nodeattr_set(
  MTAPI_INOUT mtapi_node_attributes_t* attributes,
                                       /**< [in, out] Pointer to attributes */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Initializes the MTAPI environment on a given MTAPI node in a given MTAPI
 * domain.
 *
 * It must be called on each node using MTAPI. A node maps to a process,
 * thread, thread pool, instance of an operating system, hardware accelerator,
 * processor core, a cluster of processor cores, or another abstract processing
 * entity with an independent program counter. In other words, an MTAPI node
 * is an independent thread of control.
 *
 * Application software running on an MTAPI node must call mtapi_initialize()
 * once per node. It is an error to call mtapi_initialize() multiple times
 * from a given node, unless mtapi_finalize() is called in between.
 *
 * The values for \c domain_id and \c node_id must be known a priori by the
 * application and MTAPI.
 *
 * \c mtapi_info is used to obtain information from the MTAPI implementation,
 * including MTAPI and the underlying implementation version numbers,
 * implementation vendor identification, the number of cores of a node, and
 * vendor-specific implementation information. See the header files for
 * additional information.
 *
 * A given MTAPI implementation will specify what is a node, i.e., how the
 * concrete system is partitioned into nodes and what are the underlying units
 * of execution executing tasks, e.g., threads, a thread pool, processes, or
 * hardware units.
 *
 * \c attributes is a pointer to a node attributes object that was previously
 * prepared with mtapi_nodeattr_init() and mtapi_nodeattr_set(). If
 * \c attributes is \c MTAPI_NULL, then the following default
 * attributes will be used:
 *   - all available cores will be used
 *   - the main thread will be reused as a worker
 *   - maximum number of tasks is 1024
 *   - maximum number of groups is 128
 *   - maximum number of queues is 16
 *   - maximum queue capacity is 1024
 *   - maximum number of priorities is 4.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                    | Description
 * ----------------------------- | --------------------------------------------
 * \c MTAPI_ERR_NODE_INITFAILED  | MTAPI environment could not be initialized.
 * \c MTAPI_ERR_NODE_INITIALIZED | MTAPI environment was already initialized.
 * \c MTAPI_ERR_NODE_INVALID     | The \c node_id parameter is not valid.
 * \c MTAPI_ERR_DOMAIN_INVALID   | The \c domain_id parameter is not valid.
 * \c MTAPI_ERR_PARAMETER        | Invalid mtapi_node_attributes or mtapi_info.
 *
 * \see mtapi_nodeattr_init(), mtapi_nodeattr_set()
 *
 * \notthreadsafe
 * \memory Allocates some memory depending on the node attributes. The amount
 *         allocated is returned in the mtapi_info structure.
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
void mtapi_initialize(
  MTAPI_IN mtapi_domain_t domain_id,   /**< [in] Domain id */
  MTAPI_IN mtapi_node_t node_id,       /**< [in] Node id */
  MTAPI_IN mtapi_node_attributes_t* attributes,
                                       /**< [in] Pointer to attributes */
  MTAPI_OUT mtapi_info_t* mtapi_info,  /**< [out] Pointer to info struct */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Given a node and attribute number, returns a copy of the corresponding
 * attribute value in \c *attribute.
 *
 * See mtapi_nodeattr_set() for a list of predefined attribute numbers and the
 * sizes of the attribute values. The application is responsible for allocating
 * sufficient space for the returned attribute value and for setting
 * \c attribute_size to the exact size in bytes of the attribute value.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS and the attribute value
 * will be written to \c *attribute. On error, \c *status is set to the
 * appropriate error defined below and \c *attribute is undefined.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_PARAMETER    | Invalid attribute parameter.
 * \c MTAPI_ERR_ATTR_NUM     | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE    | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_nodeattr_set()
 *
 * \waitfree
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
void mtapi_node_get_attribute(
  MTAPI_IN mtapi_node_t node,          /**< [in] Node handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_OUT void* attribute,           /**< [out] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,/**< [in] Size of attribute value */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Finalizes the MTAPI environment on a given MTAPI node and domain.
 *
 * It has to be called by each node using MTAPI. It is an error to call
 * mtapi_finalize() without first calling mtapi_initialize(). An MTAPI node
 * can call mtapi_finalize() once for each call to mtapi_initialize(), but it
 * is an error to call mtapi_finalize() multiple times from a given node
 * unless mtapi_initialize() has been called prior to each mtapi_finalize()
 * call.
 *
 * All tasks that have not completed and that have been started on the node
 * where mtapi_finalize() is called will be canceled
 * (see mtapi_task_cancel()). mtapi_finalize() blocks until all tasks that
 * have been started on the same node return (long-running tasks already
 * executing must actively poll the task state and return if canceled). Tasks
 * that execute actions on the node where mtapi_finalize() is called, also
 * block finalization of the MTAPI runtime system on that node. They are
 * canceled as well and return with an \c MTAPI_ERR_NODE_NOTINIT status. Other
 * functions that have a dependency to the node and that are called after
 * mtapi_finalize() also return \c MTAPI_ERR_NODE_NOTINIT (e.g.,
 * mtapi_task_get() starting a task associated with an action implemented on
 * the already-finalized node).
 *
 * mtapi_finalize() may not be called from an action function.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                    | Description
 * ----------------------------- | --------------------------------------------
 * \c MTAPI_ERR_NODE_FINALFAILED | The MTAPI environment couldn't be finalized.
 * \c MTAPI_ERR_NODE_NOTINIT     | The calling node is not initialized.
 *
 * \see mtapi_initialize(), mtapi_task_cancel(), mtapi_task_get()
 *
 * \notthreadsafe
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
void mtapi_finalize(
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns the domain id associated with the local node.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \returns Domain id of local node
 * \waitfree
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
mtapi_domain_t mtapi_domain_id_get(
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns the node id associated with the local node and domain.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \returns Node id of local node
 * \waitfree
 * \ingroup RUNTIME_INIT_SHUTDOWN
 */
mtapi_node_t mtapi_node_id_get(
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/* ---- ACTIONS ------------------------------------------------------------ */

/**
 * \defgroup ACTIONS Actions
 *
 * \ingroup C_MTAPI
 *
 * Hardware or software implementations of jobs.
 *
 * An action is referenced by an opaque handle of type \c mtapi_action_hndl_t,
 * or indirectly through a handle to a job of type \c mtapi_job_hndl_t. A job
 * refers to all actions implementing the same job, regardless of the node(s)
 * where they are implemented.
 *
 * An action's lifetime begins when the application successfully calls
 * mtapi_action_create() and obtains a handle to the action. Its lifetime ends
 * upon successful completion of mtapi_action_delete() or mtapi_finalize().
 *
 * While an opaque handle to an action may be used in the scope of one node
 * only, a job can be used to refer to all its associated actions implementing
 * the same job, regardless of the node where they are implemented. Tasks may
 * be invoked in this way from nodes that do not share memory or even the same
 * ISA with the node where the action resides.
 */

/**
 * This function initializes an action attributes object.
 *
 * A action attributes object is a container of action attributes, optionally
 * passed to mtapi_action_create() to create an action with non-default
 * attributes.
 *
 * The application is responsible for allocating the
 * \c mtapi_action_attributes_t
 * object and initializing it with a call to mtapi_actionattr_init(). The
 * application may then call mtapi_actionattr_set() to specify action
 * attribute values. Calls to mtapi_actionattr_init() have no effect on action
 * attributes after the action has been created with mtapi_action_create(). The
 * \c mtapi_action_attributes_t object may safely be deleted by the application
 * after the call to mtapi_action_create().
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_PARAMETER    | Invalid attributes parameter.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_actionattr_set(), mtapi_action_create()
 *
 * \notthreadsafe
 * \memberof mtapi_action_attributes_struct
 */
void mtapi_actionattr_init(
  MTAPI_OUT mtapi_action_attributes_t* attributes,
                                       /**< [out] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function sets action attribute values in an action attributes object.
 *
 * An action attributes object is a container of action attributes, optionally
 * passed to mtapi_action_create() to create an action with non-default
 * attributes.
 *
 * See the table below for a list of predefined attribute numbers and the
 * sizes of the attribute values. The application must set \c attribute_size to
 * the exact size in bytes of the attribute value. Additional attributes may
 * be defined by the implementation.
 *
 * Calls to mtapi_actionattr_set() have no effect on action attributes after
 * the action has been created. The \c mtapi_action_attributes_t object may
 * safely be deleted by the application after the call to
 * mtapi_action_create().
 *
 * MTAPI-defined action attributes:
 * <table>
 *   <tr>
 *     <th>Attribute num</th>
 *     <th>Description</th>
 *     <th>Data Type</th>
 *     <th>Default</th>
 *   </tr>
 *   <tr>
 *     <td>MTAPI_ACTION_GLOBAL</td>
 *     <td>Indicates whether or not this is a globally visible action. Local
 *         actions are not shared with other nodes.</td>
 *     <td>mtapi_boolean_t</td>
 *     <td>MTAPI_TRUE</td>
 *   </tr>
 *   <tr>
 *     <td>MTAPI_ACTION_AFFINITY</td>
 *     <td>Core affinity of action code.</td>
 *     <td>mtapi_affinity_t</td>
 *     <td>all cores set</td>
 *   </tr>
 *   <tr>
 *     <td>MTAPI_DOMAIN_SHARED</td>
 *     <td>Indicates whether or not the action is shareable across
 *         domains.</td>
 *     <td>mtapi_boolean_t</td>
 *     <td>MTAPI_TRUE</td>
 *   </tr>
 * </table>
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_ATTR_READONLY | Attribute cannot be modified.
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_action_create()
 *
 * \notthreadsafe
 * \memberof mtapi_action_attributes_struct
 */
void mtapi_actionattr_set(
  MTAPI_INOUT mtapi_action_attributes_t* attributes,
                                       /**< [in,out] Pointer to attributes */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function creates a software action (hardware actions are considered to
 * be pre-existent and do not need to be created).
 *
 * It is called on the node where the action function is implemented. An
 * action is an abstract encapsulation of everything needed to implement a
 * job. An action contains attributes, a reference to a job, a reference to an
 * action function, and a reference to node-local data. After an action is
 * created, it is referenced by the application using a node-local handle of
 * type \c mtapi_action_hndl_t, or indirectly through a node-local job handle
 * of type \c mtapi_job_hndl_t. An action's life-cycle begins with
 * mtapi_action_create(), and ends when mtapi_action_delete() or
 * mtapi_finalize() is called.
 *
 * To create an action, the application must supply the domain-wide job ID of
 * the job associated with the action. Job IDs must be predefined in the
 * application and runtime, of type \c mtapi_job_id_t, which is an
 * implementation-defined type. The job ID is unique in the sense that it is
 * unique for the job implemented by the action. However several actions may
 * implement the same job for load balancing purposes.
 *
 * For non-default behavior, \c *attributes must be prepared with
 * mtapi_actionattr_init() and mtapi_actionattr_set() prior to calling
 * mtapi_action_create(). If attributes is \c MTAPI_NULL, then default
 * attributes will be used.
 *
 * If \c node_local_data_size is not zero, \c node_local_data specifies the
 * start of node local data shared by action functions executed on the same
 * node. \c node_local_data_size can be used by the runtime for cache coherency
 * operations.
 *
 * On success, an action handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below. In the case where the action already exists, \c status will
 * be set to \c MTAPI_ERR_ACTION_EXISTS and the handle returned will not be a
 * valid handle.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_JOB_INVALID</td>
 *     <td>The \c job_id is not a valid job ID, i.e., no action was created for
 *         that ID or the action has been deleted.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_EXISTS</td>
 *     <td>This action is already created.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_LIMIT</td>
 *     <td>Exceeded maximum number of actions allowed.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_NOAFFINITY</td>
 *     <td>The action was created with an \c MTAPI_ACTION_AFFINITY attribute
 *         that has set the affinity to all cores of the node to
 *         \c MTAPI_FALSE.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_PARAMETER</td>
 *     <td>Invalid attributes parameter.</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_actionattr_init(), mtapi_actionattr_set(), mtapi_action_delete(),
 *      mtapi_finalize()
 *
 * \returns Handle to newly created action, invalid handle on error
 * \threadsafe
 * \ingroup ACTIONS
 */
mtapi_action_hndl_t mtapi_action_create(
  MTAPI_IN mtapi_job_id_t job_id,      /**< [in] Job id */
  MTAPI_IN mtapi_action_function_t function,
                                       /**< [in] Action function pointer */
  MTAPI_IN void* node_local_data,      /**< [in] Data shared across tasks */
  MTAPI_IN mtapi_size_t node_local_data_size,
                                       /**< [in] Size of shared data */
  MTAPI_IN mtapi_action_attributes_t* attributes,
                                       /**< [in] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function changes the value of the attribute that corresponds to the
 * given \c attribute_num for this action.
 *
 * \c attribute must point to the attribute value, and \c attribute_size must
 * be set to the exact size of the attribute value. See mtapi_actionattr_set()
 * for a list of predefined attribute numbers and the sizes of their values.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                  | Description
 * --------------------------- | ----------------------------------------------
 * \c MTAPI_ERR_PARAMETER      | Invalid attribute parameter.
 * \c MTAPI_ERR_ACTION_INVALID | Argument is not a valid action handle.
 * \c MTAPI_ERR_ATTR_NUM       | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE      | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT   | The calling node is not initialized.
 *
 * \see mtapi_actionattr_set()
 *
 * \notthreadsafe
 * \ingroup ACTIONS
 */
void mtapi_action_set_attribute(
  MTAPI_IN mtapi_action_hndl_t action, /**< [in] Action handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns the attribute value that corresponds to the given \c attribute_num
 * for this action.
 *
 * \c attribute must point to the location where the attribute value is to be
 * returned, and \c attribute_size must be set to the exact size of the
 * attribute value. See mtapi_actionattr_set() for a list of predefined
 * attribute numbers and the sizes of their values.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS and the attribute value is
 * returned in \c *attribute. On error, \c *status is set to the appropriate
 * error defined below and \c *attribute is undefined.
 * Error code                  | Description
 * --------------------------- | ----------------------------------------------
 * \c MTAPI_ERR_PARAMETER      | Invalid attribute parameter.
 * \c MTAPI_ERR_ACTION_INVALID | Argument is not a valid action handle.
 * \c MTAPI_ERR_ATTR_NUM       | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE      | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT   | The calling node is not initialized.
 *
 * \see mtapi_actionattr_set()
 *
 * \waitfree
 * \ingroup ACTIONS
 */
void mtapi_action_get_attribute(
  MTAPI_IN mtapi_action_hndl_t action, /**< [in] Action handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_OUT void* attribute,           /**< [out] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,/**< [in] Size of attribute value */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function deletes a software action (Hardware actions exist perpetually
 * and cannot be deleted).
 *
 * mtapi_action_delete() may be called by any node that has a valid action
 * handle. Tasks associated with an action that has been deleted may still be
 * executed depending on their internal state:
 *  - If mtapi_action_delete() is called on an action that is currently
 *    executing, the associated task's state will be set to
 *    \c MTAPI_TASK_CANCELLED and execution will continue. To accomplish this,
 *    action functions must poll the task state with
 *    mtapi_context_taskstate_get(). A call to mtapi_task_wait() on the task
 *    executing this code will return the status set by
 *    mtapi_context_status_set(), or \c MTAPI_SUCCESS if not explicitly set.
 *  - Tasks that are started or enqueued but waiting for execution by the
 *    MTAPI runtime when mtapi_action_delete() is called will not be executed
 *    anymore if the deleted action is the only action associated with that
 *    task. A call to mtapi_task_wait() will return the status
 *    \c MTAPI_ERR_ACTION_DELETED.
 *  - Tasks that are started or enqueued after deletion of the action will
 *    return \c MTAPI_ERR_ACTION_INVALID if the deleted action is the only
 *    action associated with that task.
 *
 * Calling mtapi_action_get_attribute() on a deleted action will return
 * \c MTAPI_ERR_ACTION_INVALID if all actions implementing the job had been
 * deleted.
 *
 * The function mtapi_action_delete() blocks until the corresponding action
 * code is left by all tasks that are executing the code or until the timeout
 * is reached. If \c timeout is a constant 0 or the symbolic constant
 * \c MTAPI_NOWAIT, this function only returns \c MTAPI_SUCCESS if no tasks are
 * executing the action when it is called. If it is set to \c MTAPI_INFINITE,
 * the function may block infinitely.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                  | Description
 * --------------------------- | ----------------------------------------------
 * \c MTAPI_ERR_ACTION_INVALID | Argument is not a valid action handle.
 * \c MTAPI_TIMEOUT            | Timeout was reached.
 * \c MTAPI_ERR_NODE_NOTINIT   | The calling node is not initialized.
 *
 * \see mtapi_context_taskstate_get(), mtapi_context_status_set(),
 *      mtapi_task_wait()
 *
 * \threadsafe
 * \ingroup ACTIONS
 */
void mtapi_action_delete(
  MTAPI_IN mtapi_action_hndl_t action, /**< [in] Action handle */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function disables an action.
 *
 * Tasks associated with an action that has been disabled may still be
 * executed depending on their internal state:
 *  - If mtapi_action_disable() is called on an action that is currently
 *    executing, the associated task's state will be set to
 *    \c MTAPI_TASK_CANCELLED and execution will continue. To accomplish this,
 *    action functions must poll the task with mtapi_context_taskstate_get().
 *    A call to mtapi_task_wait() on the task executing this code will return
 *    the status set by mtapi_context_status_set(), or \c MTAPI_SUCCESS if not
 *    explicitly set.
 *  - Tasks that are started or enqueued but waiting for execution by the
 *    MTAPI runtime when mtapi_action_disable() is called will not be executed
 *    anymore if the disabled action is the only action associated with that
 *    task. A call to mtapi_task_wait() will return the status
 *    \c MTAPI_ERR_ACTION_DISABLED.
 *  - Tasks that are started or enqueued after the action has been disabled
 *    will return \c MTAPI_ERR_ACTION_DISABLED if either the disabled action is
 *    the only action associated with a task or all actions associated with a
 *    task are disabled.
 * mtapi_action_disable() blocks until all running tasks exit the code, or
 * until the timeout is reached. If timeout is the constant 0 or the symbolic
 * constant \c MTAPI_NOWAIT, this function only returns \c MTAPI_SUCCESS if no
 * tasks are executing the action when it is called. If it is set to
 * \c MTAPI_INFINITE the function may block infinitely.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                  | Description
 * --------------------------- | ----------------------------------------------
 * \c MTAPI_ERR_ACTION_INVALID | Argument is not a valid action handle.
 * \c MTAPI_TIMEOUT            | Timeout was reached.
 * \c MTAPI_ERR_NODE_NOTINIT   | The calling node is not initialized.
 *
 * \see mtapi_context_taskstate_get(), mtapi_context_status_set(),
 *      mtapi_task_wait()
 *
 * \waitfree
 * \ingroup ACTIONS
 */
void mtapi_action_disable(
  MTAPI_IN mtapi_action_hndl_t action, /**< [in] Action handle */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function enables a previously disabled action.
 *
 * If this function is called on an action that no longer exists, an
 * \c MTAPI_ERR_ACTION_INVALID error will be returned.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                  | Description
 * --------------------------- | ----------------------------------------------
 * \c MTAPI_ERR_ACTION_INVALID | Argument is not a valid action handle.
 * \c MTAPI_ERR_NODE_NOTINIT   | The calling node is not initialized.
 *
 * \waitfree
 * \ingroup ACTIONS
 */
void mtapi_action_enable(
  MTAPI_IN mtapi_action_hndl_t action, /**< [in] Action handle */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                                  may be \c MTAPI_NULL */
  );

/**
 * \defgroup ACTION_FUNCTIONS Action Functions
 *
 * \ingroup C_MTAPI
 *
 * Executable software functions that implement actions.
 *
 * The runtime passes arguments to the action function when a task is started.
 * Passing arguments from one node to another node should be implemented as a
 * copy operation. Just as the arguments are passed before start of execution,
 * the result buffer is copied back to the calling node after the action
 * function terminates. In shared memory environments, the copying of data in
 * both cases is not necessary. The node-local data is data used by several
 * action functions being executed on the same node (or at least in the same
 * address space). The shared data is specified when the action is created.
 *
 * An action function can interact with the runtime environment through a task
 * context object of type \c mtapi_task_context_t. A task context object is
 * allocated and managed by the runtime. The runtime passes a pointer to the
 * context object when the action function is invoked.The action may then
 * query information about the execution context(e.g., its core number, the
 * number of tasks and task number in a multi - instance task, polling the
 * task state) by calling the \c mtapi_context_* functions. Furthermore it is
 * possible to pass information from the action function to the runtime system
 * which is executing the action function(setting the status manually, for
 * example). All of these \c mtapi_context_* functions are called in the
 * context of task execution.
 */

/**
 * This function can be called from an action function to set the status that
 * can be obtained by a subsequent call to mtapi_task_wait() or
 * mtapi_group_wait_any().
 *
 * \c task_context must be the same value as the context parameter that the
 * runtime passes to the action function when it is invoked.
 *
 * The status can be passed from the action function to the runtime system by
 * setting error_code to one of the following values:
 *  - \c MTAPI_SUCCESS for successful completion
 *  - \c MTAPI_ERR_ACTION_CANCELLED if the action execution is canceled
 *  - \c MTAPI_ERR_ACTION_FAILED if the task could not be completed as intended
 * The error code will be especially important in future versions of MTAPI
 * where tasks shall be chained (flow graphs). The chain execution can then be
 * aborted if the error code is not \c MTAPI_SUCCESS.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_CONTEXT_OUTOFCONTEXT</td>
 *     <td>Not called in the context of a task execution. This function must
 *         be used in an action function only. The action function must be
 *         called from the MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_task_wait(), mtapi_group_wait_any()
 *
 * \notthreadsafe
 * \ingroup ACTION_FUNCTIONS
 */
void mtapi_context_status_set(
  MTAPI_INOUT mtapi_task_context_t* task_context,
                                       /**< [in,out] Pointer to task context */
  MTAPI_IN mtapi_status_t error_code,  /**< [in] Task return value */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function can be called from an action function to notify the runtime
 * system.
 *
 * This is used to communicate certain states to the runtime implementation to
 * allow it to optimize task execution.
 *
 * \c task_context must be the same value as the context parameter that the
 * runtime passes to the action function when it is invoked.
 *
 * The underlying type \c mtapi_notification_t and the valid values for
 * notification are implementation-defined. The notification system is meant
 * to be flexible, and can be used in many ways, for example:
 *  - To trigger prefetching of data for further processing
 *  - To order execution via queues there might be point in the action code
 *    where the next task in the queue may be started, even if the current
 *    code, started from the same queue, is still executing
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_CONTEXT_OUTOFCONTEXT</td>
 *     <td>Not called in the context of a task execution. This function must
 *         be used in an action function only. The action function must be
 *         called from the MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \notthreadsafe
 * \ingroup ACTION_FUNCTIONS
 */
void mtapi_context_runtime_notify(
  MTAPI_IN mtapi_task_context_t* task_context,
                                       /**< [in] Pointer to task context */
  MTAPI_IN mtapi_notification_t notification,
                                       /**< [in] Notification id */
  MTAPI_IN void* data,                 /**< [in] Pointer to associated data */
  MTAPI_IN mtapi_size_t data_size,     /**< [in] Size of data */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * An action function may call this function to obtain the state of the task
 * that is associated with the action function.
 *
 * \c task_context must be the same value as the context parameter that the
 * runtime passes to the action function when it is invoked.
 *
 * The underlying representation of type \c mtapi_task_state_t is
 * implementation-defined. Values of type \c mtapi_task_state_t may be copied,
 * assigned, and compared with other values of type \c mtapi_task_state_t, but
 * the caller should make no other assumptions about its type or contents. A
 * minimal implementation must return a status of \c MTAPI_TASK_CANCELLED if
 * the task is canceled, and \c MTAPI_TASK_RUNNING otherwise. Other values of
 * the task state are implementation-defined. This task state can be used to
 * abort a long running computation inside an action function.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_CONTEXT_OUTOFCONTEXT</td>
 *     <td>Not called in the context of a task execution. This function must
 *         be used in an action function only. The action function must be
 *         called from the MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \returns Task state of current context
 * \notthreadsafe
 * \ingroup ACTION_FUNCTIONS
 */
mtapi_task_state_t mtapi_context_taskstate_get(
  MTAPI_IN mtapi_task_context_t* task_context,
                                       /**< [in] Pointer to task context */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function can be called from an action function to query the instance
 * number of the associated task.
 *
 * A task can have multiple instances (multi-instance tasks), in which case
 * the same job is executed multiple times in parallel. Each instance has a
 * number, and this function gives the instance number. Task instances are
 * numbered sequentially, starting at zero.
 *
 * \c task_context must be the same value as the context parameter that the
 * runtime passes to the action function when it is invoked.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS and the task instance
 * number is returned. On error, \c *status is set to the appropriate error
 * defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_CONTEXT_OUTOFCONTEXT</td>
 *     <td>Not called in the context of a task execution. This function must
 *         be used in an action function only. The action function must be
 *         called from the MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \returns Instance number of current task
 * \notthreadsafe
 * \ingroup ACTION_FUNCTIONS
 */
mtapi_uint_t mtapi_context_instnum_get(
  MTAPI_IN mtapi_task_context_t* task_context,
                                       /**< [in] Pointer to task context */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function can be called from an action function to query the total
 * number of parallel task instances.
 *
 * This value is greater than one for multi-instance tasks.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_CONTEXT_OUTOFCONTEXT</td>
 *     <td>Not called in the context of a task execution. This function must
 *         be used in an action function only. The action function must be
 *         called from the MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \returns Total number of parallel task instances
 * \notthreadsafe
 * \ingroup ACTION_FUNCTIONS
 */
mtapi_uint_t mtapi_context_numinst_get(
  MTAPI_IN mtapi_task_context_t* task_context,
                                       /**< [in] Pointer to task context */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function can be called from an action function to query the current
 * core number for debugging purposes.
 *
 * The core numbering is implementation-defined.
 *
 * \c task_context must be the same value as the context parameter that the
 * runtime passes to the action function when it was invoked.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_CONTEXT_OUTOFCONTEXT</td>
 *     <td>Not called in the context of a task execution. This function must
 *         be used in an action function only. The action function must be
 *         called from the MTAPI runtime system.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \returns Worker thread index the current task is running on
 * \notthreadsafe
 * \ingroup ACTION_FUNCTIONS
 */
mtapi_uint_t mtapi_context_corenum_get(
  MTAPI_IN mtapi_task_context_t* task_context,
                                       /**< [in] Pointer to task context */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/* ---- CORE AFFINITY MASKS ------------------------------------------------ */

/**
 * \defgroup CORE_AFFINITY_MASKS Core Affinities
 *
 * \ingroup C_MTAPI
 *
 * Affinities for executing action functions on subsets of cores.
 *
 * To set core affinities, the application must allocate an affinity mask
 * object of type \c mtapi_affinity_t and initialize it with a call to
 * mtapi_affinity_init(). Affinities are specified by calling
 * mtapi_affinity_set(). The application must also allocate and initialize an
 * action attributes object of type \c mtapi_action_attributes_t. The affinity
 * mask object is then passed to mtapi_actionattr_set() to set the prescribed
 * affinities in the action attributes object. The action attributes object is
 * then passed to mtapi_action_create() to create a new action with those
 * attributes.
 *
 * It is in the nature of core affinities to be highly hardware dependent. The
 * least common denominator for different architectures is enabling and
 * disabling core numbers in the affinity mask. Action-to-core affinities can
 * be set via the action attribute \c MTAPI_ACTION_AFFINITY during the creation
 * of an action.
 */

/**
 * This function initializes an affinity mask object.
 *
 * The affinity to all cores will be initialized to the value of affinity.
 * This function should be called prior to calling mtapi_affinity_set() to
 * specify non-default affinity settings. The affinity mask object may then
 * be used to set the \c MTAPI_ACTION_AFFINITY attribute when creating an
 * action with mtapi_action_create().
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_AFFINITY_MASK | Invalid mask parameter.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_affinity_set(), mtapi_action_create()
 *
 * \notthreadsafe
 * \ingroup CORE_AFFINITY_MASKS
 */
void mtapi_affinity_init(
  MTAPI_OUT mtapi_affinity_t* mask,    /**< [out] Pointer to affinity mask */
  MTAPI_IN mtapi_boolean_t affinity,   /**< [in] Initial affinity */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function is used to change the default values of an affinity mask
 * object.
 *
 * The affinity mask object can then be passed to mtapi_actionattr_set() to
 * set the \c MTAPI_ACTION_AFFINITY action attribute. An action function will
 * be executed on a core only if the core's affinity is set to \c MTAPI_TRUE.
 * Calls to mtapi_affinity_set() have no effect on action attributes after the
 * action has been created.
 *
 * \c mask must be a pointer to an affinity mask object previously initialized
 * with mtapi_affinity_init().
 *
 * The \c core_num is a hardware- and implementation-specific numeric
 * identifier for a single core of the current node.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_AFFINITY_MASK | Invalid mask parameter.
 * \c MTAPI_ERR_CORE_NUM      | Unknown core number.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_actionattr_set(), mtapi_affinity_init()
 *
 * \notthreadsafe
 * \ingroup CORE_AFFINITY_MASKS
 */
void mtapi_affinity_set(
  MTAPI_INOUT mtapi_affinity_t* mask,  /**< [in, out] Pointer to affinity
                                            mask */
  MTAPI_IN mtapi_uint_t core_num,      /**< [in] Core number */
  MTAPI_IN mtapi_boolean_t affinity,   /**< [in] Affinity to given core */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns the affinity that corresponds to the given \c core_num for this
 * affinity mask.
 *
 * \c mask is a pointer to an affinity mask object previously initialized with
 * mtapi_affinity_init().
 *
 * Note that affinities may be queried but may not be changed for an action
 * after it has been created. If affinities need to be modified at runtime,
 * new actions must be created.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_AFFINITY_MASK | Invalid mask parameter.
 * \c MTAPI_ERR_CORE_NUM      | Unknown core number.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_affinity_init()
 *
 * \returns \c MTAPI_TRUE if affinity to \c core_num is set, \c MTAPI_FALSE
 *          otherwise
 * \waitfree
 * \ingroup CORE_AFFINITY_MASKS
 */
mtapi_boolean_t mtapi_affinity_get(
  MTAPI_OUT mtapi_affinity_t* mask,    /**< [out] Pointer to affinity mask */
  MTAPI_IN mtapi_uint_t core_num,      /**< [in] Core number */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/* ---- QUEUES ------------------------------------------------------------- */

/**
 * \defgroup QUEUES Queues
 *
 * \ingroup C_MTAPI
 *
 * Queues for controlling the scheduling policy of tasks.
 *
 * The default scheduling policy for queues is ordered task execution. Tasks
 * that have to be executed sequentially are enqueued into the same queue. In
 * this case every queue is associated with exactly one action. Tasks started
 * via different queues can be executed in parallel. This is needed for packet
 * processing applications, for example: each stream is processed by one
 * queue. This ensures sequential processing of packets belonging to the same
 * stream. Different streams are processed in parallel.
 *
 * Queues were made explicit in MTAPI. This allows mapping of queues onto
 * hardware queues, if available. One MTAPI queue is associated with one
 * action, or for purposes of load balancing, with actions implementing the
 * same job on different nodes.
 */

/**
 * This function initializes a queue attributes object.
 *
 * A queue attributes object is a container of queue attributes, optionally
 * passed to mtapi_queue_create() to create a queue with non-default
 * attributes.
 *
 * The application is responsible for allocating the
 * \c mtapi_queue_attributes_t
 * object and initializing it with a call to mtapi_queueattr_init(). The
 * application may then call mtapi_queueattr_set() to specify queue attribute
 * values. Calls to mtapi_queueattr_init() have no effect on queue attributes
 * after the queue has been created. To change an attribute of an existing
 * queue, see mtapi_queue_set_attribute(). The \c mtapi_queue_attributes_t
 * object may safely be deleted by the application after the call to
 * mtapi_queue_create().
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_PARAMETER    | Invalid attributes parameter.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_queue_create(), mtapi_queueattr_set(),
 *      mtapi_queue_set_attribute()
 *
 * \notthreadsafe
 * \memberof mtapi_queue_attributes_struct
 */
void mtapi_queueattr_init(
  MTAPI_OUT mtapi_queue_attributes_t* attributes,
                                       /**< [out] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function sets queue attribute values in a queue attributes object.
 *
 * A queue attributes object is a container of queue attributes, optionally
 * passed to mtapi_queue_create() to create a queue with non-default
 * attributes.
 *
 * \c attributes must be a pointer to a queue attributes object previously
 * initialized by mtapi_queueattr_init().
 *
 * See the table below for a list of predefined attribute numbers and the
 * sizes of the attribute values. The application must set \c attribute_size to
 * the exact size in bytes of the attribute value. Additional attributes may
 * be defined by the implementation.
 *
 * Calls to mtapi_queueattr_set() have no effect on queue attributes once the
 * queue has been created. The \c mtapi_queue_attributes_t object may safely be
 * deleted by the application after the call to mtapi_queue_create().
 *
 * MTAPI-defined queue attributes:
 * <table>
 *   <tr>
 *     <th>Attribute num</th>
 *     <th>Description</th>
 *     <th>Data Type</th>
 *     <th>Default</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_QUEUE_GLOBAL</td>
 *     <td>Indicates if this is a globally visible queue. Only global queues
 *         are shared with other nodes.</td>
 *     <td>\c mtapi_boolean_t</td>
 *     <td>\c MTAPI_TRUE</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_QUEUE_PRIORITY</td>
 *     <td>Priority of the queue.</td>
 *     <td>\c mtapi_uint_t</td>
 *     <td>0(default priority)</td>
 *   </tr>
 *   <tr>
 *     <td>MTAPI_QUEUE_LIMIT</td>
 *     <td>Max. number of elements in the queue; the queue blocks on queuing
 *         more items.</td>
 *     <td>\c mtapi_uint_t</td>
 *     <td>0(0 stands for 'unlimited')</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_QUEUE_ORDERED</td>
 *     <td>Specify if the queue is order-preserving.</td>
 *     <td>\c mtapi_boolean_t</td>
 *     <td>\c MTAPI_TRUE</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_QUEUE_RETAIN</td>
 *     <td>Allow enqueuing of jobs when queue is disabled.</td>
 *     <td>\c mtapi_boolean_t</td>
 *     <td>\c MTAPI_FALSE</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_DOMAIN_SHARED</td>
 *     <td>Indicates if the queue is shareable across domains.</td>
 *     <td>\c mtapi_boolean_t</td>
 *     <td>\c MTAPI_TRUE</td>
 *   </tr>
 * </table>
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_ATTR_READONLY | Attribute cannot be modified.
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_queue_create(), mtapi_queueattr_init()
 *
 * \notthreadsafe
 * \memberof mtapi_queue_attributes_struct
 */
void mtapi_queueattr_set(
  MTAPI_INOUT mtapi_queue_attributes_t* attributes,
                                       /**< [in,out] Pointer to attributes */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function creates a software queue object and associates it with the
 * specified job.
 *
 * A job is associated with one or more actions that provide the executable
 * implementation of the job. Hardware queues are considered to be pre-existent
 * and do not need to be created.
 *
 * \c queue_id is an identifier of implementation-defined type that must be
 * supplied by the application. If \c queue_id is set to
 * \c MTAPI_QUEUE_ID_NONE, the queue will be accessible only on the node on
 * which it was created by using the returned queue handle. Otherwise the
 * application may supply a \c queue_id by which the queue can be referenced
 * domain-wide using mtapi_queue_get() to convert the id into a handle. The
 * minimum and maximum values for \c queue_id may be derived from
 * \c MTAPI_MIN_USER_QUEUE_ID and \c MTAPI_MAX_USER_QUEUE_ID.
 *
 * job is a handle to a job obtained by a previous call to mtapi_job_get(). If
 * \c attributes is \c MTAPI_NULL, the queue will be created with default
 * attribute values. Otherwise \c attributes must point to a queue attributes
 * object previously prepared using mtapi_queueattr_init() and
 * mtapi_queueattr_set().
 *
 * There is an implementation-defined maximum number of queues permitted.
 *
 * If more than one action is associated with the job, the runtime system
 * chooses dynamically which action is used for execution (for load balancing
 * purposes).
 *
 * On success, a queue handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below. In the case where the queue already exists, \c *status will
 * be set to \c MTAPI_QUEUE_EXISTS and the handle returned will not be a
 * valid handle.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_QUEUE_INVALID | The \c queue_id is not a valid queue id.
 * \c MTAPI_ERR_QUEUE_EXISTS  | This queue is already created.
 * \c MTAPI_ERR_QUEUE_LIMIT   | Exceeded maximum number of queues allowed.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 * \c MTAPI_ERR_PARAMETER     | Invalid attributes parameter.
 * \c MTAPI_ERR_JOB_INVALID   | The associated job is not valid.
 *
 * \see mtapi_queue_get(), mtapi_job_get(), mtapi_queueattr_init(),
 *      mtapi_queueattr_set()
 *
 * \returns Handle to newly created queue, invalid handle on error
 * \threadsafe
 * \ingroup QUEUES
 */
mtapi_queue_hndl_t mtapi_queue_create(
  MTAPI_IN mtapi_queue_id_t queue_id,  /**< [in] Queue id */
  MTAPI_IN mtapi_job_hndl_t job,       /**< [in] Job handle */
  MTAPI_IN mtapi_queue_attributes_t* attributes,
                                       /**< [in] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Changes the attribute value that corresponds to the given \c attribute_num
 * for the specified queue.
 *
 * See mtapi_queueattr_set() for a list of predefined attribute numbers and
 * the sizes of the attribute values. The application must set
 * \c attribute_size to the exact size in bytes of the attribute value.
 * Additional attributes may be defined by the implementation.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below and the attribute value is
 * undefined.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_QUEUE_INVALID | Argument is not a valid queue handle.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_queueattr_set()
 *
 * \notthreadsafe
 * \ingroup QUEUES
 */
void mtapi_queue_set_attribute(
  MTAPI_IN mtapi_queue_hndl_t queue,   /**< [in] Queue handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns the attribute value that corresponds to the given \c attribute_num
 * for the specified queue.
 *
 * \c attribute must point to a location in memory sufficiently large to hold
 * the returned attribute value. See mtapi_queueattr_set() for a list of
 * predefined attribute numbers and the sizes of the attribute values. The
 * application must set \c attribute_size to the exact size in bytes of the
 * attribute value. Additional attributes may be defined by the implementation.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS and the attribute value is
 * returned in \c *attribute. On error, \c *status is set to the appropriate
 * error defined below and the \c *attribute value is undefined. If this
 * function is called on a queue that no longer exists, an
 * \c MTAPI_ERR_QUEUE_INVALID error will be returned.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_QUEUE_INVALID | Argument is not a valid queue handle.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_queueattr_set()
 *
 * \waitfree
 * \ingroup QUEUES
 */
void mtapi_queue_get_attribute(
  MTAPI_IN mtapi_queue_hndl_t queue,   /**< [in] Queue handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_OUT void* attribute,           /**< [out] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,/**< [in] Size of attribute value */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function converts a domain-wide \c queue_id into a node-local queue
 * handle.
 *
 * \c queue_id must match the \c queue_id that was associated with a software
 * queue that was created with mtapi_queue_create(), or it must be a valid
 * predefined queue identifier known a priori to the runtime and application
 * (e.g., to reference a hardware queue. The minimum and maximum values for
 * \c queue_id may be derived from \c MTAPI_MIN_USER_QUEUE_ID and
 * \c MTAPI_MAX_USER_QUEUE_ID.
 *
 * On success, the queue handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below. If this function is called on a queue that no longer exists,
 * an \c MTAPI_ERR_QUEUE_INVALID error will be returned.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_QUEUE_INVALID</td>
 *     <td>The \c queue_id parameter does not refer to a valid queue or it is set
 *         to \c MTAPI_QUEUE_ID_ANY.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The node/domain is not initialized.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_DOMAIN_NOTSHARED</td>
 *     <td>This resource cannot be shared by this domain.</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_queue_create()
 *
 * \returns Handle to preexisting queue with given \c queue_id,
 *          invalid handle on error
 * \threadsafe
 * \ingroup QUEUES
 */
mtapi_queue_hndl_t mtapi_queue_get(
  MTAPI_IN mtapi_queue_id_t queue_id,  /**< [in] Queue id */
  MTAPI_IN mtapi_domain_t domain_id,   /**< [in] Domain id */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function deletes the specified software queue.
 *
 * Hardware queues are perpetual and cannot be deleted.
 *
 * \c queue must be a valid handle to an existing queue.
 *
 * \c timeout determines how long the function should wait for tasks already
 * started via that queue to finish. The underlying type of \c mtapi_timeout_t
 * is implementation-defined. If \c timeout is a constant 0 or the symbolic
 * constant \c MTAPI_NOWAIT, this function deletes the queue and returns
 * immediately. If \c timeout is set to \c MTAPI_INFINITE the function may
 * block infinitely. Other values for \c timeout and the units of measure are
 * implementation defined.
 *
 * This function can be called from any node that has a valid queue handle.
 * Tasks previously enqueued in a queue that has been deleted may still be
 * executed depending on their internal state:
 *  - If mtapi_queue_delete() is called on a queue that is currently executing
 *    an action, the task state of the corresponding task will be set to
 *    \c MTAPI_TASK_CANCELLED and execution will continue. To accomplish this,
 *    the action function must poll the task state with
 *    mtapi_context_taskstate_get(). A call to mtapi_task_wait() on the task
 *    executing this code will return the status set by
 *    mtapi_context_status_set(), or \c MTAPI_SUCCESS if not explicitly set.
 *  - Tasks that are enqueued and waiting for execution by the MTAPI runtime
 *    environment when mtapi_queue_delete() is called will not be executed any
 *    more. A call to mtapi_task_wait() will return the status
 *    \c MTAPI_ERR_QUEUE_DELETED.
 *  - Tasks that are enqueued after deletion of the queue will return a status
 *    of \c MTAPI_ERR_QUEUE_INVALID.
 *
 * If this function is called on a queue that no longer exists, an
 * \c MTAPI_ERR_QUEUE_INVALID status will be returned. A call to
 * mtapi_queue_get() on a deleted queue will return \c MTAPI_ERR_QUEUE_INVALID
 * as well, as long as no new queue has been created for the same queue ID.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_QUEUE_INVALID | Argument is not a valid queue handle.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 * \c MTAPI_TIMEOUT           | Timeout was reached.
 *
 * \see mtapi_context_taskstate_get(), mtapi_context_status_set(),
 *      mtapi_task_wait(), mtapi_queue_get()
 *
 * \threadsafe
 * \ingroup QUEUES
 */
void mtapi_queue_delete(
  MTAPI_IN mtapi_queue_hndl_t queue,   /**< [in] Queue handle */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function disables the specified queue in such a way that it can be
 * resumed later.
 *
 * This is needed to perform certain maintenance tasks. It can be called by
 * any node that has a valid queue handle.
 *
 * \c timeout determines how long the function should wait for tasks already
 * started via that queue to finish. The underlying type of \c mtapi_timeout_t
 * is implementation-defined. If \c timeout is a constant 0 or the symbolic
 * constant \c MTAPI_NOWAIT, this function deletes the queue and returns
 * immediately. If \c timeout is set to \c MTAPI_INFINITE the function may
 * block infinitely. Other values for \c timeout and the units of measure are
 * implementation defined.
 *
 * Tasks previously enqueued in a queue that has been disabled may still be
 * executed depending on their internal state:
 *  - If mtapi_queue_disable() is called on a queue that is currently executing
 *    an action, the task state of the corresponding task will be set to
 *    \c MTAPI_TASK_CANCELLED and execution will continue. To accomplish this,
 *    the action function must poll the task state by calling
 *    mtapi_context_taskstate_get(). A call to mtapi_task_wait() on the task
 *    executing this code will return the status set by
 *    mtapi_context_status_set(), or \c MTAPI_SUCCESS if not explicitly set.
 *  - Tasks that are enqueued and waiting for execution by the MTAPI runtime
 *    environment when mtapi_queue_disable() is called will not be executed
 *    anymore. They will be held in anticipation the queue is enabled again
 *    if the \c MTAPI_QUEUE_RETAIN attribute is set to \c MTAPI_TRUE. A call to
 *    mtapi_task_wait() will return the status \c MTAPI_ERR_QUEUE_DISABLED.
 *  - Tasks that are enqueued after the queue had been disabled will return
 *    \c MTAPI_ERR_QUEUE_DISABLED if the \c MTAPI_QUEUE_RETAIN attribute is set
 *    to \c MTAPI_FALSE.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_QUEUE_INVALID | Argument is not a valid queue handle.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 * \c MTAPI_TIMEOUT           | Timeout was reached.
 *
 * \see mtapi_context_taskstate_get(), mtapi_context_status_set(),
 *      mtapi_task_wait()
 *
 * \threadsafe
 * \ingroup QUEUES
 */
void mtapi_queue_disable(
  MTAPI_IN mtapi_queue_hndl_t queue,   /**< [in] Queue handle */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function may be called from any node with a valid queue handle to
 * re-enable a queue previously disabled with mtapi_queue_disable().
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_QUEUE_INVALID | Argument is not a valid queue handle.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \threadsafe
 * \ingroup QUEUES
 */
void mtapi_queue_enable(
  MTAPI_IN mtapi_queue_hndl_t queue,   /**< [in] Queue handle */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/* ---- JOBS --------------------------------------------------------------- */

/**
 * \defgroup JOBS Jobs
 *
 * \ingroup C_MTAPI
 *
 * Jobs implementing one or more actions.
 *
 * An action is a hardware or software implementation of a job. In some cases,
 * an action is referenced by an action handle, while in other cases, an action
 * is referenced indirectly through a job handle. Each job is represented by a
 * domain-wide job ID, or by a job handle which is local to one node.
 *
 * Several actions can implement the same job based on different hardware
 * resources (for instance a job can be implemented by one action on a DSP and
 * by another action on a general purpose core, or a job can be implemented by
 * both hardware and software actions).
 */

/**
 * Given a \c job_id, this function returns the MTAPI handle for referencing
 * the actions implementing the job.
 *
 * This function converts a domain-wide job ID into a node-local job handle.
 *
 * On success, the action handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below.
 * Error code                    | Description
 * ----------------------------- | --------------------------------------------
 * \c MTAPI_ERR_JOB_INVALID      | The job ID does not refer to a valid action.
 * \c MTAPI_ERR_DOMAIN_NOTSHARED | The resource can't be shared by this domain.
 * \c MTAPI_ERR_NODE_NOTINIT     | The calling node is not initialized.
 *
 * \returns Handle to job with given \c job_id, invalid handle on error
 * \threadsafe
 * \ingroup JOBS
 */
mtapi_job_hndl_t mtapi_job_get(
  MTAPI_IN mtapi_job_id_t job_id,      /**< [in] Job id */
  MTAPI_IN mtapi_domain_t domain_id,   /**< [in] Domain id */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/* ---- TASKS -------------------------------------------------------------- */

/**
 * \defgroup TASKS Tasks
 *
 * \ingroup C_MTAPI
 *
 * Tasks representing pieces of work "in flight" (similar to a thread handles).
 *
 * A task is associated with a job object, which is associated with one or
 * more actions implementing the same job for load balancing purposes.
 * A task may optionally be associated with a task group. A task has
 * attributes, and an internal state. A task begins its lifetime with a call
 * to mtapi_task_start() or mtapi_task_enqueue(). A
 * task is referenced by a handle of type \c mtapi_task_hndl_t. The underlying
 * type of \c mtapi_task_hndl_t is implementation defined. Task handles may be
 * copied, assigned, and passed as arguments, but otherwise the application
 * should make no assumptions about the internal representation of a task
 * handle.
 *
 * Once a task is started, it is possible to wait for task completion from
 * other parts of the program.
 */

/**
 * This function initializes a task attributes object.
 *
 * A task attributes object is a container of task attributes. It is an
 * optional argument passed to mtapi_task_start() or mtapi_task_enqueue() to
 * specify non-default task attributes when starting a task.
 *
 * To set task attributes to non-default values, the application must allocate
 * a task attributes object of type \c mtapi_task_attributes_t and initialize
 * it with a call to mtapi_taskattr_init(). The application may call
 * mtapi_taskattr_set() to specify attribute values. Calls to
 * mtapi_taskattr_init() have no effect on task attributes after the task has
 * started. The \c mtapi_task_attributes_t object may safely be deleted by the
 * application after the task has started.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_PARAMETER    | Invalid attributes parameter.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_task_start(), mtapi_task_enqueue(), mtapi_taskattr_set()
 *
 * \notthreadsafe
 * \memberof mtapi_task_attributes_struct
 */
void mtapi_taskattr_init(
  MTAPI_OUT mtapi_task_attributes_t* attributes,
                                       /**< [out] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function sets task attribute values in a task attributes object.
 *
 * A task attributes object is a container of task attributes, optionally
 * passed to mtapi_task_start() or mtapi_task_enqueue() to specify non-default
 * task attributes when starting a task.
 *
 * attributes is a pointer to a task attributes object that was previously
 * initialized with a call to mtapi_taskattr_init(). Calls to
 * mtapi_taskattr_set() have no effect on task attributes after the task has
 * been created. The task attributes object may safely be deleted by the
 * application after the task has started.
 *
 * See the table below for a list of predefined attribute numbers and the
 * sizes of the attribute values. The application must set attribute_size to
 * the exact size in bytes of the attribute value. Additional attributes may
 * be defined by the implementation.
 *
 * MTAPI-defined task attributes:
 * <table>
 *   <tr>
 *     <th>Attribute num</th>
 *     <th>Description</th>
 *     <th>Data Type</th>
 *     <th>Default</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TASK_DETACHED</td>
 *     <td>Indicates if this is a detached task. A detached task is deleted by
 *         MTAPI runtime after execution. The task handle of detached tasks
 *         must not be used, i.e., it is not possible to wait for completion
 *         of dedicated detached tasks. But it is possible to add detached
 *         tasks to a group and wait for completion of the group.</td>
 *     <td>\c mtapi_boolean_t</td>
 *     <td>\c MTAPI_FALSE</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TASK_INSTANCES</td>
 *     <td>Indicates how many parallel instances of task shall be started by
 *         MTAPI. The default case is that each task is executed exactly once.
 *         Setting this value to n, the corresponding action code will be
 *         executed n times, in parallel, if the underlying hardware allows it.
 *         (see chapter 4.1.7 Multi-Instance Tasks, page 107)</td>
 *     <td>\c mtapi_uint_t</td>
 *     <td>1</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TASK_PRIORITY</td>
 *     <td>Indicates the prority this task should be run at. Priorities range
 *         from zero to one minus the maximum number of priorities specified at
 *         the call to mtapi_initialize().</td>
 *     <td>\c mtapi_uint_t</td>
 *     <td>0 (default priority)</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TASK_AFFINITY</td>
 *     <td>Indicates the affinity of this task. Affinities are manipulated by
 *         the matpi_affinity_init() and mtapi_affinity_set() calls.</td>
 *     <td>\c mtapi_affinity_t</td>
 *     <td>all workers</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TASK_USER_DATA</td>
 *     <td>Provides a pointer to some data required by the user during scheduling
 *         (e.g. in a MTAPI plugin).</td>
 *     <td>\c void*</td>
 *     <td>\c MTAPI_NULL</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TASK_COMPLETE_FUNCTION</td>
 *     <td>Pointer to a function being called when the task finishes.</td>
 *     <td>\c mtapi_task_complete_function_t</td>
 *     <td>\c MTAPI_NULL</td>
 *   </tr>
 * </table>
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_ATTR_READONLY | Attribute cannot be modified.
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_task_start(), mtapi_task_enqueue(), mtapi_taskattr_init()
 *
 * \notthreadsafe
 * \memberof mtapi_task_attributes_struct
 */
void mtapi_taskattr_set(
  MTAPI_INOUT mtapi_task_attributes_t* attributes,
                                       /**< [in, out] Pointer to attributes */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function schedules a task for execution.
 *
 * A task is associated with a job. A job is associated with one or more
 * actions. An action provides an action function, which is the executable
 * implementation of a job. If more than one action is associated with the job,
 * the runtime system chooses dynamically which action is used for execution
 * for load balancing purposes.
 *
 * \c task_id is an optional ID provided by the application for debugging
 * purposes. If not needed, it can be set to \c MTAPI_TASK_ID_NONE. The minimum
 * and maximum values for \c task_id may be derived from
 * \c MTAPI_MIN_USER_TASK_ID and \c MTAPI_MAX_USER_TASK_ID.
 *
 * \c job must be a handle to a job obtained by a previous call to
 * mtapi_job_get().
 *
 * If \c arguments_size is not zero, then arguments must point to data of
 * \c arguments_size bytes. The arguments will be transferred by the runtime
 * from the node where the action was created to the executing node if
 * necessary. Marshalling of arguments is not part of the MTAPI specification
 * and is implementation-defined.
 *
 * If \c attributes is \c MTAPI_NULL, the task will be started with default
 * attribute values. Otherwise \c attributes must point to a task attributes
 * object previously prepared using mtapi_taskattr_init() and
 * mtapi_taskattr_set(). The attributes of a task cannot be changed after the
 * task is created.
 *
 * \c group must be set to \c MTAPI_GROUP_NONE if the task is not part of a
 * task group. Otherwise \c group must be a group handle obtained by a previous
 * call to mtapi_group_create().
 *
 * On success, a task handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_TASK_LIMIT    | Exceeded maximum number of tasks allowed.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 * \c MTAPI_ERR_PARAMETER     | Invalid attributes parameter.
 * \c MTAPI_ERR_GROUP_INVALID | Argument is not a valid group handle.
 * \c MTAPI_ERR_JOB_INVALID   | The associated job is not valid.
 *
 * \see mtapi_job_get(), mtapi_taskattr_init(), mtapi_taskattr_set(),
 *      mtapi_group_create()
 *
 * \returns Handle to newly started task, invalid handle on error
 * \threadsafe
 * \ingroup TASKS
 */
mtapi_task_hndl_t mtapi_task_start(
  MTAPI_IN mtapi_task_id_t task_id,    /**< [in] Task id */
  MTAPI_IN mtapi_job_hndl_t job,       /**< [in] Job handle */
  MTAPI_IN void* arguments,            /**< [in] Pointer to arguments */
  MTAPI_IN mtapi_size_t arguments_size,/**< [in] Size of arguments */
  MTAPI_OUT void* result_buffer,       /**< [out] Pointer to result buffer */
  MTAPI_IN mtapi_size_t result_size,   /**< [in] Size of one result */
  MTAPI_IN mtapi_task_attributes_t* attributes,
                                       /**< [in] Pointer to attributes */
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle, may be
                                            \c MTAPI_GROUP_NONE */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                             may be \c MTAPI_NULL */
  );

/**
 * This function schedules a task for execution using a queue.
 *
 * A queue is a task associated with a job. A job is associated with one or
 * more actions. An action provides an action function, which is the executable
 * implementation of a job.
 *
 * \c task_id is an optional ID provided by the application for debugging
 * purposes. If not needed, it can be set to \c MTAPI_TASK_ID_NONE. The
 * underlying type of \c mtapi_task_id_t is implementation-defined. The
 * minimum and maximum values for \c task_id may be derived from
 * \c MTAPI_MIN_USER_TASK_ID and \c MTAPI_MAX_USER_TASK_ID.
 *
 * \c queue must be a handle to a queue obtained by a previous call to
 * mtapi_queue_create().
 *
 * If \c arguments_size is not zero, then arguments must point to data of
 * \c arguments_size bytes. The arguments will be transferred by the runtime
 * from the node where the action was created to the executing node.
 * Marshalling of arguments is not part of the MTAPI specification and is
 * implementation-defined.
 *
 * If \c attributes is \c MTAPI_NULL, the task will be started with default
 * attribute values. Otherwise \c attributes must point to a task attributes
 * object previously prepared using mtapi_taskattr_init() and
 * mtapi_taskattr_set(). Once a task has been enqueued, its attributes may not
 * be changed.
 *
 * \c group must be set to \c MTAPI_GROUP_NONE if the task is not part of a
 * task group. Otherwise \c group must be a group handle obtained by a
 * previous call to mtapi_group_create().
 *
 * On success, a task handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_TASK_LIMIT    | Exceeded maximum number of tasks allowed.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 * \c MTAPI_ERR_PARAMETER     | Invalid attributes parameter.
 * \c MTAPI_ERR_QUEUE_INVALID | Argument is not a valid queue handle.
 *
 * \see mtapi_queue_create(), mtapi_taskattr_init(), mtapi_taskattr_set(),
 *      mtapi_group_create()
 *
 * \returns Handle to newly enqueued task, invalid handle on error
 * \threadsafe
 * \ingroup TASKS
 */
mtapi_task_hndl_t mtapi_task_enqueue(
  MTAPI_IN mtapi_task_id_t task_id,    /**< [in] Task id */
  MTAPI_IN mtapi_queue_hndl_t queue,   /**< [in] Queue handle */
  MTAPI_IN void* arguments,            /**< [in] Pointer to arguments */
  MTAPI_IN mtapi_size_t arguments_size,/**< [in] Size of arguments */
  MTAPI_OUT void* result_buffer,       /**< [out] Pointer to result buffer */
  MTAPI_IN mtapi_size_t result_size,   /**< [in] Size of one result */
  MTAPI_IN mtapi_task_attributes_t* attributes,
                                       /**< [in] Pointer to task attributes */
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle, may be
                                            \c MTAPI_GROUP_NONE */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns a copy of the attribute value that corresponds to the given
 * \c attribute_num for the specified task.
 *
 * The attribute value will be returned in \c *attribute. Note that task
 * attributes may be queried but may not be changed after a task has been
 * created.
 *
 * \c task must be a valid handle to a task that was obtained by a previous
 * call to mtapi_task_start() or mtapi_task_enqueue().
 *
 * See mtapi_taskattr_set() for a list of predefined attribute numbers
 * and the sizes of the attribute values. The application is responsible for
 * allocating sufficient space for the returned attribute value and for
 * setting \c attribute_size to the exact size in bytes of the attribute value.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS and the attribute value is
 * returned in \c *attribute. On error, \c *status is set to the appropriate
 * error defined below and the attribute value is undefined. If this function
 * is called on a task that no longer exists, an \c MTAPI_ERR_TASK_INVALID
 * error code will be returned.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_PARAMETER    | Invalid attribute parameter.
 * \c MTAPI_ERR_TASK_INVALID | Argument is not a valid task handle.
 * \c MTAPI_ERR_ATTR_NUM     | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE    | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_task_start(), mtapi_task_enqueue(), mtapi_taskattr_set()
 *
 * \waitfree
 * \ingroup TASKS
 */
void mtapi_task_get_attribute(
  MTAPI_IN mtapi_task_hndl_t task,     /**< [in] Task handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_OUT void* attribute,           /**< [out] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,/**< [in] Size of attribute value */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function cancels a task and sets the task status to
 * \c MTAPI_TASK_CANCELLED.
 *
 * \c task must be a valid handle to a task that was obtained by a previous
 * call to mtapi_task_start() or mtapi_task_enqueue().
 *
 * If the execution of a task has not been started, the runtime system might
 * remove the task from the runtime-internal task queues. If task execution is
 * already running, an action function implemented in software can poll the
 * task status and react accordingly.
 *
 * Since the task is referenced by a task handle which can only be used
 * node-locally, a task can be canceled only on the node where the task was
 * created.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_TASK_INVALID | Argument is not a valid task handle.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_task_start(), mtapi_task_enqueue()
 *
 * \waitfree
 * \ingroup TASKS
 */
void mtapi_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,     /**< [in] Task handle */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function waits for the completion of the specified task.
 *
 * \c task must be a valid handle to a task that was obtained by a previous
 * call to mtapi_task_start() or mtapi_task_enqueue(). The task handle becomes
 * invalid on a successful wait, i.e., after the task had run to completion
 * and mtapi_task_wait() returns \c MTAPI_SUCCESS.
 *
 * \c timeout determines how long the function should wait for tasks already
 * started via that queue to finish. The underlying type of \c mtapi_timeout_t
 * is implementation-defined. If \c timeout is a constant 0 or the symbolic
 * constant \c MTAPI_NOWAIT, this function does not block and returns
 * immediately. If \c timeout is set to \c MTAPI_INFINITE the function may
 * block infinitely. Other values for \c timeout and the units of measure are
 * implementation-defined.
 *
 * Results of completed tasks can be obtained via \c result_buffer associated
 * with the task. The size of the buffer has to be equal to the result size
 * written in the action code. If the result is not needed by the calling
 * code, \c result_buffer may be set to \c MTAPI_NULL. For multi-instance
 * tasks, the result buffer is filled by an array of all the task instances'
 * results. I.e., the result buffer has to be allocated big enough (number of
 * instances times size of result).
 *
 * Calling mtapi_task_wait() more than once for the same task results in
 * undefined behavior.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below. If this function is called on
 * a task that no longer exists, an \c MTAPI_ERR_TASK_INVALID error code will
 * be returned. \c status will be \c MTAPI_ERR_ARG_SIZE or
 * \c MTAPI_ERR_RESULT_SIZE if the sizes of arguments or result buffer do
 * not match.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_TASK_INVALID</td>
 *     <td>Argument is not a valid task handle.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TIMEOUT</td>
 *     <td>Timeout was reached.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_PARAMETER</td>
 *     <td>Invalid timeout parameter.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_TASK_CANCELLED</td>
 *     <td>The task has been canceled because of mtapi_task_cancel() was
 *         called before the task was executed or the error code was set to
 *         \c MTAPI_ERR_TASK_CANCELLED by mtapi_context_status_set() in the
 *         action function.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_WAIT_PENDING</td>
 *     <td>mtapi_task_wait() had already been called for the same task and the
 *         first wait call is still pending.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_CANCELLED</td>
 *     <td>Action execution was canceled by the action function
 *         (mtapi_context_status_set()).</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_FAILED</td>
 *     <td>Error set by action function (mtapi_context_status_set()).</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_DELETED</td>
 *     <td>All actions associated with the task have been deleted before the
 *         execution of the task was started or the error code has been set in
 *         the action function to \c MTAPI_ERR_ACTION_DELETED by
 *         mtapi_context_status_set().</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ARG_SIZE</td>
 *     <td>The size of the arguments expected by action differs from arguments
 *         size of the caller.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_RESULT_SIZE</td>
 *     <td>The size of the result buffer expected by action differs from
 *         result buffer size of the caller.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_task_start(), mtapi_task_enqueue(), mtapi_task_wait(),
 *      mtapi_task_cancel(), mtapi_context_status_set()
 *
 * \threadsafe
 * \ingroup TASKS
 */
void mtapi_task_wait(
  MTAPI_IN mtapi_task_hndl_t task,     /**< [in] Task handle */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/* ---- TASK GROUPS -------------------------------------------------------- */

/**
 * \defgroup TASK_GROUPS Task Groups
 *
 * \ingroup C_MTAPI
 *
 * Facilities for synchronizing on groups of tasks.
 *
 * This concept is similar to barrier synchronization of threads. MTAPI
 * specifies a minimal task group feature set in order to allow small and
 * efficient implementations.
 */

/**
 * This function initializes a group attributes object.
 *
 * A group attributes object is a container of group attributes. It is an
 * optional argument passed to mtapi_group_create() to specify non-default
 * group attributes when creating a task group.
 *
 * To set group attributes to non-default values, the application must
 * allocate a group attributes object of type \c mtapi_group_attributes_t and
 * initialize it with a call to mtapi_groupattr_init(). The application may
 * call mtapi_groupattr_set() to specify attribute values. Calls to
 * mtapi_groupattr_init() have no effect on group attributes after the group
 * has been created. The \c mtapi_group_attributes_t object may safely be
 * deleted by the application after the call to mtapi_group_create().
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_PARAMETER    | Invalid attributes parameter.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 *
 * \see mtapi_group_create(), mtapi_groupattr_set()
 *
 * \notthreadsafe
 * \memberof mtapi_group_attributes_struct
 */
void mtapi_groupattr_init(
  MTAPI_OUT mtapi_group_attributes_t* attributes,
                                       /**< [out] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function sets group attribute values in a group attributes object.
 *
 * A group attributes object is a container of group attributes, optionally
 * passed to mtapi_group_create() to specify non-default group attributes when
 * creating a task group.
 *
 * \c attributes is a pointer to a group attributes object that was previously
 * initialized with a call to mtapi_groupattr_init(). Calls to
 * mtapi_groupattr_set() have no effect on group attributes after the group
 * has been created. The group attributes object may safely be deleted by the
 * application after the call to mtapi_group_create().
 *
 * See the table below for a list of predefined attribute numbers and the
 * sizes of the attribute values. The application must set \c attribute_size to
 * the exact size in bytes of the attribute value.
 * Additional attributes may be defined by the implementation.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_ATTR_READONLY | Attribute cannot be modified.
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_group_create(), mtapi_groupattr_init()
 *
 * \notthreadsafe
 * \memberof mtapi_group_attributes_struct
 */
void mtapi_groupattr_set(
  MTAPI_INOUT mtapi_group_attributes_t* attributes,
                                       /**< [in,out] Pointer to attributes */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_IN void* attribute,            /**< [in] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function creates a task group and returns a handle to the group.
 *
 * After a group is created, a task may be associated with a group when the
 * task is started with mtapi_task_start() or mtapi_task_enqueue().
 *
 * \c group_id is an optional ID provided by the application for debugging
 * purposes. If not needed, it can be set to \c MTAPI_GROUP_ID_NONE. The
 * underlying type of \c mtapi_group_id_t is implementation-defined. The
 * minimum and maximum values for \c group_id may be derived from
 * \c MTAPI_MIN_USER_GROUP_ID and \c MTAPI_MAX_USER_GROUP_ID.
 *
 * If \c attributes is \c MTAPI_NULL, the group will be created with default
 * attribute values. Otherwise \c attributes must point to a group attributes
 * object previously prepared using mtapi_groupattr_init() and
 * mtapi_groupattr_set().
 *
 * On success, a group handle is returned and \c *status is set to
 * \c MTAPI_SUCCESS. On error, \c *status is set to the appropriate error
 * defined below.
 * Error code                | Description
 * ------------------------- | ------------------------------------------------
 * \c MTAPI_ERR_GROUP_LIMIT  | Exceeded maximum number of groups allowed.
 * \c MTAPI_ERR_NODE_NOTINIT | The calling node is not initialized.
 * \c MTAPI_ERR_PARAMETER    | Invalid attributes parameter.
 *
 * \see mtapi_task_start(), mtapi_task_enqueue(), mtapi_groupattr_init(),
 *      mtapi_groupattr_set()
 *
 * \returns Handle to newly created group, invalid handle on error
 * \threadsafe
 * \memory This function allocates a new queue for tracking completion of the
 *         tasks belonging to the group.
 * \ingroup TASK_GROUPS
 */
mtapi_group_hndl_t mtapi_group_create(
  MTAPI_IN mtapi_group_id_t group_id,  /**< [in] Group id */
  MTAPI_IN mtapi_group_attributes_t* attributes,
                                       /**< [in] Pointer to attributes */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Changes the value of the attribute that corresponds to the given
 * \c attribute_num for the specified task group.
 *
 * \c attribute must point to the attribute value, and \c attribute_size must
 * be set to the exact size of the attribute value. See mtapi_groupattr_set()
 * for a list of predefined attribute numbers and the sizes of their values.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_GROUP_INVALID | Argument is not a valid group handle.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_groupattr_set()
 *
 * \notthreadsafe
 * \ingroup TASK_GROUPS
 */
void mtapi_group_set_attribute(
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_OUT void* attribute,           /**< [out] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,
                                       /**< [in] Size of attribute value. may
                                            be 0, attribute is interpreted as
                                            value in that case */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * Returns the attribute value that corresponds to the given \c attribute_num
 * for this task group.
 *
 * \c attribute must point to the location where the attribute value is to be
 * returned, and \c attribute_size must be set to the exact size of the
 * attribute value. See mtapi_groupattr_set() for a list of predefined
 * attribute numbers and the sizes of their values.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS and the attribute value is
 * returned in \c *attribute. On error, \c *status is set to the appropriate
 * error defined below and \c *attribute is undefined. If this function is
 * called on a group that no longer exists, an \c MTAPI_ERR_GROUP_INVALID
 * error code will be returned.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_PARAMETER     | Invalid attribute parameter.
 * \c MTAPI_ERR_GROUP_INVALID | Argument is not a valid group handle.
 * \c MTAPI_ERR_ATTR_NUM      | Unknown attribute number.
 * \c MTAPI_ERR_ATTR_SIZE     | Incorrect attribute size.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \see mtapi_groupattr_set()
 *
 * \waitfree
 * \ingroup TASK_GROUPS
 */
void mtapi_group_get_attribute(
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle */
  MTAPI_IN mtapi_uint_t attribute_num, /**< [in] Attribute id */
  MTAPI_OUT void* attribute,           /**< [out] Pointer to attribute value */
  MTAPI_IN mtapi_size_t attribute_size,/**< [in] Size of attribute value */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function waits for the completion of a task group.
 *
 * Tasks may be associated with groups when the tasks are started. Each task
 * is associated with one or more actions. This function returns when all the
 * associated action functions have completed or canceled. The group handle
 * becomes invalid if this function returns \c MTAPI_SUCCESS.
 *
 * \c timeout determines how long the function should wait for tasks already
 * started in the group to finish. The underlying type of \c mtapi_timeout_t is
 * implementation-defined. If \c timeout is a constant 0 or the symbolic
 * constant \c MTAPI_NOWAIT, this function does not block and returns
 * immediately. If \c timeout is set to \c MTAPI_INFINITE the function may
 * block infinitely. Other values for \c timeout and the units of measure are
 * implementation defined.
 *
 * To obtain results from a task, the application should call
 * mtapi_group_wait_any() instead.
 *
 * During execution, an action function may optionally call
 * mtapi_context_status_set() to set a task status that will be returned in
 * this function in \c *status. If multiple action functions set different task
 * status values, it is implementation-defined which of those is returned in
 * mtapi_group_wait_all(). The following task status values may be set by an
 * action function: \c MTAPI_ERR_TASK_CANCELLED, \c MTAPI_ERR_ACTION_CANCELLED,
 * \c MTAPI_ERR_ACTION_FAILED, and \c MTAPI_ERR_ACTION_DELETED.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TIMEOUT</td>
 *     <td>Timeout was reached.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_GROUP_INVALID</td>
 *     <td>Argument is not a valid task handle.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_WAIT_PENDING</td>
 *     <td>mtapi_group_wait_all() had already been called for the same group
 *         and the first wait call is still pending.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_PARAMETER</td>
 *     <td>Invalid timeout parameter.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ARG_SIZE</td>
 *     <td>The size of the arguments expected by action differs from arguments
 *         size of the caller.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_RESULT_SIZE</td>
 *     <td>The size of the result buffer expected by action differs from
 *         result buffer size of the caller.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_GROUP_COMPLETED</td>
 *     <td>Group completed, i.e., there are no more task to wait for in
 *         the group.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_TASK_CANCELLED</td>
 *     <td>At least one task has been canceled because of mtapi_task_cancel()
 *         was called before the task was executed or the error code was set
 *         to \c MTAPI_ERR_TASK_CANCELLED by mtapi_context_status_set() in the
 *         action function.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_CANCELLED</td>
 *     <td>The action execution of at least one task was canceled by the
 *         action function (mtapi_context_status_set()).</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_FAILED</td>
 *     <td>Error set by at least one action function
 *         (mtapi_context_status_set()).</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_DELETED</td>
 *     <td>All actions associated with the task have been deleted before the
 *         execution of the task was started or the error code has been set in
 *         the action function to \c MTAPI_ERR_ACTION_DELETED by
 *         mtapi_context_status_set().</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_group_wait_any(), mtapi_context_status_set(), mtapi_task_cancel()
 *
 * \threadsafe
 * \ingroup TASK_GROUPS
 */
void mtapi_group_wait_all(
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                             may be \c MTAPI_NULL */
  );

/**
 * This function waits for the completion of any task in a task group.
 *
 * Tasks may be associated with groups when the tasks are started. Each task
 * is associated with one or more actions. This function returns when any of
 * the associated action functions have completed or have been canceled.
 *
 * The group handle does not become invalid if this function returns
 * \c MTAPI_SUCCESS. The group handle becomes invalid if this function returns
 * \c MTAPI_GROUP_COMPLETED.
 *
 * \c group must be a valid group handle obtained by a previous call to
 * mtapi_group_create().
 *
 * Action functions may pass results that will be available in \c *result after
 * mtapi_group_wait_any() returns. If the results are not needed, \c result may
 * be set to \c MTAPI_NULL. Otherwise, \c result must point to an area in
 * memory of sufficient size to hold the array of results from the completed
 * task(s). The size of the result buffer is given in the argument
 * \c result_buffer_size that the runtime passes to an action function upon
 * invocation.
 *
 * \c timeout determines how long the function should wait for a task in the
 * group to finish. The underlying type of \c mtapi_timeout_t is
 * implementation-defined. If \c timeout is a constant 0 or the symbolic
 * constant \c MTAPI_NOWAIT, this function does not block and returns
 * immediately. If \c timeout is set to \c MTAPI_INFINITE the function may
 * block infinitely. Other values for \c timeout and the units of measure are
 * implementation defined.
 *
 * During execution, an action function may optionally call
 * mtapi_context_status_set() to set a task status that will be returned in
 * this function in \c *status. The following task status values may be set by
 * an action function: \c MTAPI_ERR_TASK_CANCELLED,
 * \c MTAPI_ERR_ACTION_CANCELLED, \c MTAPI_ERR_ACTION_FAILED, and
 * \c MTAPI_ERR_ACTION_DELETED.
 *
 * On success, \c *status is either set to \c MTAPI_SUCCESS if one of the
 * tasks in the group completed or to \c MTAPI_GROUP_COMPLETED if all tasks of
 * the group have completed and successfully waited for. On error, \c *status
 * is set to the appropriate error defined below.
 * <table>
 *   <tr>
 *     <th>Error code</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_TIMEOUT</td>
 *     <td>Timeout was reached.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_GROUP_INVALID</td>
 *     <td>Argument is not a valid task handle.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_PARAMETER</td>
 *     <td>Invalid timeout parameter.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ARG_SIZE</td>
 *     <td>The size of the arguments expected by action differs from arguments
 *         size of the caller.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_RESULT_SIZE</td>
 *     <td>The size of the result buffer expected by action differs from
 *         result buffer size of the caller.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_GROUP_COMPLETED</td>
 *     <td>Group completed, i.e., there are no more tasks to wait for in
 *         the group.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_TASK_CANCELLED</td>
 *     <td>The task has been canceled because mtapi_task_cancel() was called
 *         before the task was executed, or the error code was set to
 *         \c MTAPI_ERR_TASK_CANCELLED by mtapi_context_status_set() in the
 *         action code.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_CANCELLED</td>
 *     <td>Action execution was canceled by the action function
 *         (mtapi_context_status_set()).</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_FAILED</td>
 *     <td>Error set by action function (mtapi_context_status_set()).</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_ACTION_DELETED</td>
 *     <td>All actions associated with the task have been deleted before the
 *         execution of the task was started or the error code has been set in
 *         the action code to \c MTAPI_ERR_ACTION_DELETED
 *         by mtapi_context_status_set().</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_group_create(), mtapi_context_status_set(), mtapi_task_cancel()
 *
 * \threadsafe
 * \ingroup TASK_GROUPS
 */
void mtapi_group_wait_any(
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle */
  MTAPI_OUT void** result,             /**< [out] Pointer to result buffer
                                            supplied at task start */
  MTAPI_IN mtapi_timeout_t timeout,    /**< [in] Timeout duration in
                                            milliseconds */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );

/**
 * This function deletes a task group.
 *
 * Deleting a group does not have any influence on tasks belonging to the
 * group. Adding tasks to a group that is already deleted will result in an
 * \c MTAPI_ERR_GROUP_INVALID error.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                 | Description
 * -------------------------- | -----------------------------------------------
 * \c MTAPI_ERR_GROUP_INVALID | Argument is not a valid group handle.
 * \c MTAPI_ERR_NODE_NOTINIT  | The calling node is not initialized.
 *
 * \threadsafe
 * \ingroup TASK_GROUPS
 */
void mtapi_group_delete(
  MTAPI_IN mtapi_group_hndl_t group,   /**< [in] Group handle */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
  );


/**
 * \internal
 *
 * \defgroup INTERNAL Internal Implementation
 *
 * \ingroup C_MTAPI
 *
 * This section describes the internal implementation of the MTAPI interface.
 */


#ifdef __cplusplus
}
#endif

#endif // EMBB_MTAPI_C_MTAPI_H_
