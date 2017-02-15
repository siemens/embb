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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_GROUP_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_GROUP_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_pool_template.h>
#include <embb_mtapi_task_queue_t.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_node_t_fwd.h>


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Group class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_group_struct {
  mtapi_group_hndl_t handle;

  mtapi_group_id_t group_id;
  embb_atomic_int deleted;
  embb_atomic_int num_tasks;
  mtapi_group_attributes_t attributes;
  embb_mtapi_task_queue_t queue;
};

#include <embb_mtapi_group_t_fwd.h>

/**
 * Default constructor.
 * \memberof embb_mtapi_group_struct
 */
void embb_mtapi_group_initialize(embb_mtapi_group_t * that);

/**
 * Destructor.
 * \memberof embb_mtapi_group_struct
 */
void embb_mtapi_group_finalize(embb_mtapi_group_t * that);


/* ---- POOL DECLARATION --------------------------------------------------- */

embb_mtapi_pool(group)


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_GROUP_T_H_
