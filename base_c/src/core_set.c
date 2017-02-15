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

#include <embb/base/c/core_set.h>
#include <embb/base/c/internal/platform.h>
#include <embb/base/c/internal/bitset.h>
#include <embb/base/c/internal/unused.h>
#include <limits.h>
#include <assert.h>

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

/**
 * For handling of more than 64 logical processors on Windows.
 */
typedef struct processor_info_t {
  unsigned short group_count;
  unsigned short processor_counts[64];
} processor_info_t;

/**
 * Holds information about Windows processor groups.
 *
 * Is initialized on the first call of embb_core_set_init.
 *
 * !!! Not used at the moment. Results in a limitation of 64 logical processors.
 */
processor_info_t processor_info;

unsigned int embb_core_count_available() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
}

void embb_core_set_init(embb_core_set_t* core_set, int initializer) {
  assert(core_set != NULL);
  assert(embb_core_count_available() < 64 &&
    "Core sets are only supported up to 64 processors!");

  /* Cache windows processor grouping information */
  if (processor_info.group_count == 0) {
    /* Set relation group */
    LOGICAL_PROCESSOR_RELATIONSHIP rel = (LOGICAL_PROCESSOR_RELATIONSHIP)4;
    /* Assume only one element of SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX is
       returned to the buffer. */
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer;
    /* The length is that of the buffer */
    DWORD length = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX);
    BOOL status = GetLogicalProcessorInformationEx(rel, &buffer, &length);
    assert(status == TRUE);
    EMBB_UNUSED_IN_RELEASE(status);
    processor_info.group_count = buffer.Group.ActiveGroupCount;
    for (unsigned short i = 0; i < processor_info.group_count; i++) {
      processor_info.processor_counts[i] =
        (unsigned short)(buffer.Group.GroupInfo[i].ActiveProcessorCount);
    }
  }

  if (initializer == 0) {
    embb_bitset_clear_all(&core_set->rep);
  } else {
    embb_bitset_set_n(&core_set->rep, embb_core_count_available());
  }
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

#ifdef EMBB_PLATFORM_HAS_HEADER_SYSINFO
#include <sys/sysinfo.h>
#elif defined EMBB_PLATFORM_HAS_SC_NPROCESSORS_ONLN
#include <unistd.h>
#endif

unsigned int embb_core_count_available() {
#ifdef EMBB_PLATFORM_HAS_HEADER_SYSINFO
  return get_nprocs();
#elif defined EMBB_PLATFORM_HAS_SC_NPROCESSORS_ONLN
  return (int)sysconf(_SC_NPROCESSORS_ONLN);
#else
#error "No implementation for embb_core_count_available()!"
#endif
}

void embb_core_set_init(embb_core_set_t* core_set, int initializer) {
  assert(core_set != NULL);
  assert(embb_core_count_available() < 64 &&
    "Core sets are only supported up to 64 processors!");
  if (initializer == 0) {
    embb_bitset_clear_all(&core_set->rep);
  } else {
    embb_bitset_set_n(&core_set->rep, embb_core_count_available());
  }
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */

void embb_core_set_add(embb_core_set_t* core_set, unsigned int core_number) {
  assert(core_set != NULL);
  assert(core_number < embb_core_count_available());
  embb_bitset_set(&core_set->rep, core_number);
}

void embb_core_set_remove(embb_core_set_t* core_set, unsigned int core_number) {
  assert(core_set != NULL);
  assert(core_number < embb_core_count_available());
  embb_bitset_clear(&core_set->rep, core_number);
}

int embb_core_set_contains(const embb_core_set_t* core_set,
  unsigned int core_number) {
  assert(core_set != NULL);
  assert(core_number < embb_core_count_available());
  return (int)(embb_bitset_is_set(&core_set->rep, core_number));
}

void embb_core_set_intersection(embb_core_set_t* set1,
                                const embb_core_set_t* set2) {
  assert(set1 != NULL);
  assert(set2 != NULL);
  embb_bitset_intersect(&set1->rep, set2->rep);
}

void embb_core_set_union(embb_core_set_t* set1, const embb_core_set_t* set2) {
  assert(set1 != NULL);
  assert(set2 != NULL);
  embb_bitset_union(&set1->rep, set2->rep);
}

unsigned int embb_core_set_count(const embb_core_set_t* core_set) {
  assert(core_set != NULL);
  return embb_bitset_count(&core_set->rep);
}
