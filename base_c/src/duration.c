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

#include <embb/base/c/duration.h>
#include <embb/base/c/internal/platform.h>
#include <limits.h>
#include <assert.h>

const embb_duration_t* embb_duration_max() {
  static embb_duration_t max = { EMBB_DURATION_MAX_SECONDS, 0 };
  return &max;
}

const embb_duration_t* embb_duration_min() {
  static embb_duration_t min = { 0, EMBB_DURATION_MIN_NANOSECONDS };
  return &min;
}

const embb_duration_t* embb_duration_zero() {
  static embb_duration_t zero = { 0, 0 };
  return &zero;
}

int embb_duration_set_nanoseconds(embb_duration_t* duration,
                                  unsigned long long nanoseconds) {
  if (duration == NULL) {
    return EMBB_ERROR;
  }
  if (nanoseconds > 0) {
    if (embb_duration_min()->nanoseconds > nanoseconds) {
      duration->seconds = 0;
      duration->nanoseconds = 0;
      return EMBB_UNDERFLOW;
    }
    const embb_duration_t* max = embb_duration_max();
    if (max->seconds * 1000000000 + max->nanoseconds < nanoseconds) {
      duration->seconds = max->seconds;
      duration->nanoseconds = max->nanoseconds;
      return EMBB_OVERFLOW;
    }
  }
  duration->seconds = nanoseconds / 1000000000;
  duration->nanoseconds = (nanoseconds % 1000000000);
  return EMBB_SUCCESS;
}

int embb_duration_set_microseconds(embb_duration_t* duration,
                                   unsigned long long microseconds) {
  if (duration == NULL) {
    return EMBB_ERROR;
  }
  if (microseconds > 0) {
    if (embb_duration_min()->nanoseconds > microseconds*1000) {
      duration->seconds = 0;
      duration->nanoseconds = 0;
      return EMBB_UNDERFLOW;
    }
    const embb_duration_t* max = embb_duration_max();
    if (max->seconds * 1000000 + max->nanoseconds / 1000 < microseconds) {
      duration->seconds = max->seconds;
      duration->nanoseconds = max->nanoseconds;
      return EMBB_OVERFLOW;
    }
  }
  duration->seconds = microseconds / 1000000;
  duration->nanoseconds = (microseconds % 1000000) * 1000;
  return EMBB_SUCCESS;
}

int embb_duration_set_milliseconds(embb_duration_t* duration,
                                   unsigned long long milliseconds) {
  if (duration == NULL) {
    return EMBB_ERROR;
  }
  if (milliseconds > 0) {
    if (embb_duration_min()->nanoseconds > milliseconds*1000000) {
      duration->seconds = 0;
      duration->nanoseconds = 0;
      return EMBB_UNDERFLOW;
    }
    const embb_duration_t* max = embb_duration_max();
    if (max->seconds * 1000 + max->nanoseconds / 1000000 < milliseconds) {
      duration->seconds = max->seconds;
      duration->nanoseconds = max->nanoseconds;
      return EMBB_OVERFLOW;
    }
  }
  duration->seconds = milliseconds / 1000;
  duration->nanoseconds = (milliseconds % 1000) * 1000000;
  return EMBB_SUCCESS;
}

int embb_duration_set_seconds(embb_duration_t* duration,
                              unsigned long long seconds) {
  if (duration == NULL) {
    return EMBB_ERROR;
  }
  if (seconds > 0) {
    if (embb_duration_min()->nanoseconds > seconds*1000000000) {
      duration->seconds = 0;
      duration->nanoseconds = 0;
      return EMBB_UNDERFLOW;
    }
    const embb_duration_t* max = embb_duration_max();
    if (max->seconds + max->nanoseconds / 1000000000 < seconds) {
      duration->seconds = max->seconds;
      duration->nanoseconds = max->nanoseconds;
      return EMBB_OVERFLOW;
    }
  }
  duration->seconds = seconds;
  duration->nanoseconds = 0;
  return EMBB_SUCCESS;
}

int embb_duration_add(embb_duration_t* lhs, const embb_duration_t* rhs) {
  if (lhs == NULL || rhs == NULL) {
    return EMBB_ERROR;
  }
  int carry = (int)((lhs->nanoseconds + rhs->nanoseconds) / 1000000000);
  if (lhs->seconds + rhs->seconds + carry > EMBB_DURATION_MAX_SECONDS) {
    lhs->seconds = 0;
    lhs->nanoseconds = 0;
    return EMBB_OVERFLOW;
  }
  lhs->nanoseconds = (lhs->nanoseconds + rhs->nanoseconds) % 1000000000;
  lhs->seconds = lhs->seconds + rhs->seconds + carry;
  return EMBB_SUCCESS;
}

int embb_duration_as_nanoseconds(const embb_duration_t* duration,
                                 unsigned long long* nanoseconds) {
  if (duration == NULL || nanoseconds == NULL) {
    return EMBB_ERROR;
  }
  if (duration->seconds*1000000000.0 + duration->nanoseconds > ULLONG_MAX) {
    return EMBB_OVERFLOW;
  }
  *nanoseconds = duration->seconds*1000000000 + duration->nanoseconds;
  return EMBB_SUCCESS;
}

int embb_duration_as_microseconds(const embb_duration_t* duration,
                                  unsigned long long* microseconds) {
  if (duration == NULL || microseconds == NULL) {
    return EMBB_ERROR;
  }
  if (duration->nanoseconds % 1000 > 0) {
    return EMBB_UNDERFLOW;
  }
  if (duration->seconds*1000000.0 + duration->nanoseconds/1000 > ULLONG_MAX) {
    return EMBB_OVERFLOW;
  }
  *microseconds = duration->seconds*1000000 + duration->nanoseconds / 1000;
  return EMBB_SUCCESS;
}

int embb_duration_as_milliseconds(const embb_duration_t* duration,
                                  unsigned long long* milliseconds) {
  if (duration == NULL || milliseconds == NULL) {
    return EMBB_ERROR;
  }
  if (duration->nanoseconds % 1000000 > 0) {
    return EMBB_UNDERFLOW;
  }
  if (duration->seconds*1000.0 + duration->nanoseconds/1000000 > ULLONG_MAX) {
    return EMBB_OVERFLOW;
  }
  *milliseconds = duration->seconds*1000 + duration->nanoseconds / 1000000;
  return EMBB_SUCCESS;
}

int embb_duration_as_seconds(const embb_duration_t* duration,
                             unsigned long long* seconds) {
  if (duration == NULL || seconds == NULL) {
    return EMBB_ERROR;
  }
  if (duration->nanoseconds % 1000000000 > 0) {
    return EMBB_UNDERFLOW;
  }
  *seconds = duration->seconds;
  return EMBB_SUCCESS;
}

int embb_duration_compare(const embb_duration_t* lhs,
                          const embb_duration_t* rhs) {
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
