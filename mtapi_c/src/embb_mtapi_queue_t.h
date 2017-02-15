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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_QUEUE_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_QUEUE_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_pool_template.h>
#include <embb/base/c/mutex.h>
#include <embb_mtapi_task_queue_t.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Queue class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_queue_struct {
  mtapi_queue_hndl_t handle;

  mtapi_queue_id_t queue_id;
  embb_atomic_char enabled;
  mtapi_job_hndl_t job_handle;
  mtapi_queue_attributes_t attributes;

  embb_atomic_int num_tasks;
  mtapi_affinity_t ordered_affinity;
  embb_mtapi_task_queue_t retained_tasks;
  embb_mtapi_task_queue_t ordered_tasks;
  embb_atomic_int ordered_task_executing;
};

#include <embb_mtapi_queue_t_fwd.h>

/**
 * Default constructor.
 * \memberof embb_mtapi_queue_struct
 */
void embb_mtapi_queue_initialize(embb_mtapi_queue_t* that);

/**
 * Constructor with attributes and job.
 * \memberof embb_mtapi_queue_struct
 */
void embb_mtapi_queue_initialize_with_attributes_and_job(
  embb_mtapi_queue_t* that,
  mtapi_queue_attributes_t* attributes,
  mtapi_job_hndl_t job);

/**
 * Destructor.
 * \memberof embb_mtapi_queue_struct
 */
void embb_mtapi_queue_finalize(embb_mtapi_queue_t* that);

/**
 * Notify queue that an associated Task has started.
 * \memberof embb_mtapi_queue_struct
 */
void embb_mtapi_queue_task_started(embb_mtapi_queue_t* that);

/**
 * Notify queue that an associated Task has finished.
 * \memberof embb_mtapi_queue_struct
 */
void embb_mtapi_queue_task_finished(embb_mtapi_queue_t* that);

/**
 * Start ordered task on queue, returns 0 if a task is already running.
 * \memberof embb_mtapi_queue_struct
 */
int embb_mtapi_queue_ordered_task_start(embb_mtapi_queue_t* that);

/**
 * Finish ordered task on queue.
 * \memberof embb_mtapi_queue_struct
 */
void embb_mtapi_queue_ordered_task_finish(embb_mtapi_queue_t* that);


/* ---- POOL DECLARATION --------------------------------------------------- */

embb_mtapi_pool(queue)


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_QUEUE_T_H_
