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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_TASK_CONTEXT_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_TASK_CONTEXT_T_H_

#include <embb/mtapi/c/mtapi.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_thread_context_t_fwd.h>
#include <embb_mtapi_task_t_fwd.h>


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Task context class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_task_context_struct {
  mtapi_uint_t instance_num;
  mtapi_uint_t num_instances;
  embb_mtapi_task_t* task;
  embb_mtapi_thread_context_t* thread_context;
};

#include <embb_mtapi_task_context_t_fwd.h>

/**
 * Constructor from a thread_context and a task.
 * \memberof embb_mtapi_task_context_struct
 */
void embb_mtapi_task_context_initialize_with_thread_context_and_task(
  embb_mtapi_task_context_t* that,
  embb_mtapi_thread_context_t* thread_context,
  embb_mtapi_task_t* task);

/**
 * Destructor.
 * \memberof embb_mtapi_task_context_struct
 */
void embb_mtapi_task_context_finalize(embb_mtapi_task_context_t* that);


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_TASK_CONTEXT_T_H_
