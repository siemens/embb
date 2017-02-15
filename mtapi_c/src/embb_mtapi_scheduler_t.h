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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_SCHEDULER_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_SCHEDULER_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_task_visitor_function_t.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_queue_t_fwd.h>
#include <embb_mtapi_group_t_fwd.h>
#include <embb_mtapi_thread_context_t_fwd.h>
#include <embb_mtapi_task_t_fwd.h>
#include <embb_mtapi_node_t_fwd.h>
typedef int (embb_mtapi_scheduler_worker_func_t)(void * args);

/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Scheduler mode type
 *
 * \ingroup INTERNAL
 */
enum embb_mtapi_scheduler_mode_enum {
  // Victim Higher Priority First. Steal if at least one local queue is empty.
  WORK_STEAL_VHPF = 0,
  // Local First. Steal if all local queues are empty.
  WORK_STEAL_LF   = 1,

  NUM_SCHEDULER_MODES
};

/**
 * Scheduler mode type.
 * \memberof embb_mtapi_scheduler_struct
 */
typedef enum embb_mtapi_scheduler_mode_enum embb_mtapi_scheduler_mode_t;

/**
 * \internal
 * Scheduler class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_scheduler_struct {
  mtapi_uint_t worker_count;
  embb_mtapi_thread_context_t * worker_contexts;
  mtapi_action_attributes_t attributes;

  // using enum value instead of function pointer to simplify testing
  // for modes, like
  //   if (scheduler->mode == WORK_STEAL_VHPF)
  embb_mtapi_scheduler_mode_t mode;

  embb_atomic_int affine_task_counter;
};

#include <embb_mtapi_scheduler_t_fwd.h>

/**
 * The default worker thread function used by the scheduler. Implements
 * scheduling strategy as defined by scheduler mode.
 * \memberof embb_mtapi_scheduler_struct
 */
int embb_mtapi_scheduler_worker(void * arg);

/**
 * \internal
 * Resolve the worker function for a scheduler.
 * \memberof embb_mtapi_scheduler_struct
 * \ingroup INTERNAL
 */
embb_mtapi_scheduler_worker_func_t *
embb_mtapi_scheduler_worker_func(embb_mtapi_scheduler_t * that);

/**
 * Wait for a given task and schedule new ones while waiting.
 * \memberof embb_mtapi_scheduler_struct
 */
mtapi_boolean_t embb_mtapi_scheduler_wait_for_task(
  embb_mtapi_task_t * task,
  mtapi_timeout_t timeout);

/**
 * Get a task from any of the available queues.
 * \memberof embb_mtapi_scheduler_struct
 */
embb_mtapi_task_t * embb_mtapi_scheduler_get_next_task(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context);

/**
 * Set the scheduling strategy.
 * \memberof embb_mtapi_scheduler_struct
 */
void embb_mtapi_scheduler_set_mode(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_scheduler_mode_t mode);

/**
 * Determines the thread context associated with the current thread.
 * \memberof embb_mtapi_scheduler_struct
 */
embb_mtapi_thread_context_t * embb_mtapi_scheduler_get_current_thread_context(
  embb_mtapi_scheduler_t * that);

/**
 * Processes finished task.
 * Notifies associated group and queue and deletes task if it is detached.
 */
void embb_mtapi_scheduler_finalize_task(
  embb_mtapi_task_t * task,
  embb_mtapi_node_t * node,
  mtapi_task_state_t next_task_state);

/**
 * Executes the given task if the thread context is valid.
 * \memberof embb_mtapi_scheduler_struct
 */
mtapi_boolean_t embb_mtapi_scheduler_execute_task(
  embb_mtapi_task_t * task,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context);

/**
 * Fetches and executes a single task if the thread context is valid,
 * yields otherwise.
 * \memberof embb_mtapi_scheduler_struct
 */
void embb_mtapi_scheduler_execute_task_or_yield(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_node_t * node,
  embb_mtapi_thread_context_t * thread_context);

/**
 * operator new.
 * \memberof embb_mtapi_scheduler_struct
 * \returns pointer to the scheduler oder MTAPI_NULL on error
 */
embb_mtapi_scheduler_t * embb_mtapi_scheduler_new();

/**
 * operator delete.
 * \memberof embb_mtapi_scheduler_struct
 */
void embb_mtapi_scheduler_delete(embb_mtapi_scheduler_t * that);

/**
 * Default constructor. Using default scheduling strategy.
 * \memberof embb_mtapi_scheduler_struct
 * \returns MTAPI_TRUE on success, MTAPI_FALSE on error
 */
mtapi_boolean_t embb_mtapi_scheduler_initialize(embb_mtapi_scheduler_t * that);

/**
 * Constructor allowing manual setting of scheduling strategy.
 * \memberof embb_mtapi_scheduler_struct
 * \returns MTAPI_TRUE on success, MTAPI_FALSE on error
 */
mtapi_boolean_t embb_mtapi_scheduler_initialize_with_mode(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_scheduler_mode_t mode);

/**
 * Destructor.
 * \memberof embb_mtapi_scheduler_struct
 */
void embb_mtapi_scheduler_finalize(embb_mtapi_scheduler_t * that);

/**
 * Apply visitor to all Tasks in the queues of the scheduler.
 * \memberof embb_mtapi_scheduler_struct
 */
mtapi_boolean_t embb_mtapi_scheduler_process_tasks(
  embb_mtapi_scheduler_t* that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data);

/**
 * Put a Task into one of the queues of the scheduler, the tasks state needs
 * to be either MTAPI_TASK_SCHEDULED or MTAPI_TASK_RETAINED.
 * \memberof embb_mtapi_scheduler_struct
 */
mtapi_boolean_t embb_mtapi_scheduler_schedule_task(
  embb_mtapi_scheduler_t * that,
  embb_mtapi_task_t * task);


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_SCHEDULER_T_H_
