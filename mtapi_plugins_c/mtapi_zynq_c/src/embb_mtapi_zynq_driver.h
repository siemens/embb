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

#ifndef EMBB_MTAPI_ZYNQ_DRIVER_H_
#define EMBB_MTAPI_ZYNQ_DRIVER_H_

// welche header dateien sollten alle inkludiert werden?
// forward: nur forward im header einfügen und definition nur in c datei?

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>



#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************/

#define MAX_UIO_NAME_SIZE	64
/* Typedefs */


struct uio_info_struct{
	int uio_id;
	char uio_name[MAX_UIO_NAME_SIZE];
};

typedef struct uio_info_struct uio_info_t;

struct acc_struct{
  uint32_t* control_base_address;
  uint32_t phys_address;
  int uio_id;
  int uio_fd;
  char uio_name[MAX_UIO_NAME_SIZE];
};

typedef struct acc_struct acc_t;

struct cma_buffer_struct{
  uint32_t phys_addr;
  uint32_t* virt_addr;
  int32_t id;
  uint32_t len; // len must be dividible by 4 or 8 depending on __SIZEOF_POINTER__
  uint8_t cacheable;
};

typedef struct cma_buffer_struct cma_buffer_t;

/*************************************************************************/


/* Function Prototypes */
int cma_alloc_buf(cma_buffer_t* cma_buf);
int cma_free_buf(cma_buffer_t* cma_buf);

int acc_initialize(acc_t* acc);
int acc_release(acc_t* acc);
void acc_interrupt_global_disable(acc_t* acc);
void acc_interrupt_global_enable(acc_t* acc);
void acc_interrupt_done_enable(acc_t* acc);
void acc_interrupt_done_disable(acc_t* acc);
void acc_interrupt_clear(acc_t* acc);
uint32_t acc_interrupt_read(acc_t* acc);


void acc_start(acc_t* acc);
void acc_write_arguments_addr(acc_t* acc, cma_buffer_t* cma_arguments);
void acc_write_arguments_size(acc_t* acc, uint32_t arguments_size);
void acc_write_result_addr(acc_t* acc, cma_buffer_t* cma_results);
void acc_write_result_size(acc_t* acc, uint32_t result_size);
void acc_write_node_local_data_addr(acc_t* acc, cma_buffer_t* cma_node_local_data);
void acc_write_node_local_data_size(acc_t* acc, uint32_t node_local_data_size);
void acc_write_config(acc_t* acc, uint32_t reg);

void uio_mmap(acc_t* acc);
void uio_release(acc_t* acc);
int uio_initialize(const char* name, int* id);
int uio_open(int *fd, int *id);
void uio_close(int *fd);
int uio_wait_interrupt(int *fd);
int uio_enable_interrupt(int *fd);

/*************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // EMBB_MTAPI_ZYNQ_DRIVER_H_
