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

#include <embb/base/c/condition_variable.h>
#include <embb/base/c/time.h>
#include <embb/base/c/internal/unused.h>
#include <assert.h>
#include <stdio.h>

int embb_condition_wait_for(embb_condition_t* condition_var,
                            embb_mutex_t* mutex,
                            const embb_duration_t* duration) {
  if (condition_var == NULL || mutex == NULL) {
    return EMBB_ERROR;
  }
  embb_time_t time;
  int status = embb_time_in(&time, duration);
  if (status != EMBB_SUCCESS) {
    return status;
  }
  return embb_condition_wait_until(condition_var, mutex, &time);
}

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

int embb_condition_init(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  InitializeConditionVariable(condition_var);
  return EMBB_SUCCESS;
}

int embb_condition_notify_one(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  WakeConditionVariable(condition_var);
  return EMBB_SUCCESS;
}

int embb_condition_notify_all(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  WakeAllConditionVariable(condition_var);
  return EMBB_SUCCESS;
}

int embb_condition_wait(embb_condition_t* condition_var,
                        embb_mutex_t* mutex) {
  if (condition_var == NULL || mutex == NULL) {
    return EMBB_ERROR;
  }
  if (SleepConditionVariableCS(condition_var, mutex, INFINITE)) {
    return EMBB_SUCCESS;
  }
  return EMBB_ERROR;
}

int embb_condition_wait_until(embb_condition_t* condition_var,
                              embb_mutex_t* mutex, const embb_time_t* time) {
  if (condition_var == NULL || mutex == NULL || time == NULL) {
    return EMBB_ERROR;
  }
  /* The Windows API needs a time duration, so we need to convert the given time
     by using the time now. */
  embb_time_t now;
  embb_time_now(&now);
  /* Check if absolute timepoint (in milliseconds) still is in the future */
  if ((time->seconds * 1000 + time->nanoseconds / 1000000)
      > (now.seconds * 1000 + now.nanoseconds / 1000000)) {
    /* Convert to (unsigned type) milliseconds and round up */
    DWORD time_diff = (DWORD) (
        time->seconds * 1000 + time->nanoseconds / 1000000
        - now.seconds * 1000 - now.nanoseconds / 1000000);
    if (SleepConditionVariableCS(condition_var, mutex, time_diff) == 0) {
      if (GetLastError() == ERROR_TIMEOUT) {
        return EMBB_TIMEDOUT;
      } else {
        return EMBB_ERROR;
      }
    }
  } else {
    return EMBB_TIMEDOUT;
  }
  return EMBB_SUCCESS;
}

int embb_condition_destroy(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  EMBB_UNUSED_IN_RELEASE(condition_var);
  return EMBB_SUCCESS;
}

#endif /* EMBB_PLATFORM_THREADING_WINTHREADS */

#ifdef EMBB_PLATFORM_THREADING_POSIXTHREADS

int embb_condition_init(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  int result = pthread_cond_init(condition_var, NULL);
  return result == 0 ? EMBB_SUCCESS : EMBB_ERROR;
}

int embb_condition_notify_one(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  int result = pthread_cond_signal(condition_var);
  return result == 0 ? EMBB_SUCCESS : EMBB_ERROR;
}

int embb_condition_notify_all(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  int result = pthread_cond_broadcast(condition_var);
  return result == 0 ? EMBB_SUCCESS : EMBB_ERROR;
}

int embb_condition_wait(embb_condition_t* condition_var, embb_mutex_t* mutex) {
  if (condition_var == NULL || mutex == NULL) {
    return EMBB_ERROR;
  }
  int result = pthread_cond_wait(condition_var, mutex);
  return result == 0 ? EMBB_SUCCESS : EMBB_ERROR;
}

int embb_condition_wait_until(embb_condition_t* condition_var,
                              embb_mutex_t* mutex, const embb_time_t* time) {
  if (condition_var == NULL || mutex == NULL || time == NULL) {
    return EMBB_ERROR;
  }
  /* Convert EMBB time to Unix time format */
  struct timespec unix_time;
  unix_time.tv_sec = time->seconds;
  unix_time.tv_nsec = time->nanoseconds;
  int result = pthread_cond_timedwait(condition_var, mutex, &unix_time);
  if (result == ETIMEDOUT) {
    return EMBB_TIMEDOUT;
  }
  if (result != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

int embb_condition_destroy(embb_condition_t* condition_var) {
  if (condition_var == NULL) {
    return EMBB_ERROR;
  }
  int status = pthread_cond_destroy(condition_var);
  if (status != 0) {
    return EMBB_ERROR;
  }
  return EMBB_SUCCESS;
}

#endif /* EMBB_PLATFORM_THREADING_POSIXTHREADS */
