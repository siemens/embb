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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_NODE_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_NODE_T_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_log.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_job_t_fwd.h>
#include <embb_mtapi_scheduler_t_fwd.h>
#include <embb_mtapi_action_t_fwd.h>
#include <embb_mtapi_group_t_fwd.h>
#include <embb_mtapi_task_t_fwd.h>
#include <embb_mtapi_queue_t_fwd.h>


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Node class.
 *
 * The node class is used as a singleton and is the root of this
 * MTAPI implementation.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_node_struct {
  mtapi_domain_t domain_id;
  mtapi_node_t node_id;
  mtapi_node_attributes_t attributes;
  mtapi_info_t info;
  embb_mtapi_scheduler_t * scheduler;
  embb_mtapi_job_t * job_list;
  embb_mtapi_action_pool_t * action_pool;
  embb_mtapi_group_pool_t * group_pool;
  embb_mtapi_task_pool_t * task_pool;
  embb_mtapi_queue_pool_t * queue_pool;
  embb_atomic_int is_scheduler_running;
  mtapi_affinity_t affinity_all;
};

#include <embb_mtapi_node_t_fwd.h>

/**
 * Checks if the node singleton was initialized already.
 * \memberof embb_mtapi_node_struct
 */
mtapi_boolean_t embb_mtapi_node_is_initialized();

/**
 * Retrieves the previously initialized node instance, but returns MTAPI_NULL
 * if the node singleton is not yet created.
 * \memberof embb_mtapi_node_struct
 */
embb_mtapi_node_t* embb_mtapi_node_get_instance();


#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_NODE_T_H_
