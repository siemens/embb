/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#include <cuda.h>
#include <string.h>
#include <assert.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/mtapi/c/mtapi_ext.h>
#include <embb/base/c/internal/unused.h>

#include <embb/mtapi/c/mtapi_cuda.h>

#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>
#include <mtapi_status_t.h>

struct embb_mtapi_cuda_plugin_struct {
  CUdevice device;
  CUcontext context;
  CUstream stream;
  int work_group_size;
};

typedef struct embb_mtapi_cuda_plugin_struct embb_mtapi_cuda_plugin_t;

static embb_mtapi_cuda_plugin_t embb_mtapi_cuda_plugin;

struct embb_mtapi_cuda_action_struct {
  CUmodule module;
  CUfunction function;
  CUdeviceptr node_local_data;
  int node_local_data_size;
  size_t local_work_size;
  size_t element_size;
};

typedef struct embb_mtapi_cuda_action_struct embb_mtapi_cuda_action_t;

struct embb_mtapi_cuda_task_struct {
  CUdeviceptr arguments;
  int arguments_size;
  CUdeviceptr result_buffer;
  int result_buffer_size;
  mtapi_task_hndl_t task;
};

typedef struct embb_mtapi_cuda_task_struct embb_mtapi_cuda_task_t;

static size_t round_up(size_t group_size, size_t global_size) {
  size_t r = global_size % group_size;
  if (r == 0) {
    return global_size;
  } else {
    return global_size + group_size - r;
  }
}

static void CUDA_CB cuda_task_complete(
  CUstream stream, CUresult status, void *data) {
  EMBB_UNUSED(stream);
  EMBB_UNUSED(status);
  CUresult err;
  EMBB_UNUSED_IN_RELEASE(err);
  embb_mtapi_cuda_task_t * cuda_task = (embb_mtapi_cuda_task_t*)data;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(
      node->task_pool, cuda_task->task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(
          node->task_pool, cuda_task->task);

      if (0 != cuda_task->result_buffer) {
        err = cuMemFree_v2(cuda_task->result_buffer);
        assert(CUDA_SUCCESS == err);
      }
      if (0 != cuda_task->arguments) {
        err = cuMemFree_v2(cuda_task->arguments);
        assert(CUDA_SUCCESS == err);
      }

      embb_free(cuda_task);

      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        embb_mtapi_action_t * local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
            node->action_pool, local_task->action);

        embb_atomic_fetch_and_add_int(&local_action->num_tasks,
          -(int)local_task->attributes.num_instances);
      }

      embb_mtapi_task_set_state(local_task, MTAPI_TASK_COMPLETED);
    }
  }
}

static void cuda_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  CUresult err = CUDA_SUCCESS;
  embb_mtapi_cuda_plugin_t * plugin = &embb_mtapi_cuda_plugin;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        embb_mtapi_action_t * local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, local_task->action);

        embb_mtapi_cuda_action_t * cuda_action =
          (embb_mtapi_cuda_action_t*)local_action->plugin_data;
        embb_mtapi_cuda_task_t * cuda_task =
          (embb_mtapi_cuda_task_t*)embb_alloc(
            sizeof(embb_mtapi_cuda_task_t));

        size_t elements = local_task->result_size /
          cuda_action->element_size;
        size_t global_work_size;

        if (0 == elements)
          elements = 1;
        global_work_size =
          round_up(cuda_action->local_work_size, elements);

        cuda_task->task = task;

        cuda_task->arguments_size = (int)local_task->arguments_size;
        if (0 < local_task->arguments_size) {
          err = cuMemAlloc_v2(&cuda_task->arguments,
            local_task->arguments_size);
        } else {
          cuda_task->arguments = 0;
        }
        cuda_task->result_buffer_size = (int)local_task->result_size;
        if (0 < local_task->result_size) {
          err = cuMemAlloc_v2(&cuda_task->result_buffer,
            local_task->result_size);
        } else {
          cuda_task->result_buffer = 0;
        }

        if (0 != cuda_task->arguments) {
          err = cuMemcpyHtoDAsync_v2(cuda_task->arguments,
            local_task->arguments, (size_t)cuda_task->arguments_size,
            plugin->stream);
        }

        if (CUDA_SUCCESS == err) {
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_RUNNING);
          void * args[6];
          args[0] = &cuda_task->arguments;
          args[1] = &cuda_task->arguments_size;
          args[2] = &cuda_task->result_buffer;
          args[3] = &cuda_task->result_buffer_size;
          args[4] = &cuda_action->node_local_data;
          args[5] = &cuda_action->node_local_data_size;

          err = cuLaunchKernel(cuda_action->function,
            global_work_size, 1, 1,
            cuda_action->local_work_size, 1, 1,
            1024, plugin->stream, args, NULL);

          if (CUDA_SUCCESS == err) {
            if (0 != cuda_task->result_buffer) {
              err = cuMemcpyDtoHAsync_v2(local_task->result_buffer,
                cuda_task->result_buffer, cuda_task->result_buffer_size,
                plugin->stream);
            }

            err = cuStreamAddCallback(plugin->stream, cuda_task_complete,
              cuda_task, 0);
          }
        }

        if (CUDA_SUCCESS != err) {
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_ERROR);
          local_status = MTAPI_ERR_ACTION_FAILED;
        } else {
          local_status = MTAPI_SUCCESS;
        }
      }
    }
  }

  mtapi_status_set(status, local_status);
}

static void cuda_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(task);

  mtapi_status_set(status, local_status);
}

static void cuda_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  CUresult err;
  EMBB_UNUSED_IN_RELEASE(err);

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t * local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
        node->action_pool, action);
      embb_mtapi_cuda_action_t * cuda_action =
        (embb_mtapi_cuda_action_t *)local_action->plugin_data;
      if (0 != cuda_action->node_local_data) {
        err = cuMemFree_v2(cuda_action->node_local_data);
        assert(CUDA_SUCCESS == err);
      }

      err = cuModuleUnload(cuda_action->module);
      assert(CUDA_SUCCESS == err);

      embb_free(cuda_action);
      local_status = MTAPI_SUCCESS;
    }
  }

  mtapi_status_set(status, local_status);
}

char buffer[1024];

void mtapi_cuda_plugin_initialize(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  CUresult err;
  embb_mtapi_cuda_plugin_t * plugin = &embb_mtapi_cuda_plugin;

  mtapi_status_set(status, MTAPI_ERR_UNKNOWN);

  err = cuInit(0);
  if (CUDA_SUCCESS != err) return;
  err =  cuDeviceGet(&plugin->device, 0);
  if (CUDA_SUCCESS != err) return;
  err = cuCtxCreate_v2(&plugin->context, 0, plugin->device);
  if (CUDA_SUCCESS != err) return;
  cuDeviceGetAttribute(&plugin->work_group_size,
    CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X, plugin->device);
  err = cuStreamCreate(&plugin->stream, CU_STREAM_NON_BLOCKING);
  if (CUDA_SUCCESS != err) return;
  local_status = MTAPI_SUCCESS;

  mtapi_status_set(status, local_status);
}

void mtapi_cuda_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  CUresult err;
  EMBB_UNUSED_IN_RELEASE(err);
  embb_mtapi_cuda_plugin_t * plugin = &embb_mtapi_cuda_plugin;

  /* finalization */
  err = cuStreamDestroy_v2(plugin->stream);
  assert(CUDA_SUCCESS == err);
  err = cuCtxDestroy_v2(plugin->context);
  assert(CUDA_SUCCESS == err);

  local_status = MTAPI_SUCCESS;
  mtapi_status_set(status, local_status);
}

mtapi_action_hndl_t mtapi_cuda_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN char* kernel_source,
  MTAPI_IN char* kernel_name,
  MTAPI_IN mtapi_size_t local_work_size,
  MTAPI_IN mtapi_size_t element_size,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  mtapi_status_set(status, MTAPI_ERR_UNKNOWN);

  CUresult err;
  embb_mtapi_cuda_action_t * action =
    (embb_mtapi_cuda_action_t*)embb_alloc(
      sizeof(embb_mtapi_cuda_action_t));
  mtapi_action_hndl_t action_hndl = { 0, 0 }; // invalid handle
  mtapi_boolean_t free_module_on_error = MTAPI_FALSE;
  mtapi_boolean_t free_node_local_data_on_error = MTAPI_FALSE;

  action->local_work_size = local_work_size;
  action->element_size = element_size;

  /* initialization */
  err = cuModuleLoadData(&action->module, kernel_source);
  if (CUDA_SUCCESS == err) {
    free_module_on_error = MTAPI_TRUE;
    err = cuModuleGetFunction(&action->function, action->module, kernel_name);
  }
  if (CUDA_SUCCESS == err) {
    if (0 < node_local_data_size) {
      err = cuMemAlloc_v2(&action->node_local_data, node_local_data_size);
      if (CUDA_SUCCESS == err) {
        free_node_local_data_on_error = MTAPI_TRUE;
      }
      action->node_local_data_size = (int)node_local_data_size;
      if (CUDA_SUCCESS == err) {
        err = cuMemcpyHtoD_v2(
          action->node_local_data, node_local_data, node_local_data_size);
      }
    } else {
      action->node_local_data = 0;
      action->node_local_data_size = 0;
    }
  }

  if (CUDA_SUCCESS == err) {
    action_hndl = mtapi_ext_plugin_action_create(
      job_id,
      cuda_task_start,
      cuda_task_cancel,
      cuda_action_finalize,
      action,
      node_local_data,
      node_local_data_size,
      MTAPI_NULL,
      &local_status);
  } else {
    if (free_node_local_data_on_error) {
      cuMemFree_v2(action->node_local_data);
    }
    if (free_module_on_error) {
      cuModuleUnload(action->module);
    }
    embb_free(action);
  }

  mtapi_status_set(status, local_status);

  return action_hndl;
}

CUcontext mtapi_cuda_get_context(
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                       may be \c MTAPI_NULL */
) {
  embb_mtapi_cuda_plugin_t * plugin = &embb_mtapi_cuda_plugin;
  mtapi_status_set(status, MTAPI_SUCCESS);
  return plugin->context;
}
