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

#ifndef EMBB_MTAPI_C_MTAPI_ZYNQ_H_
#define EMBB_MTAPI_C_MTAPI_ZYNQ_H_


#include <embb/mtapi/c/mtapi_ext.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup C_MTAPI_ZYNQ MTAPI ZYNQ Plugin
 *
 * \ingroup C_MTAPI_EXT
 *
 * Provides functionality to execute tasks on Zynq devices.
 */


/**
 * Initializes the MTAPI Zynq environment on a previously initialized MTAPI
 * node.
 *
 * It must be called on all nodes using the MTAPI Zynq plugin.
 *
 * Application software using MTAPI Zynq must call
 * mtapi_zynq_plugin_initialize() once per node. It is an error to call
 * mtapi_zynq_plugin_initialize() multiple times
 * from a given node, unless mtapi_zynq_plugin_finalize() is called in
 * between.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                  | Description
 * --------------------------- | ----------------------------------------------
 * \c MTAPI_ERR_UNKNOWN        | MTAPI Zynq couldn't be initialized.
 *
 * \see mtapi_zynq_plugin_finalize()
 *
 * \notthreadsafe
 * \ingroup C_MTAPI_ZYNQ
 */
void mtapi_zynq_plugin_initialize(
  MTAPI_IN char* uio_name,             /**< [out] UIO device tree name */
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
);

/**
 * Finalizes the MTAPI Zynq environment on the local MTAPI node.
 *
 * It has to be called by each node using MTAPI Zynq. It is an error to call
 * mtapi_zynq_plugin_finalize() without first calling
 * mtapi_zynq_plugin_initialize(). An MTAPI node can call
 * mtapi_zynq_plugin_finalize() once for each call to
 * mtapi_zynq_plugin_initialize(), but it is an error to call
 * mtapi_zynq_plugin_finalize() multiple times from a given node
 * unless mtapi_zynq_plugin_initialize() has been called prior to each
 * mtapi_zynq_plugin_finalize() call.
 *
 * All Zynq tasks that have not completed and that have been started on the
 * node where mtapi_zynq_plugin_finalize() is called will be canceled
 * (see mtapi_task_cancel()). mtapi_zynq_plugin_finalize() blocks until all
 * tasks that have been started on the same node return. Tasks that execute
 * actions on the node where mtapi_zynq_plugin_finalize() is called, also
 * block finalization of the MTAPI Zynq system on that node.
 *
 * On success, \c *status is set to \c MTAPI_SUCCESS. On error, \c *status is
 * set to the appropriate error defined below.
 * Error code                    | Description
 * ----------------------------- | --------------------------------------------
 * \c MTAPI_ERR_UNKNOWN          | MTAPI Zynq couldn't be finalized.
 *
 * \see mtapi_zynq_plugin_initialize(), mtapi_task_cancel()
 *
 * \notthreadsafe
 * \ingroup C_MTAPI_ZYNQ
 */
void mtapi_zynq_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
);

/**
 * This function creates a ZYNQ action.
 *
 * It is called on the node where the user wants to execute an action on a
 * Zynq device. A Zynq action contains a reference to a local job and the physical
 * address of an accelerator in the programmable logic.
 * After a Zynq action is created, it is referenced by the application using
 * a node-local handle of type \c mtapi_action_hndl_t, or indirectly through a
 * node-local job handle of type \c mtapi_job_hndl_t. A Zynq action's
 * life-cycle begins with mtapi_zynq_action_create(), and ends when
 * mtapi_action_delete() or mtapi_finalize() is called.
 *
 * To create an action, the application must supply the domain-wide job ID of
 * the job associated with the action. Job IDs must be predefined in the
 * application and runtime, of type \c mtapi_job_id_t, which is an
 * implementation-defined type. The job ID is unique in the sense that it is
 * unique for the job implemented by the action. However several actions may
 * implement the same job for load balancing purposes.
 *
 * If \c node_local_data_size is not zero, \c node_local_data specifies the
 * start of node local data shared by kernel functions executed on the same
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
 *     <td>\c MTAPI_ERR_NODE_NOTINIT</td>
 *     <td>The calling node is not initialized.</td>
 *   </tr>
 *   <tr>
 *     <td>\c MTAPI_ERR_UNKNOWN</td>
 *     <td>The kernel could not be compiled or no OpenCL device was
 *         available.</td>
 *   </tr>
 * </table>
 *
 * \see mtapi_action_delete(), mtapi_finalize()
 *
 * \returns Handle to newly created Zynq action, invalid handle on error
 * \threadsafe
 * \ingroup C_MTAPI_ZYNQ
 */
mtapi_action_hndl_t mtapi_zynq_action_create(
  MTAPI_IN mtapi_job_id_t job_id,      /**< [in] Job id */
  MTAPI_IN char* uio_name,             /**< [in] Name of the UIO node in the device tree, may be \c MTAPI_NULL to use /dev/mem for memory mapping*/
  MTAPI_IN uint32_t phys_addr,         /**< [in] Physical address of the accelerator*/
  MTAPI_IN void* node_local_data,      /**< [in] Data shared across tasks */
  MTAPI_IN mtapi_size_t node_local_data_size,
                                       /**< [in] Size of shared data */
  MTAPI_IN uint8_t cacheable,          /**< [in] Defines whether buffers are cached or not*/                                    
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                            may be \c MTAPI_NULL */
);

#ifdef __cplusplus
}
#endif


#endif // EMBB_MTAPI_C_MTAPI_ZYNQ_H_
