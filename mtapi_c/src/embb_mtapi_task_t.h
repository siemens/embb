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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_TASK_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_TASK_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_pool_template.h>
#include <embb/base/c/mutex.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_task_context_t_fwd.h>


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Task class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_task_struct {
  mtapi_task_hndl_t handle;

  mtapi_task_id_t task_id;
  mtapi_job_hndl_t job;
  const void * arguments;
  mtapi_size_t arguments_size;
  void * result_buffer;
  mtapi_size_t result_size;
  mtapi_task_attributes_t attributes;
  mtapi_group_hndl_t group;
  mtapi_queue_hndl_t queue;

  mtapi_action_hndl_t action;
  embb_atomic_int state;
  embb_atomic_unsigned_int current_instance;
  embb_atomic_unsigned_int instances_todo;

  mtapi_status_t error_code;

  struct embb_mtapi_task_struct * next;
};

#include <embb_mtapi_task_t_fwd.h>

/**
 * Pooled operator new.
 * \memberof embb_mtapi_task_struct
 */
embb_mtapi_task_t* embb_mtapi_task_new(embb_mtapi_task_pool_t* pool);

/**
 * Pooled operator delete.
 * \memberof embb_mtapi_task_struct
 */
void embb_mtapi_task_delete(
  embb_mtapi_task_t* that,
  embb_mtapi_task_pool_t* pool);

/**
 * Default constructor.
 * \memberof embb_mtapi_task_struct
 */
void embb_mtapi_task_initialize(embb_mtapi_task_t* that);

/**
 * Destructor.
 * \memberof embb_mtapi_task_struct
 */
void embb_mtapi_task_finalize(embb_mtapi_task_t* that);

/**
 * Execute the action function of a task within the given context. Notfies
 * the associated task group or queue if set. Deletes the task if it is
 * detached.
 * \memberof embb_mtapi_task_struct
 */
mtapi_boolean_t embb_mtapi_task_execute(
  embb_mtapi_task_t* that,
  embb_mtapi_task_context_t * context,
  mtapi_task_state_t * new_task_state);

/**
 * Set the current task state.
 * \memberof embb_mtapi_task_struct
 */
void embb_mtapi_task_set_state(
  embb_mtapi_task_t* that,
  mtapi_task_state_t state);


/* ---- POOL DECLARATION --------------------------------------------------- */

embb_mtapi_pool(task)


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_TASK_T_H_
