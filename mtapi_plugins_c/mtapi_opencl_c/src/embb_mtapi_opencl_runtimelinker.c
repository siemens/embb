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

#include <CL/opencl.h>
#include <embb/base/c/internal/config.h>
#include <embb_mtapi_opencl_runtimelinker.h>

//////////////////////////////////////////////////////////////////////////
// function pointer wrappers to hide runtime linking

#define DECLARECLFUNC(rettype, name, params) typedef CL_API_ENTRY rettype\
    (CL_API_CALL *name##Proc)params;\
    name##Proc name##_Dynamic = 0;\
    \
    CL_API_ENTRY rettype CL_API_CALL name params

DECLARECLFUNC(cl_int, clGetPlatformIDs, (cl_uint num_entries,
  cl_platform_id * platforms, cl_uint * num_platforms)) {
  return clGetPlatformIDs_Dynamic(num_entries, platforms, num_platforms);
}

DECLARECLFUNC(cl_int, clGetPlatformInfo, (cl_platform_id platform,
  cl_platform_info param_name, size_t param_value_size, void * param_value,
  size_t * param_value_size_ret)) {
  return clGetPlatformInfo_Dynamic(platform, param_name, param_value_size,
    param_value, param_value_size_ret);
}

DECLARECLFUNC(cl_int, clGetDeviceIDs, (cl_platform_id platform,
  cl_device_type device_type, cl_uint num_entries, cl_device_id * devices,
  cl_uint * num_devices)) {
  return clGetDeviceIDs_Dynamic(platform, device_type, num_entries, devices,
    num_devices);
}

DECLARECLFUNC(cl_int, clGetDeviceInfo, (cl_device_id device,
  cl_device_info param_name, size_t param_value_size, void * param_value,
  size_t * param_value_size_ret)) {
  return clGetDeviceInfo_Dynamic(device, param_name, param_value_size,
    param_value, param_value_size_ret);
}

DECLARECLFUNC(cl_context, clCreateContext,
  (const cl_context_properties * properties, cl_uint num_devices,
  const cl_device_id * devices,
  void (CL_CALLBACK * pfn_notify)(const char *, const void *, size_t, void *),
  void * user_data, cl_int * errcode_ret)) {
  return clCreateContext_Dynamic(properties, num_devices, devices, pfn_notify,
    user_data, errcode_ret);
}

DECLARECLFUNC(cl_command_queue, clCreateCommandQueue, (cl_context context,
  cl_device_id device, cl_command_queue_properties properties,
  cl_int * errcode_ret)) {
  return clCreateCommandQueue_Dynamic(context, device, properties,
    errcode_ret);
}

DECLARECLFUNC(cl_mem, clCreateBuffer, (cl_context context, cl_mem_flags flags,
  size_t size, void * host_ptr, cl_int * errcode_ret)) {
  return clCreateBuffer_Dynamic(context, flags, size, host_ptr, errcode_ret);
}

DECLARECLFUNC(cl_program, clCreateProgramWithSource, (cl_context context,
  cl_uint count, const char ** strings, const size_t * lengths,
  cl_int * errcode_ret)) {
  return clCreateProgramWithSource_Dynamic(context, count, strings, lengths,
    errcode_ret);
}

DECLARECLFUNC(cl_int, clBuildProgram, (cl_program program, cl_uint num_devices,
  const cl_device_id * device_list, const char * options,
  void (CL_CALLBACK * pfn_notify)(cl_program, void *), void * user_data)) {
  return clBuildProgram_Dynamic(program, num_devices, device_list, options,
    pfn_notify, user_data);
}

DECLARECLFUNC(cl_int, clGetProgramBuildInfo, (cl_program program,
  cl_device_id device, cl_program_build_info param_name,
  size_t param_value_size, void * param_value,
  size_t * param_value_size_ret)) {
  return clGetProgramBuildInfo_Dynamic(program, device, param_name,
    param_value_size, param_value, param_value_size_ret);
}

DECLARECLFUNC(cl_kernel, clCreateKernel, (cl_program program,
  const char * kernel_name, cl_int * errcode_ret)) {
  return clCreateKernel_Dynamic(program, kernel_name, errcode_ret);
}

DECLARECLFUNC(cl_int, clSetKernelArg, (cl_kernel kernel, cl_uint arg_index,
  size_t arg_size, const void * arg_value)) {
  return clSetKernelArg_Dynamic(kernel, arg_index, arg_size, arg_value);
}

DECLARECLFUNC(cl_int, clEnqueueWriteBuffer, (cl_command_queue command_queue,
  cl_mem buffer, cl_bool blocking_write, size_t offset, size_t cb,
  const void * ptr, cl_uint num_events_in_wait_list,
  const cl_event * event_wait_list, cl_event * event)) {
  return clEnqueueWriteBuffer_Dynamic(command_queue, buffer, blocking_write,
    offset, cb, ptr, num_events_in_wait_list, event_wait_list, event);
}

DECLARECLFUNC(cl_int, clEnqueueNDRangeKernel, (cl_command_queue command_queue,
  cl_kernel kernel, cl_uint work_dim, const size_t * global_work_offset,
  const size_t * global_work_size, const size_t * local_work_size,
  cl_uint num_events_in_wait_list, const cl_event * event_wait_list,
  cl_event * event)) {
  return clEnqueueNDRangeKernel_Dynamic(command_queue, kernel, work_dim,
    global_work_offset, global_work_size, local_work_size,
    num_events_in_wait_list, event_wait_list, event);
}

DECLARECLFUNC(cl_int, clEnqueueReadBuffer, (cl_command_queue command_queue,
  cl_mem buffer, cl_bool blocking_read, size_t offset, size_t cb, void * ptr,
  cl_uint num_events_in_wait_list, const cl_event * event_wait_list,
  cl_event * event)) {
  return clEnqueueReadBuffer_Dynamic(command_queue, buffer, blocking_read,
    offset, cb, ptr, num_events_in_wait_list, event_wait_list, event);
}

DECLARECLFUNC(cl_int, clSetEventCallback, (cl_event event,
  cl_int command_exec_callback_type,
  void (CL_CALLBACK * pfn_notify)(cl_event, cl_int, void *),
  void * user_data)) {
  return clSetEventCallback_Dynamic(event, command_exec_callback_type,
    pfn_notify, user_data);
}

DECLARECLFUNC(cl_int, clWaitForEvents, (cl_uint num_events,
  const cl_event * event_list)) {
  return clWaitForEvents_Dynamic(num_events, event_list);
}

DECLARECLFUNC(cl_int, clReleaseKernel, (cl_kernel kernel)) {
  return clReleaseKernel_Dynamic(kernel);
}

DECLARECLFUNC(cl_int, clReleaseProgram, (cl_program program)) {
  return clReleaseProgram_Dynamic(program);
}

DECLARECLFUNC(cl_int, clReleaseCommandQueue,
  (cl_command_queue command_queue)) {
  return clReleaseCommandQueue_Dynamic(command_queue);
}

DECLARECLFUNC(cl_int, clReleaseContext, (cl_context context)) {
  return clReleaseContext_Dynamic(context);
}

DECLARECLFUNC(cl_int, clReleaseMemObject, (cl_mem memobj)) {
  return clReleaseMemObject_Dynamic(memobj);
}

DECLARECLFUNC(cl_int, clFlush, (cl_command_queue command_queue)) {
  return clFlush_Dynamic(command_queue);
}

DECLARECLFUNC(cl_int, clFinish, (cl_command_queue command_queue)) {
  return clFinish_Dynamic(command_queue);
}

DECLARECLFUNC(cl_sampler, clCreateSampler, (cl_context context,
  cl_bool normalized_coords, cl_addressing_mode addressing_mode,
  cl_filter_mode filter_mode, cl_int * errcode_ret)) {
  return clCreateSampler_Dynamic(context, normalized_coords, addressing_mode,
    filter_mode, errcode_ret);
}

DECLARECLFUNC(cl_int, clReleaseSampler, (cl_sampler sampler)) {
  return clReleaseSampler_Dynamic(sampler);
}

DECLARECLFUNC(cl_mem, clCreateImage2D, (cl_context context,
  cl_mem_flags flags, const cl_image_format * image_format, size_t image_width,
  size_t image_height, size_t image_row_pitch, void * host_ptr,
  cl_int * errcode_ret)) {
  return clCreateImage2D_Dynamic(context, flags, image_format, image_width,
    image_height, image_row_pitch, host_ptr, errcode_ret);
}

DECLARECLFUNC(cl_mem, clCreateImage3D, (cl_context context, cl_mem_flags flags,
  const cl_image_format * image_format, size_t image_width,
  size_t image_height, size_t image_depth, size_t image_row_pitch,
  size_t image_slice_pitch, void * host_ptr, cl_int * errcode_ret)) {
  return clCreateImage3D_Dynamic(context, flags, image_format, image_width,
    image_height, image_depth, image_row_pitch, image_slice_pitch, host_ptr,
    errcode_ret);
}

DECLARECLFUNC(cl_int, clEnqueueAcquireGLObjects,
  (cl_command_queue command_queue, cl_uint num_objects,
  const cl_mem * mem_objects, cl_uint num_events_in_wait_list,
  const cl_event * event_wait_list, cl_event * event)) {
  return clEnqueueAcquireGLObjects_Dynamic(command_queue, num_objects,
    mem_objects, num_events_in_wait_list, event_wait_list, event);
}

DECLARECLFUNC(cl_int, clEnqueueReleaseGLObjects,
  (cl_command_queue command_queue, cl_uint num_objects,
  const cl_mem * mem_objects, cl_uint num_events_in_wait_list,
  const cl_event * event_wait_list, cl_event * event)) {
  return clEnqueueReleaseGLObjects_Dynamic(command_queue, num_objects,
    mem_objects, num_events_in_wait_list, event_wait_list, event);
}

DECLARECLFUNC(cl_mem, clCreateFromGLBuffer, (cl_context context,
  cl_mem_flags flags, cl_GLuint bufobj, int * errcode_ret)) {
  return clCreateFromGLBuffer_Dynamic(context, flags, bufobj, errcode_ret);
}

//////////////////////////////////////////////////////////////////////////
// system specific functions

#ifdef _WIN32

#include <Windows.h>

#define CHECKEDIMPORT(name) name##_Dynamic = \
  (name##Proc)GetProcAddress(opencl_dll_handle, #name); \
  if (name##_Dynamic == 0) return 0;

#else

#include <dlfcn.h>

#define CHECKEDIMPORT(name) name##_Dynamic = \
  (name##Proc)dlsym(opencl_dll_handle, #name); \
  if (name##_Dynamic == 0) return 0;

#endif

#ifdef __cplusplus
extern "C"
#endif
int embb_mtapi_opencl_link_at_runtime() {
#ifdef _WIN32
  HMODULE opencl_dll_handle = LoadLibraryA("opencl.dll");
#else
  void * opencl_dll_handle = dlopen("libOpenCL.so", RTLD_LAZY);
#endif
  if (opencl_dll_handle == 0)
    return -1;

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4191)
#endif
  CHECKEDIMPORT(clGetPlatformIDs);
  CHECKEDIMPORT(clGetPlatformInfo);
  CHECKEDIMPORT(clGetDeviceIDs);
  CHECKEDIMPORT(clGetDeviceInfo);
  CHECKEDIMPORT(clCreateContext);
  CHECKEDIMPORT(clCreateCommandQueue);
  CHECKEDIMPORT(clCreateBuffer);
  CHECKEDIMPORT(clCreateProgramWithSource);
  CHECKEDIMPORT(clBuildProgram);
  CHECKEDIMPORT(clGetProgramBuildInfo);
  CHECKEDIMPORT(clCreateKernel);
  CHECKEDIMPORT(clSetKernelArg);
  CHECKEDIMPORT(clEnqueueWriteBuffer);
  CHECKEDIMPORT(clEnqueueNDRangeKernel);
  CHECKEDIMPORT(clEnqueueReadBuffer);
  CHECKEDIMPORT(clSetEventCallback);
  CHECKEDIMPORT(clWaitForEvents);
  CHECKEDIMPORT(clReleaseKernel);
  CHECKEDIMPORT(clReleaseProgram);
  CHECKEDIMPORT(clReleaseCommandQueue);
  CHECKEDIMPORT(clReleaseContext);
  CHECKEDIMPORT(clReleaseMemObject);
  CHECKEDIMPORT(clFlush);
  CHECKEDIMPORT(clFinish);
  CHECKEDIMPORT(clCreateSampler);
  CHECKEDIMPORT(clReleaseSampler);
  CHECKEDIMPORT(clCreateImage2D);
  CHECKEDIMPORT(clCreateImage3D);

  CHECKEDIMPORT(clEnqueueAcquireGLObjects);
  CHECKEDIMPORT(clEnqueueReleaseGLObjects);
  CHECKEDIMPORT(clCreateFromGLBuffer);
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif

  return 1;
}
