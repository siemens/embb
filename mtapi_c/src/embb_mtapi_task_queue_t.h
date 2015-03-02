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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_TASK_QUEUE_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_TASK_QUEUE_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_spinlock_t.h>
#include <embb_mtapi_task_visitor_function_t.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_task_t_fwd.h>


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Task queue class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_task_queue_struct {
  embb_mtapi_task_t ** task_buffer;
  mtapi_uint_t tasks_available;
  mtapi_uint_t get_task_position;
  mtapi_uint_t put_task_position;
  mtapi_queue_attributes_t attributes;
  embb_mtapi_spinlock_t lock;
};

#include <embb_mtapi_task_queue_t_fwd.h>

/**
 * Default constructor.
 * \memberof embb_mtapi_task_queue_struct
 */
void embb_mtapi_task_queue_initialize(embb_mtapi_task_queue_t* that);

/**
 * Constructor with configurable capacity.
 * \memberof embb_mtapi_task_queue_struct
 */
void embb_mtapi_task_queue_initialize_with_capacity(
  embb_mtapi_task_queue_t* that,
  mtapi_uint_t capacity);

/**
 * Destructor.
 * \memberof embb_mtapi_task_queue_struct
 */
void embb_mtapi_task_queue_finalize(embb_mtapi_task_queue_t* that);

/**
 * Pop a task from the queue. Returns MTAPI_NULL if the queue is empty.
 * \memberof embb_mtapi_task_queue_struct
 */
embb_mtapi_task_t * embb_mtapi_task_queue_pop(embb_mtapi_task_queue_t* that);

/**
 * Push a task into the queue. Returns MTAPI_TRUE if successfull and
 * MTAPI_FALSE if the queue is full or cannot be locked in time.
 * \memberof embb_mtapi_task_queue_struct
 */
mtapi_boolean_t embb_mtapi_task_queue_push(
  embb_mtapi_task_queue_t* that,
  embb_mtapi_task_t * task);


/**
 * Process all elements of the task queue using the given functor.
 * \memberof embb_mtapi_task_queue_struct
 */
mtapi_boolean_t embb_mtapi_task_queue_process(
  embb_mtapi_task_queue_t * that,
  embb_mtapi_task_visitor_function_t process,
  void * user_data);


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_TASK_QUEUE_T_H_
