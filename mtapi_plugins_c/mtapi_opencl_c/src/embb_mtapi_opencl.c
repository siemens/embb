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

#include <CL/cl.h>
#include <string.h>
#include <assert.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/mtapi/c/mtapi_ext.h>
#include <embb/base/c/internal/unused.h>

#include <embb/mtapi/c/mtapi_opencl.h>

#include <embb_mtapi_opencl_runtimelinker.h>

#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>
#include <mtapi_status_t.h>

struct embb_mtapi_opencl_plugin_struct {
  cl_platform_id platform_id;
  cl_device_id device_id;
  cl_context context;
  cl_command_queue command_queue;
  size_t work_group_size;
  size_t work_item_sizes[3];
};

typedef struct embb_mtapi_opencl_plugin_struct embb_mtapi_opencl_plugin_t;

static embb_mtapi_opencl_plugin_t embb_mtapi_opencl_plugin;

struct embb_mtapi_opencl_action_struct {
  cl_program program;
  cl_kernel kernel;
  cl_mem node_local_data;
  int node_local_data_size;
  size_t local_work_size;
  size_t element_size;
};

typedef struct embb_mtapi_opencl_action_struct embb_mtapi_opencl_action_t;

struct embb_mtapi_opencl_task_struct {
  cl_mem arguments;
  int arguments_size;
  cl_mem result_buffer;
  int result_buffer_size;
  cl_event kernel_finish_event;
  mtapi_task_hndl_t task;
};

typedef struct embb_mtapi_opencl_task_struct embb_mtapi_opencl_task_t;

static size_t round_up(size_t group_size, size_t global_size) {
  size_t r = global_size % group_size;
  if (r == 0) {
    return global_size;
  } else {
    return global_size + group_size - r;
  }
}

static void CL_API_CALL opencl_task_complete(
  cl_event ev, cl_int status, void * data) {
  EMBB_UNUSED(ev);
  EMBB_UNUSED(status);

  cl_int err;
  EMBB_UNUSED_IN_RELEASE(err);
  embb_mtapi_opencl_task_t * opencl_task = (embb_mtapi_opencl_task_t*)data;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(
      node->task_pool, opencl_task->task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(
          node->task_pool, opencl_task->task);

      err = clWaitForEvents(1, &opencl_task->kernel_finish_event);
      assert(CL_SUCCESS == err);

      if (NULL != opencl_task->result_buffer) {
        err = clReleaseMemObject(opencl_task->result_buffer);
        assert(CL_SUCCESS == err);
      }
      if (NULL != opencl_task->arguments) {
        err = clReleaseMemObject(opencl_task->arguments);
        assert(CL_SUCCESS == err);
      }

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

static void opencl_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  cl_int err;

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

        embb_mtapi_opencl_plugin_t * plugin = &embb_mtapi_opencl_plugin;
        embb_mtapi_opencl_action_t * opencl_action =
          (embb_mtapi_opencl_action_t*)local_action->plugin_data;
        embb_mtapi_opencl_task_t * opencl_task =
          (embb_mtapi_opencl_task_t*)embb_alloc(
            sizeof(embb_mtapi_opencl_task_t));

        size_t elements = local_task->result_size /
          opencl_action->element_size;
        size_t global_work_size;

        if (0 == elements)
          elements = 1;
        global_work_size =
          round_up(opencl_action->local_work_size, elements);

        opencl_task->task = task;

        opencl_task->arguments_size = (int)local_task->arguments_size;
        if (0 < local_task->arguments_size) {
          opencl_task->arguments = clCreateBuffer(plugin->context,
            CL_MEM_READ_ONLY, local_task->arguments_size, NULL, &err);
        } else {
          opencl_task->arguments = NULL;
        }
        opencl_task->result_buffer_size = (int)local_task->result_size;
        if (0 < local_task->result_size) {
          opencl_task->result_buffer = clCreateBuffer(plugin->context,
            CL_MEM_WRITE_ONLY, local_task->result_size, NULL, &err);
        } else {
          opencl_task->result_buffer = NULL;
        }

        err = clSetKernelArg(opencl_action->kernel, 0, sizeof(cl_mem),
          (const void*)&opencl_task->arguments);
        err |= clSetKernelArg(opencl_action->kernel, 1, sizeof(cl_int),
          (const void*)&opencl_task->arguments_size);

        err |= clSetKernelArg(opencl_action->kernel, 2, sizeof(cl_mem),
          (const void*)&opencl_task->result_buffer);
        err |= clSetKernelArg(opencl_action->kernel, 3, sizeof(cl_int),
          (const void*)&opencl_task->result_buffer_size);

        err |= clEnqueueWriteBuffer(plugin->command_queue,
          opencl_task->arguments, CL_FALSE, 0,
          (size_t)opencl_task->arguments_size, local_task->arguments,
          0, NULL, NULL);

        if (CL_SUCCESS == err) {
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_RUNNING);

          err |= clEnqueueNDRangeKernel(plugin->command_queue,
            opencl_action->kernel, 1, NULL,
            &global_work_size, &opencl_action->local_work_size, 0, NULL, NULL);
          err |= clEnqueueReadBuffer(plugin->command_queue,
            opencl_task->result_buffer, CL_FALSE, 0,
            (size_t)opencl_task->result_buffer_size, local_task->result_buffer,
            0, NULL, &opencl_task->kernel_finish_event);
          err |= clSetEventCallback(opencl_task->kernel_finish_event,
            CL_COMPLETE, opencl_task_complete, opencl_task);
        }

        err |= clFlush(plugin->command_queue);
        if (CL_SUCCESS != err) {
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

static void opencl_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(task);

  mtapi_status_set(status, local_status);
}

static void opencl_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  cl_int err;
  EMBB_UNUSED_IN_RELEASE(err);

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t * local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
        node->action_pool, action);
      embb_mtapi_opencl_action_t * opencl_action =
        (embb_mtapi_opencl_action_t *)local_action->plugin_data;
      if (NULL != opencl_action->node_local_data) {
        err = clReleaseMemObject(opencl_action->node_local_data);
        assert(CL_SUCCESS == err);
      }

      err = clReleaseKernel(opencl_action->kernel);
      assert(CL_SUCCESS == err);
      err = clReleaseProgram(opencl_action->program);
      assert(CL_SUCCESS == err);

      embb_free(opencl_action);
      local_status = MTAPI_SUCCESS;
    }
  }

  mtapi_status_set(status, local_status);
}

char buffer[1024];

void mtapi_opencl_plugin_initialize(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  cl_int err;
  embb_mtapi_opencl_plugin_t * plugin = &embb_mtapi_opencl_plugin;

  err = embb_mtapi_opencl_link_at_runtime();
  if (err <= 0) {
    // OpenCL not available, or wrong version
    local_status = MTAPI_ERR_FUNC_NOT_IMPLEMENTED;
  } else {
    // all good, go ahead
    err = clGetPlatformIDs(1, &plugin->platform_id, NULL);
    if (CL_SUCCESS == err) {
      err = clGetDeviceIDs(plugin->platform_id, CL_DEVICE_TYPE_DEFAULT,
        1, &plugin->device_id, NULL);
    }
    if (CL_SUCCESS == err) {
      plugin->context = clCreateContext(NULL, 1, &plugin->device_id,
        NULL, NULL, &err);
    }
    if (CL_SUCCESS == err) {
      err = clGetDeviceInfo(plugin->device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE,
        sizeof(size_t), &plugin->work_group_size, NULL);
    }
    if (CL_SUCCESS == err) {
      err = clGetDeviceInfo(plugin->device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES,
        3 * sizeof(size_t), &plugin->work_item_sizes[0], NULL);
    }
    if (CL_SUCCESS == err) {
      plugin->command_queue = clCreateCommandQueue(plugin->context,
        plugin->device_id, 0, &err);
    }
    if (CL_SUCCESS == err) {
      local_status = MTAPI_SUCCESS;
    }
  }

  mtapi_status_set(status, local_status);
}

void mtapi_opencl_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  cl_int err;
  EMBB_UNUSED_IN_RELEASE(err);
  embb_mtapi_opencl_plugin_t * plugin = &embb_mtapi_opencl_plugin;

  /* finalization */
  err = clReleaseCommandQueue(plugin->command_queue);
  assert(CL_SUCCESS == err);
  err = clReleaseContext(plugin->context);
  assert(CL_SUCCESS == err);

  local_status = MTAPI_SUCCESS;
  mtapi_status_set(status, local_status);
}

mtapi_action_hndl_t mtapi_opencl_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN char* kernel_source,
  MTAPI_IN char* kernel_name,
  MTAPI_IN mtapi_size_t local_work_size,
  MTAPI_IN mtapi_size_t element_size,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  cl_int err;
  embb_mtapi_opencl_plugin_t * plugin = &embb_mtapi_opencl_plugin;
  embb_mtapi_opencl_action_t * action =
    (embb_mtapi_opencl_action_t*)embb_alloc(
      sizeof(embb_mtapi_opencl_action_t));
  mtapi_action_hndl_t action_hndl = { 0, 0 }; // invalid handle
  size_t kernel_length = strlen(kernel_source);
  mtapi_boolean_t free_program_on_error = MTAPI_FALSE;
  mtapi_boolean_t free_kernel_on_error = MTAPI_FALSE;
  mtapi_boolean_t free_node_local_data_on_error = MTAPI_FALSE;

  action->local_work_size = local_work_size;
  action->element_size = element_size;

  /* initialization */
  action->program = clCreateProgramWithSource(plugin->context,
    1, &kernel_source, &kernel_length, &err);
  if (CL_SUCCESS == err) {
    free_program_on_error = MTAPI_TRUE;
    err = clBuildProgram(action->program, 1, &plugin->device_id,
      NULL, NULL, NULL);
  } else {
    err = clGetProgramBuildInfo(action->program, plugin->device_id,
      CL_PROGRAM_BUILD_LOG, 1024, buffer, NULL);
  }

  if (CL_SUCCESS == err) {
    action->kernel = clCreateKernel(action->program, kernel_name, &err);
    if (CL_SUCCESS == err) {
      free_kernel_on_error = MTAPI_TRUE;
    }
  }

  if (0 < node_local_data_size) {
    action->node_local_data = clCreateBuffer(plugin->context, CL_MEM_READ_ONLY,
      node_local_data_size, NULL, &err);
    if (CL_SUCCESS == err) {
      free_node_local_data_on_error = MTAPI_TRUE;
    }
    action->node_local_data_size = (int)node_local_data_size;
    if (CL_SUCCESS == err) {
      err = clEnqueueWriteBuffer(plugin->command_queue,
        action->node_local_data, CL_TRUE, 0,
        (size_t)action->node_local_data_size, node_local_data, 0, NULL, NULL);
    }
  } else {
    action->node_local_data = NULL;
    action->node_local_data_size = 0;
  }

  if (CL_SUCCESS == err) {
    err = clSetKernelArg(action->kernel, 4, sizeof(cl_mem),
      (const void*)&action->node_local_data);
  }
  if (CL_SUCCESS == err) {
    err = clSetKernelArg(action->kernel, 5, sizeof(cl_int),
      (const void*)&action->node_local_data_size);
  }

  if (CL_SUCCESS == err) {
    action_hndl = mtapi_ext_plugin_action_create(
      job_id,
      opencl_task_start,
      opencl_task_cancel,
      opencl_action_finalize,
      action,
      node_local_data,
      node_local_data_size,
      MTAPI_NULL,
      &local_status);
  } else {
    if (free_node_local_data_on_error) {
      clReleaseMemObject(action->node_local_data);
    }
    if (free_kernel_on_error) {
      clReleaseKernel(action->kernel);
    }
    if (free_program_on_error) {
      clReleaseProgram(action->program);
    }
    embb_free(action);
  }

  mtapi_status_set(status, local_status);

  return action_hndl;
}

cl_context mtapi_opencl_get_context(
  MTAPI_OUT mtapi_status_t* status     /**< [out] Pointer to error code,
                                       may be \c MTAPI_NULL */
) {
  embb_mtapi_opencl_plugin_t * plugin = &embb_mtapi_opencl_plugin;
  mtapi_status_set(status, MTAPI_SUCCESS);
  return plugin->context;
}
