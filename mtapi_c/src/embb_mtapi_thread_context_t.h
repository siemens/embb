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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_THREAD_CONTEXT_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_THREAD_CONTEXT_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/base.h>

#include <embb_mtapi_task_visitor_function_t.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_task_queue_t_fwd.h>
#include <embb_mtapi_node_t_fwd.h>
#include <embb_mtapi_scheduler_t_fwd.h>

/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Thread context class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_thread_context_struct {
  embb_mutex_t work_available_mutex;
  embb_condition_t work_available;
  embb_thread_t thread;
  embb_tss_t tss_id;
  embb_atomic_int is_sleeping;

  embb_mtapi_node_t* node;
  embb_mtapi_task_queue_t** queue;
  embb_mtapi_task_queue_t** private_queue;

  mtapi_uint_t priorities;
  mtapi_uint_t worker_index;
  mtapi_uint_t core_num;
  embb_atomic_int run;
  mtapi_status_t status;
  mtapi_boolean_t is_initialized;
  mtapi_boolean_t is_main_thread;

  embb_thread_priority_t thread_priority;
};

#include <embb_mtapi_thread_context_t_fwd.h>

/**
 * Constructor using attributes from node and a given core number.
 * \memberof embb_mtapi_thread_context_struct
 * \returns MTAPI_TRUE if successful, MTAPI_FALSE on error
 */
mtapi_boolean_t embb_mtapi_thread_context_initialize(
  embb_mtapi_thread_context_t* that,
  embb_mtapi_node_t* node,
  mtapi_uint_t worker_index,
  mtapi_uint_t core_num,
  embb_thread_priority_t priority);

/**
 * Destructor.
 * \memberof embb_mtapi_thread_context_struct
 */
void embb_mtapi_thread_context_finalize(embb_mtapi_thread_context_t* that);

/**
 * Start worker thread.
 * \memberof embb_mtapi_thread_context_struct
 * \returns MTAPI_TRUE if successful, MTAPI_FALSE on error
 */
mtapi_boolean_t embb_mtapi_thread_context_start(
  embb_mtapi_thread_context_t* that,
  embb_mtapi_scheduler_t * scheduler);

/**
 * Stop worker thread.
 * \memberof embb_mtapi_thread_context_struct
 */
void embb_mtapi_thread_context_stop(embb_mtapi_thread_context_t* that);

/**
 * Apply visitor function to all tasks in the queues of the context.
 * \memberof embb_mtapi_thread_context_struct
 */
mtapi_boolean_t embb_mtapi_thread_context_process_tasks(
  embb_mtapi_thread_context_t* that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data);


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_THREAD_CONTEXT_T_H_
