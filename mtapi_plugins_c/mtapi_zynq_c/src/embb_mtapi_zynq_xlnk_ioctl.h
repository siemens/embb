#ifndef EMBB_MTAPI_ZYNQ_XLNK_IOCTL_H_
#define EMBB_MTAPI_ZYNQ_XLNK_IOCTL_H_

#include <sys/ioctl.h>
#include <stdint.h>

#define ALLOCATE_CMA_BUFFER	_IOWR('X', 2, unsigned long)
#define FREE_CMA_BUFFER		  _IOWR('X', 3, unsigned long)

#if __SIZEOF_POINTER__  == 4
	typedef uint32_t ptr_type;
#elif __SIZEOF_POINTER__  == 8
	typedef uint64_t ptr_type;
#else
	#error
#endif

union xlnk_arguments_u {
  struct __attribute__ ((__packed__)) {
    uint32_t len;
    int32_t id;
    ptr_type phys_addr;
    uint8_t cacheable;
  } allocate_buffer;
  struct __attribute__ ((__packed__)) {
    uint32_t id;
    ptr_type buffer;
  } free_buffer;
  struct __attribute__ ((__packed__)) {
    int32_t unused_0;
    ptr_type unused_1;
  } unused_struct_0;
  struct __attribute__ ((__packed__)) {
    int8_t unused_0[64];
    ptr_type unused_1;
    uint32_t unused_2;
    uint32_t unused_3;
  } unused_struct_1;
  struct __attribute__ ((__packed__)) {
    ptr_type unused_0;
    ptr_type unused_1;
    ptr_type unused_2;
    uint32_t unused_3;
    uint32_t unused_4;
    uint32_t unused_5;
    ptr_type unused_6;
    uint32_t unused_7;
    int32_t unused_8;
    uint32_t unused_9;
    uint32_t unused_10[5];
    uint32_t unused_11;
    uint32_t unused_12;
    ptr_type unused_13;
    uint32_t unused_14;
  } unused_struct_2;
  struct __attribute__ ((__packed__)) {
    ptr_type unused_0;
    uint32_t unused_1;
    uint32_t unused_2[5];
    uint32_t unused_3;
  } unused_struct_3;
  struct __attribute__ ((__packed__)) {
    ptr_type unused_0;
  } unused_struct_4;
  struct __attribute__ ((__packed__))  {
    ptr_type unused_0;
    uint32_t unused_1;
    uint32_t unused_2[8];
    int8_t unused_3[32];
    uint32_t unused_4;
  } unused_struct_5;
  struct __attribute__ ((__packed__)) {
    ptr_type unused_0;
  } unused_struct_6;
  struct __attribute__ ((__packed__)) {
    int8_t unused_0[32];
    uint32_t unused_1;
    ptr_type unused_2;
    uint32_t unused_3;
    uint32_t unused_4;
    uint32_t unused_5;
    uint32_t unused_6;
    uint32_t unused_7;
    uint32_t unused_8;
    uint32_t unused_9;
    uint32_t unused_10;
    uint32_t unused_11;
    uint32_t unused_12;
    uint32_t unused_13;
    uint32_t unused_14;
  } unused_struct_7;
  struct __attribute__ ((__packed__)) {
    int8_t unused_0[32];
    uint32_t unused_1;
    ptr_type unused_2;
    uint32_t unused_3;
    uint32_t unused_4;
    uint32_t unused_5;
    uint32_t unused_6;
    uint32_t unused_7;
  } unused_struct_8;
  struct __attribute__ ((__packed__)) {
    ptr_type unused_0;
    uint32_t unused_1;
    int32_t unused_2;
  } unused_struct_9;
  struct __attribute__ ((__packed__)) {
    ptr_type unused_0;
    int32_t unused_1;
    int32_t unused_2;
    int32_t unused_3;
    ptr_type unused_4;
    ptr_type unused_5;
  } unused_struct_10;
};

#endif // EMBB_MTAPI_ZYNQ_XLNK_IOCTL_H_