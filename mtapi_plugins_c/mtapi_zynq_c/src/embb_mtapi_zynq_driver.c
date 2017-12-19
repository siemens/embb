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

// wann assert, wann status = ERROR ?

#define _SVID_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <inttypes.h>

#include <embb_mtapi_zynq_driver.h>
#include <embb_mtapi_zynq_xlnk_ioctl.h>

#include <embb/base/c/thread.h>

 #include <stdio.h>

/*************************************************************************/
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


/*************************************************************************/
/* Xlnk Driver Functions */ 

int cma_alloc_buf(cma_buffer_t* cma_buf){
  union xlnk_arguments_u xlnk_arguments;
  char file[10];
  int fd;
  int err;
  uint8_t rest = 0;
#if __SIZEOF_POINTER__ == 4
  rest = cma_buf->len % 4;
#elif __SIZEOF_POINTER__ == 8
  rest = cma_buf->len % 8;
#endif
  
  xlnk_arguments.allocate_buffer.len = cma_buf->len + rest;
  xlnk_arguments.allocate_buffer.cacheable = cma_buf->cacheable;
  
  sprintf(file, "/dev/xlnk");
  if ((fd = open(file, O_RDWR)) < 0) {
    printf("ERROR: Could not open file: %s \n", file);   
    return 1;
  }
  
  err = ioctl(fd, ALLOCATE_CMA_BUFFER, &xlnk_arguments);
  
  if(!err){
    cma_buf->phys_addr = xlnk_arguments.allocate_buffer.phys_addr;
    cma_buf->id = xlnk_arguments.allocate_buffer.id;
    // buf_id << (16-PAGE_SHIFT) = 4
    uint32_t page_size = getpagesize();
    uint32_t page_shift;
    for(page_shift=0; page_shift<32; page_shift++){
      page_size = page_size >> 1;
      if(page_size == 0x1){
        break;
      }
    }
    page_shift+=1;
    // TODO
    cma_buf->virt_addr = (uint32_t*)mmap(NULL, cma_buf->len + rest, PROT_READ|PROT_WRITE, MAP_SHARED, fd, cma_buf->id << (page_shift+4));
  }
  
  close(fd);
  
  return err;
}

int cma_free_buf(cma_buffer_t* cma_buf){
  union xlnk_arguments_u xlnk_arguments;
  char file[10];
  int fd;
  int err;
  
  uint8_t rest = 0;
#if __SIZEOF_POINTER__ == 4
  rest = cma_buf->len % 4;
#elif __SIZEOF_POINTER__ == 8
  rest = cma_buf->len % 8;
#endif
  
  if(cma_buf->virt_addr == NULL){
    return 1;
  }
  
  munmap((void*)cma_buf->virt_addr, cma_buf->len + rest);
  
  xlnk_arguments.free_buffer.id = cma_buf->id;
  
  sprintf(file, "/dev/xlnk");
  if ((fd = open(file, O_RDWR)) < 0) {
    printf("ERROR: Could not open file: %s \n", file);   
    return 1;
  }
  
  err = ioctl(fd, FREE_CMA_BUFFER, &xlnk_arguments);
  close(fd);

  return err;
}

/*************************************************************************/
/* MTAPI FPGA Accelerator Driver Functions */ 

int acc_initialize(acc_t* acc){
  char file[10];
  int fd;
  
  assert(acc != NULL);
  sprintf(file, "/dev/mem");
  if ((fd = open(file, O_RDWR)) < 0) {
    printf("ERROR: Could not open file: %s \n", file);   
    return 1;
  }
  
  //acc->control_base_address = (uint32_t*)cma_mmap((unsigned long)acc->phys_address, MAP_SIZE);
  
  unsigned page_addr = (acc->phys_address & (~(getpagesize()-1)));
	unsigned page_offset = acc->phys_address - page_addr;

  acc->control_base_address = (uint32_t*)mmap(NULL, MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);
  acc->control_base_address+= page_offset;
  
  close(fd);
  return 0;
}

int acc_release(acc_t* acc){
  assert(acc != NULL);
  munmap((void*)acc->control_base_address, MAP_SHARED);
  return 0;
}

void acc_interrupt_global_disable(acc_t* acc){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_GIE, 0x0);
}

void acc_interrupt_global_enable(acc_t* acc){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_GIE, 0x1);
}

void acc_interrupt_done_enable(acc_t* acc){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_IER, 0x1);
}

void acc_interrupt_done_disable(acc_t* acc){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_IER, 0x0);
}

void acc_interrupt_clear(acc_t* acc){
  assert(acc != NULL);
  uint32_t reg;
  reg = read_32bit_reg(acc->control_base_address, CONTROL_ADDR_ISR);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_ISR, reg);
}

uint32_t acc_interrupt_read(acc_t* acc){
  assert(acc != NULL);
  return read_32bit_reg(acc->control_base_address, CONTROL_ADDR_ISR);
}

void acc_start(acc_t* acc){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_AP_CTRL, 0x01);
}

void acc_write_arguments_addr(acc_t* acc, cma_buffer_t* cma_arguments){
  assert(acc != NULL);
  assert(cma_arguments != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_ARGUMENTS, (uint32_t) cma_arguments->phys_addr); 
}

void acc_write_arguments_size(acc_t* acc, uint32_t arguments_size){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_ARGUMENTS_SIZE, arguments_size); 
}

void acc_write_result_addr(acc_t* acc, cma_buffer_t* cma_result_buffer){
  assert(acc != NULL);
  assert(cma_result_buffer != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_RESULT_BUFFER, (uint32_t) cma_result_buffer->phys_addr); 
}

void acc_write_result_size(acc_t* acc, uint32_t result_size){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_RESULT_BUFFER_SIZE, result_size); 
}

void acc_write_node_local_data_addr(acc_t* acc, cma_buffer_t* cma_node_local_data){
  assert(acc != NULL);
  assert(cma_node_local_data != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_NODE_LOCAL_DATA, (uint32_t) cma_node_local_data->phys_addr); 
}

void acc_write_node_local_data_size(acc_t* acc, uint32_t node_local_data_size){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_NODE_LOCAL_DATA_SIZE, node_local_data_size); 
}

void acc_write_config(acc_t* acc, uint32_t reg){
  assert(acc != NULL);
  write_32bit_reg(acc->control_base_address, CONTROL_ADDR_CONFIG, reg); 
}



/* UIO Driver Functions */ 

void uio_mmap(acc_t* acc){
  assert(acc->uio_fd != NULL);
  acc->control_base_address = (uint32_t*)mmap(NULL, MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, acc->uio_fd, 0* getpagesize());
}

void uio_release(acc_t* acc){
  munmap((void*)acc->control_base_address, MAP_SIZE);
}

int filter_dir(const struct dirent *entry)
{
  int i = strcmp(entry->d_name, "uio\n");
  return (i>0);
}

int uio_initialize(const char* name, int* id){
  
  char file[MAX_UIO_PATH_SIZE];
  FILE* file_ptr;
  struct dirent **namelist;
  int n, i, j;
  
  char tmp_name[MAX_UIO_NAME_SIZE];
  
  if(strlen(name)>MAX_UIO_NAME_SIZE){
    printf("ERROR: uio name too long\r\n");
    return 1;
  }
  
  n = scandir("/sys/class/uio/", &namelist, filter_dir, alphasort);
  if(n <0){
    return 1;
  }
  
  
  // Get name and id of the uio devices
  for (i = 0;  i < n; i++) {
    strcpy(file, "/sys/class/uio/");
    strcat(file, namelist[i]->d_name);
    strcat(file, "/name");

    // get name
    file_ptr = fopen(file, "r");
    if(!file_ptr){
      printf("ERROR: File not existing!\n");
      return 2;
    }
    if (fgets(tmp_name, MAX_UIO_NAME_SIZE, file_ptr) == NULL){
      printf("ERROR: Fgets!\n");
      fclose(file_ptr);
      return 3;
    }
    fclose(file_ptr);
    
    for (j=0; (*tmp_name)&&(j<MAX_UIO_NAME_SIZE); j++) {
        if (*(tmp_name+j) == '\n') *(tmp_name+j) = 0;
    }
    
    if(strcmp(name, tmp_name) == 0){
      *id = atoi((namelist[i]->d_name) + 3);
      printf("Found id: %d\n", *id);
      break;
    }
  }
  // Free namelist pointers which are allocated by scandir
  if (namelist!= NULL){
    for (i = 0; i < n; i++)
    {
      free(namelist[i]);
    }
    free(namelist);
  }
  
  return 0;
}

int uio_open(int *fd, int *id){
  char file[MAX_UIO_PATH_SIZE];
  
  assert(fd != NULL);
  sprintf(file, "/dev/uio%d", *id);
  if ((*fd = open(file, O_RDWR)) < 0) {
    printf("ERROR: Could not open file: %s \n", file);   
    return 1;
  }
  else{
    return 0;
  }
}


void uio_close(int *fd){
  assert(fd != NULL);
  close(*fd);
}

int uio_wait_interrupt(int *fd){
  assert(fd != NULL);
  int int_count;
  int err;
  // blocking read
  err = read(*fd, &int_count, sizeof(int_count));
  if(err != sizeof(int_count)){
    printf("ERROR when reading uio!");
    return 1;
  }
  return 0;
}

int uio_enable_interrupt(int *fd){
  assert(fd != NULL);
  int int_count = 1;
  int err;
  err = write(*fd, &int_count, sizeof(int_count));
  if(err != sizeof(int_count)){
    printf("Error when writing uio dev!\n");
    return 1;
  }
  return 0;
}

/*************************************************************************/