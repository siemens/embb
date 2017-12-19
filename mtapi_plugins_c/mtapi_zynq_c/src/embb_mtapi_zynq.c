/*
 * Copyright (c) 2017, Siemens AG. All rights reserved.
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

#include <string.h>

#include <embb/base/c/thread.h>
#include <embb/base/c/memory_allocation.h>

#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>
#include <mtapi_status_t.h>
#include <embb/base/c/internal/unused.h>
#include <assert.h>

#include <embb_mtapi_zynq_task_queue.h>
#include <embb_mtapi_zynq_driver.h>
#include <embb_mtapi_zynq.h>

#include <stdbool.h>

#include <stdio.h>

/* Inline Functions */
#define write_32bit_reg(base_address, reg_offset, data) \
    *(volatile uint32_t*)((base_address) + (reg_offset >> 2)) = (uint32_t)(data)
#define read_32bit_reg(base_address, reg_offset) \
    *(volatile uint32_t*)((base_address) + (reg_offset >> 2))

/* Macros */ 
#define MAX_UIO_PATH_SIZE 64
#define MAX_UIO_DEVICES 64
#define MAP_SIZE 0x1000

/* Accelerator Control Interface Offsets */
#define CONTROL_ADDR_AP_CTRL                0x00
#define CONTROL_ADDR_GIE                    0x04
#define CONTROL_ADDR_IER                    0x08
#define CONTROL_ADDR_ISR                    0x0c
#define CONTROL_ADDR_CONFIG                 0x10
#define CONTROL_BITS_CONFIG                 32
#define CONTROL_ADDR_ARGUMENTS              0x18
#define CONTROL_BITS_ARGUMENTS              32
#define CONTROL_ADDR_ARGUMENTS_SIZE         0x20
#define CONTROL_BITS_ARGUMENTS_SIZE         32
#define CONTROL_ADDR_RESULT_BUFFER          0x28
#define CONTROL_BITS_RESULT_BUFFER          32
#define CONTROL_ADDR_RESULT_BUFFER_SIZE     0x30
#define CONTROL_BITS_RESULT_BUFFER_SIZE     32
#define CONTROL_ADDR_NODE_LOCAL_DATA        0x38
#define CONTROL_BITS_NODE_LOCAL_DATA        32
#define CONTROL_ADDR_NODE_LOCAL_DATA_SIZE   0x40
#define CONTROL_BITS_NODE_LOCAL_DATA_SIZE   32

static uio_info_t uio_base;
    
struct embb_mtapi_zynq_action_struct {
  acc_t accelerator;
  embb_thread_t thread;
  embb_atomic_int run;
  embb_mtapi_zynq_task_queue_t zynq_task_queue;
  // file which is used to wait for interrupt
  const void* node_local_data;
  mtapi_size_t node_local_data_size;
  cma_buffer_t cma_node_local_data;
  mtapi_boolean_t cacheable;
};
typedef struct embb_mtapi_zynq_action_struct embb_mtapi_zynq_action_t;

static void zynq_load_node_local_data(embb_mtapi_zynq_action_t* action);
static void zynq_task_complete(embb_mtapi_zynq_task_t* zynq_task);
static void zynq_task_start(acc_t* accelerator, embb_mtapi_zynq_task_t* zynq_task);

static int embb_mtapi_zynq_thread(void* arguments){
  embb_mtapi_zynq_action_t* action = (embb_mtapi_zynq_action_t*) arguments;
  embb_mtapi_zynq_task_t* zynq_task;
  
  if(action->node_local_data_size > 0){
    zynq_load_node_local_data(action);
  }
  
  while(embb_atomic_load_int(&action->run) == 1){

    zynq_task = embb_mtapi_zynq_task_queue_pop_front(&action->zynq_task_queue);
    if(zynq_task != NULL){


      uio_enable_interrupt(&action->accelerator.uio_fd);
      zynq_task_start(&action->accelerator, zynq_task);
      uio_wait_interrupt(&action->accelerator.uio_fd);
      
      if(acc_interrupt_read(&action->accelerator) == 0x00){
        do{
          uio_enable_interrupt(&action->accelerator.uio_fd);
          uio_wait_interrupt(&action->accelerator.uio_fd);
        }while(acc_interrupt_read(&action->accelerator) == 0x00);
      }

      acc_interrupt_clear(&action->accelerator);
      zynq_task_complete(zynq_task);

    }
    else{
      embb_thread_yield();
    }
  }
  return 0;
}

static void zynq_load_node_local_data(embb_mtapi_zynq_action_t* action){
  
  assert(action->cma_node_local_data.virt_addr != NULL);
  assert(action->node_local_data != NULL);
  memcpy(action->cma_node_local_data.virt_addr, action->node_local_data, action->node_local_data_size);
  
  acc_write_node_local_data_addr(&action->accelerator, &action->cma_node_local_data);
  acc_write_node_local_data_size(&action->accelerator, action->node_local_data_size);
  
  uint32_t* ptr = action->cma_node_local_data.virt_addr;
  printf("node_local_data: %d\n", ptr[0]); 
  
  uint32_t reg;
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_AP_CTRL);
  printf("CONTROL_ADDR_AP_CTRL: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_GIE);
  printf("CONTROL_ADDR_GIE: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_IER);
  printf("CONTROL_ADDR_IER: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_ISR);
  printf("CONTROL_ADDR_ISR: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_CONFIG);
  printf("CONTROL_ADDR_CONFIG: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_ARGUMENTS);
  printf("CONTROL_ADDR_ARGUMENTS: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_ARGUMENTS_SIZE);
  printf("CONTROL_ADDR_ARGUMENTS_SIZE: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_RESULT_BUFFER);
  printf("CONTROL_ADDR_RESULT_BUFFER: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_RESULT_BUFFER_SIZE);
  printf("CONTROL_ADDR_RESULT_BUFFER_SIZE: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_NODE_LOCAL_DATA);
  printf("CONTROL_ADDR_NODE_LOCAL_DATA: %x\n", reg);
  reg = read_32bit_reg(action->accelerator.control_base_address, CONTROL_ADDR_NODE_LOCAL_DATA_SIZE);
  printf("CONTROL_ADDR_NODE_LOCAL_DATA_SIZE: %x\n", reg);
  
  // change mode to load node local data
  acc_write_config(&action->accelerator, 0x01);
  
  uio_enable_interrupt(&action->accelerator.uio_fd);
  printf("Start accelerator\n"); 
  acc_start(&action->accelerator);
  uio_wait_interrupt(&action->accelerator.uio_fd);
  if(acc_interrupt_read(&action->accelerator) == 0x00){
        do{
          uio_enable_interrupt(&action->accelerator.uio_fd);
          uio_wait_interrupt(&action->accelerator.uio_fd);
        }while(acc_interrupt_read(&action->accelerator) == 0x00);
  }
  acc_interrupt_clear(&action->accelerator);
  
  // change back to normal mode
  acc_write_config(&action->accelerator, 0x00);
  printf("Node local data done\n"); 
}

static void zynq_task_complete(embb_mtapi_zynq_task_t* zynq_task) {
  int err = 0;
  if (embb_mtapi_node_is_initialized()){
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, zynq_task->task)){
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, zynq_task->task);
  
      // move result and free cma buffers
      if(local_task->result_size > 0){
        assert(zynq_task->cma_result_buffer.virt_addr != NULL);
        
        memcpy(local_task->result_buffer, zynq_task->cma_result_buffer.virt_addr, local_task->result_size);
     
        err = cma_free_buf(&zynq_task->cma_result_buffer);

      }
      
      if(local_task->arguments_size > 0){
        err |= cma_free_buf(&zynq_task->cma_arguments);
      }
      
      if(err != 0){
        printf("ERROR: Could not free CMA buffers!\r\n");
      }
      // free task
      embb_free(zynq_task);
      
      // decrease action num
      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        embb_mtapi_action_t * local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
            node->action_pool, local_task->action);

        embb_atomic_fetch_and_add_int(&local_action->num_tasks,
          -(int)local_task->attributes.num_instances);
      }
      
      // set status
      embb_mtapi_task_set_state(local_task, MTAPI_TASK_COMPLETED);
      
      if (MTAPI_NULL != local_task->attributes.complete_func) {
        local_task->attributes.complete_func(zynq_task->task, MTAPI_NULL);
      }

    }
  }
}

static void zynq_task_start(acc_t* accelerator, embb_mtapi_zynq_task_t* zynq_task){
  
  if(embb_mtapi_node_is_initialized()){
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if(embb_mtapi_task_pool_is_handle_valid(
      node->task_pool, zynq_task->task)){
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(
          node->task_pool, zynq_task->task);
   
      if(local_task->arguments_size > 0){
        assert(zynq_task->cma_arguments.virt_addr != NULL);
        assert(local_task->arguments != NULL);
        // copy arguments

        memcpy(zynq_task->cma_arguments.virt_addr, local_task->arguments, local_task->arguments_size);
        
        // write arguments physical address
        acc_write_arguments_addr(accelerator, &zynq_task->cma_arguments);
      }
      if(local_task->result_size > 0){
        // write result buffer physical address
        acc_write_result_addr(accelerator, &zynq_task->cma_result_buffer);
      }
      acc_write_arguments_size(accelerator, local_task->arguments_size);
      acc_write_result_size(accelerator, local_task->result_size);
      
      // start accelerator
      acc_start(accelerator);
    }
  }

}

/**
 * Represents a callback function that is called when a plugin action is about
 * to start a plugin task.
 * This function should return MTAPI_SUCCESS if the task could be started and
 * the appropriate MTAPI_ERR_* if not.
 */
 
static void zynq_task_enqueue(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status){
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int err = 0;
  
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

        embb_mtapi_zynq_action_t * zynq_action =
          (embb_mtapi_zynq_action_t*)local_action->plugin_data;
        embb_mtapi_zynq_task_t * zynq_task =
          (embb_mtapi_zynq_task_t*)embb_alloc(
            sizeof(embb_mtapi_zynq_task_t));
          
          
        zynq_task->task = task;
        // Allocate CMA buffers
        
        if(0 < local_task->arguments_size){
          zynq_task->cma_arguments.len = (uint32_t)local_task->arguments_size;
          zynq_task->cma_arguments.cacheable = zynq_action->cacheable;
          err = cma_alloc_buf(&zynq_task->cma_arguments);
        }
        else{
          zynq_task->cma_arguments.virt_addr = NULL;
        }
        if(err != 0){
          printf("ERROR: Failed to allocate CMA buffer! \r\n");
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_ERROR);
          local_status = MTAPI_ERR_ACTION_FAILED;
          mtapi_status_set(status, local_status);
          return;
        }
        
        if(0 < local_task->result_size){
          zynq_task->cma_result_buffer.len = (uint32_t)local_task->result_size;
          zynq_task->cma_result_buffer.cacheable = zynq_action->cacheable;         
          err |= cma_alloc_buf(&zynq_task->cma_result_buffer);
        } 
        else {
          zynq_task->cma_result_buffer.virt_addr = NULL;
        }
        
        if(err != 0){
          printf("ERROR: Failed to allocate CMA buffer! \r\n");
          cma_free_buf(&zynq_task->cma_arguments);
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_ERROR);
          local_status = MTAPI_ERR_ACTION_FAILED;
          mtapi_status_set(status, local_status);
          return;
        }
        
        embb_mtapi_zynq_task_queue_push_back(&zynq_action->zynq_task_queue, zynq_task);

        
        embb_mtapi_task_set_state(local_task, MTAPI_TASK_RUNNING);
        // start 
        if (0 != err) {
          printf("ERROR IN TASK ENQUEUE \r\n");
          embb_mtapi_task_set_state(local_task, MTAPI_TASK_ERROR);
          local_status = MTAPI_ERR_ACTION_FAILED;
        }
        else{
          local_status = MTAPI_SUCCESS;
        }
      }
    }
  }
  mtapi_status_set(status, local_status);
  
}

/**
 * Represents a callback function that is called when a plugin task is about
 * to be canceled.
 * This function should return MTAPI_SUCCESS if the task could be canceled and
 * the appropriate MTAPI_ERR_* if not.
 * Not possible to cancel a task!
 */
static void zynq_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status){
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  
  EMBB_UNUSED(task);  
  mtapi_status_set(status, local_status);
}
  
  
void mtapi_zynq_plugin_initialize(
  MTAPI_IN char* uio_name,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN; 
  
  // initialize uio interrupt controller
  if(strlen(uio_name)>MAX_UIO_NAME_SIZE){
      printf("ERROR: uio name too long\r\n");
  }
  else{
    strcpy(uio_base.uio_name, uio_name);
  
    if (uio_initialize(uio_base.uio_name, &uio_base.uio_id) == 0){
      local_status = MTAPI_SUCCESS;
    }
  }
  
  mtapi_status_set(status, local_status);
}

void mtapi_zynq_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  
  local_status = MTAPI_SUCCESS;
  mtapi_status_set(status, local_status);
}
  
// destroy objects
static void zynq_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int err;
  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t * local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
        node->action_pool, action);
      embb_mtapi_zynq_action_t * zynq_action =
        (embb_mtapi_zynq_action_t *)local_action->plugin_data;
      
      // join thread
      embb_atomic_store_int(&zynq_action->run, 0);
      embb_thread_join(&zynq_action->thread, &err);
      embb_atomic_destroy_int(&zynq_action->run);
      
      acc_interrupt_done_disable(&zynq_action->accelerator);
      acc_interrupt_global_disable(&zynq_action->accelerator);
      
      uio_close(&zynq_action->accelerator.uio_fd);
      if(zynq_action->accelerator.uio_name == MTAPI_NULL){
        acc_release(&zynq_action->accelerator);
      }
      else{
        uio_release(&zynq_action->accelerator);
      }
      
      embb_mtapi_zynq_task_queue_finalize(&zynq_action->zynq_task_queue);
      
      if(zynq_action->node_local_data_size > 0){
        err |= cma_free_buf(&zynq_action->cma_node_local_data);
      }
      
      embb_free(zynq_action);
      local_status = MTAPI_SUCCESS;
    }
  }
  
  mtapi_status_set(status, local_status);
}

mtapi_action_hndl_t mtapi_zynq_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN char* uio_name,
  MTAPI_IN uint32_t phys_addr,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_IN mtapi_boolean_t cacheable,
  MTAPI_OUT mtapi_status_t* status){
    
    
    int err;
    err = 0;
    mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
    
    embb_mtapi_zynq_action_t * action =
    (embb_mtapi_zynq_action_t*)embb_alloc(
      sizeof(embb_mtapi_zynq_action_t));
      
    mtapi_action_hndl_t action_hndl = { 0, 0 }; // invalid handle
    
    embb_atomic_init_int(&action->run, 0);
    action->accelerator.phys_address = phys_addr;
    action->cacheable = cacheable;
    
    if(uio_name == MTAPI_NULL){
      // Use dev/mem for memory mapping
      printf("Use /dev/mem \n");
      acc_initialize(&action->accelerator);
      err |= uio_open(&action->accelerator.uio_fd, &uio_base.uio_id);
    }
    else{
      // Use uio for memory mapping
      if(strlen(uio_name)>MAX_UIO_NAME_SIZE){
        printf("ERROR: uio name too long\r\n");
        return action_hndl;
      }
      strcpy(action->accelerator.uio_name, uio_name);
      // find uio id
      uio_initialize(action->accelerator.uio_name, &action->accelerator.uio_id);
      uio_open(&action->accelerator.uio_fd, &action->accelerator.uio_id);
      printf("Use UIO mapping \n");
      uio_mmap(&action->accelerator);
    }
    acc_interrupt_clear(&action->accelerator);
    acc_interrupt_done_enable(&action->accelerator);
    acc_interrupt_global_enable(&action->accelerator);
    
    embb_mtapi_zynq_task_queue_initialize(&action->zynq_task_queue);
    
    
    action->node_local_data = node_local_data;
    action->node_local_data_size = node_local_data_size;
    if(node_local_data_size > 0){
      action->cma_node_local_data.len = (uint32_t)node_local_data_size;
      action->cma_node_local_data.cacheable = action->cacheable;
      err |= cma_alloc_buf(&action->cma_node_local_data);
    }
    else{
      action->cma_node_local_data.virt_addr = NULL;
    }
    
    embb_atomic_store_int(&action->run, 1);
    err |= embb_thread_create(&action->thread, NULL, embb_mtapi_zynq_thread, (void*)action);
    
    if(err != 0){
      printf("ERROR in action create r\n");
    }
    
    action_hndl = mtapi_ext_plugin_action_create(
      job_id,
      zynq_task_enqueue,
      zynq_task_cancel,
      zynq_action_finalize,
      action,
      node_local_data,
      node_local_data_size,
      MTAPI_NULL,
      &local_status);
	  
	  mtapi_status_set(status, local_status);
    
    return action_hndl;
}