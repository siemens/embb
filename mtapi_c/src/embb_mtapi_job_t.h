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

#ifndef MTAPI_C_SRC_EMBB_MTAPI_JOB_T_H_
#define MTAPI_C_SRC_EMBB_MTAPI_JOB_T_H_

#include <embb/mtapi/c/mtapi_ext.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ---- FORWARD DECLARATIONS ----------------------------------------------- */

#include <embb_mtapi_node_t_fwd.h>
#include <embb_mtapi_action_t_fwd.h>


/* ---- CLASS DECLARATION -------------------------------------------------- */

/**
 * \internal
 * Job class.
 *
 * \ingroup INTERNAL
 */
struct embb_mtapi_job_struct {
  mtapi_job_hndl_t handle;

  mtapi_domain_t domain_id;
  mtapi_node_t node_id;
  mtapi_uint_t num_actions;
  mtapi_uint_t max_actions;
  mtapi_action_hndl_t* actions;

  mtapi_ext_job_attributes_t attributes;
};

#include <embb_mtapi_job_t_fwd.h>

/**
 * Constructs the global job list.
 * \memberof embb_mtapi_job_struct
 */
mtapi_boolean_t embb_mtapi_job_initialize_list(embb_mtapi_node_t * node);

/**
 * Destroys the global job list.
 * \memberof embb_mtapi_job_struct
 */
void embb_mtapi_job_finalize_list(embb_mtapi_node_t * node);

/**
 * Checks if a given job handle is valid.
 * \memberof embb_mtapi_job_struct
 */
mtapi_boolean_t embb_mtapi_job_is_handle_valid(
  embb_mtapi_node_t * node,
  mtapi_job_hndl_t handle);

/**
 * Gets the pointer associated with the given handle.
 *
 * The handle is expected to be valid, so check it beforehand using
 * embb_mtapi_job_is_handle_valid().
 * \memberof embb_mtapi_job_struct
 */
embb_mtapi_job_t * embb_mtapi_job_get_storage_for_handle(
  embb_mtapi_node_t * node,
  mtapi_job_hndl_t handle);

/**
 * Checks if a given job id is valid.
 * \memberof embb_mtapi_job_struct
 */
mtapi_boolean_t embb_mtapi_job_is_id_valid(
  embb_mtapi_node_t * node,
  mtapi_job_id_t id);

/**
 * Gets the pointer associated with the given id.
 *
 * The id is expected to be valid, so check it beforehand using
 * embb_mtapi_job_is_id_valid().
 *
 * \memberof embb_mtapi_job_struct
 */
embb_mtapi_job_t * embb_mtapi_job_get_storage_for_id(
  embb_mtapi_node_t * node,
  mtapi_job_id_t id);

/**
 * Constructor with configurable maximum action count.
 * \memberof embb_mtapi_job_struct
 */
void embb_mtapi_job_initialize(
  embb_mtapi_job_t * that,
  mtapi_uint_t max_actions);

/**
 * Destructor.
 * \memberof embb_mtapi_job_struct
 */
void embb_mtapi_job_finalize(embb_mtapi_job_t * that);

/**
 * Add an action to the job.
 *
 * Returns MTAPI_TRUE if there are less actions than the configured maximum
 * action count in the job already. Otherwise returns MTAPI_FALSE:
 *
 * \memberof embb_mtapi_job_struct
 */
mtapi_boolean_t embb_mtapi_job_add_action(
  embb_mtapi_job_t * that,
  embb_mtapi_action_t * action);

/**
 * Remove an action from the job.
 * \memberof embb_mtapi_job_struct
 */
void embb_mtapi_job_remove_action(
  embb_mtapi_job_t * that,
  embb_mtapi_action_t * action);

#ifdef __cplusplus
}
#endif

#endif // MTAPI_C_SRC_EMBB_MTAPI_JOB_T_H_
