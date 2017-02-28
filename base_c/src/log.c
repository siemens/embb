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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <embb/base/c/log.h>

#include <embb/base/c/internal/config.h>
#include <embb/base/c/internal/unused.h>

void embb_log_write_file(
  void * context,
  char const * message) {
  assert(context != NULL);
  FILE * ff = (FILE*)context;
  fprintf(ff, "%s", message);
  fflush(ff);
}

static embb_log_level_t embb_log_global_log_level =
  EMBB_LOG_LEVEL_NONE;

static void * embb_log_global_log_context = NULL;

static embb_log_function_t embb_log_global_log_function = embb_log_write_file;

void embb_log_set_log_level(
  embb_log_level_t log_level) {
  embb_log_global_log_level = log_level;
}

void embb_log_set_log_function(
  void * context,
  embb_log_function_t func) {
  embb_log_global_log_context = context;
  embb_log_global_log_function = func;
}

void embb_log_write_internal(
  char const * channel,
  embb_log_level_t log_level,
  char const * message,
  va_list argp) {
  if (log_level <= embb_log_global_log_level) {
    char * log_level_str = "     ";
    char const * channel_str = channel;
    void * log_context = embb_log_global_log_context;
    if (NULL == channel_str) {
      channel_str = " global ";
    }
    if (NULL == log_context) {
      log_context = (void*)stdout;
    }
    switch (log_level) {
    case EMBB_LOG_LEVEL_ERROR:
      log_level_str = "ERROR";
      break;
    case EMBB_LOG_LEVEL_WARNING:
      log_level_str = "WARN ";
      break;
    case EMBB_LOG_LEVEL_INFO:
      log_level_str = "INFO ";
      break;
    case EMBB_LOG_LEVEL_TRACE:
      log_level_str = "TRACE";
      break;

    case EMBB_LOG_LEVEL_NONE:
    default:
      break;
    }
#if defined(EMBB_PLATFORM_COMPILER_MSVC)
    char msg_buffer[400];
    char buffer[500];
    vsprintf_s(msg_buffer, sizeof(msg_buffer), message, argp);
    sprintf_s(buffer, sizeof(buffer), "[%s] - [%s] %s",
      channel_str, log_level_str, msg_buffer);
    embb_log_global_log_function(log_context, buffer);
#elif defined(EMBB_PLATFORM_COMPILER_GNUC)
    char msg_buffer[400];
    char buffer[500];
    vsnprintf(msg_buffer, sizeof(msg_buffer), message, argp);
    snprintf(buffer, sizeof(buffer), "[%s] - [%s] %s",
      channel_str, log_level_str, msg_buffer);
    embb_log_global_log_function(log_context, buffer);
#else
    embb_log_global_log_function(log_context, "[");
    embb_log_global_log_function(log_context, channel_str);
    embb_log_global_log_function(log_context, "] - [");
    embb_log_global_log_function(log_context, log_level_str);
    embb_log_global_log_function(log_context, "] ");
    /* no secure formatting possible, sorry */
    embb_log_global_log_function(log_context, message);
#endif
  }
}

void embb_log_write(
  char const * channel,
  embb_log_level_t log_level,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, log_level, message, argp);
  va_end(argp);
}

#ifdef EMBB_DEBUG
void embb_log_trace(
  char const * channel,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_TRACE, message, argp);
  va_end(argp);
}

void embb_log_info(
  char const * channel,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_INFO, message, argp);
  va_end(argp);
}
#else
#undef embb_log_trace
void embb_log_trace(
  char const * channel,
  char const * message,
  ...) {
  EMBB_UNUSED(channel);
  EMBB_UNUSED(message);
}

#undef embb_log_info
void embb_log_info(
  char const * channel,
  char const * message,
  ...) {
  EMBB_UNUSED(channel);
  EMBB_UNUSED(message);
}
#endif

void embb_log_warning(
  char const * channel,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_WARNING, message, argp);
  va_end(argp);
}

void embb_log_error(
  char const * channel,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_ERROR, message, argp);
  va_end(argp);
}
