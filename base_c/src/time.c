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

#include <embb/base/c/time.h>
#include <embb/base/c/internal/platform.h>
#include <assert.h>

void embb_time_now(embb_time_t* time) {
  embb_time_in(time, embb_duration_zero());
}

int embb_time_compare(const embb_time_t* lhs, const embb_time_t* rhs) {
  assert(lhs != NULL && rhs != NULL);
  assert(lhs->nanoseconds < 1000000000 && rhs->nanoseconds < 1000000000);
  if (lhs->seconds > rhs->seconds) {
    return 1;
  } else if (lhs->seconds < rhs->seconds) {
    return -1;
  } else { /* Seconds are equal */
    if (lhs->nanoseconds > rhs->nanoseconds) {
      return 1;
    } else if (lhs->nanoseconds < rhs->nanoseconds) {
      return -1;
    }
  }
  return 0;
}


#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

int embb_time_in(embb_time_t* time, const embb_duration_t* duration) {
  if (time == NULL || duration == NULL) {
    return EMBB_ERROR;
  }
  /* Get system time */
  SYSTEMTIME system_time;
  GetLocalTime(&system_time);
  /* Convert system time to file time */
  FILETIME file_time;
  SystemTimeToFileTime(&system_time, &file_time); /* Has 100 nanosec. ticks */
  /* Convert file time to windows unsigned large integer union type */
  ULARGE_INTEGER win_time;
  win_time.LowPart = file_time.dwLowDateTime;
  win_time.HighPart = file_time.dwHighDateTime;
  /* Convert windows time type to EMBB type */
  time->seconds = win_time.QuadPart / 10000000;
  time->nanoseconds = win_time.QuadPart % 10000000;
  /* Add duration to time */
  int carry = (int)((time->nanoseconds + duration->nanoseconds) / 1000000000);
  if ((double)time->seconds + duration->seconds + carry >
    EMBB_TIME_MAX_SECONDS) {
    return EMBB_OVERFLOW;
  }
  time->seconds += duration->seconds + carry;
  time->nanoseconds += duration->nanoseconds - carry * 1000000000;
  return EMBB_SUCCESS;
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */


#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

int embb_time_in(embb_time_t* time, const embb_duration_t* duration) {
  if (time == NULL || duration == NULL) {
    return EMBB_ERROR;
  }
  struct timespec unix_time;
  clock_gettime(CLOCK_REALTIME, &unix_time);
  time->seconds = unix_time.tv_sec;
  time->nanoseconds = unix_time.tv_nsec;
  int carry = (time->nanoseconds + duration->nanoseconds) / 1000000000;
  if ((double)time->seconds + duration->seconds + carry >
    EMBB_TIME_MAX_SECONDS) {
    return EMBB_OVERFLOW;
  }
  time->seconds += duration->seconds + carry;
  time->nanoseconds += duration->nanoseconds - carry * 1000000000;
  return EMBB_SUCCESS;
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */

