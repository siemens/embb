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

#include <embb/base/log.h>
#include <embb/base/c/internal/unused.h>

namespace embb {
namespace base {

Log::Log() {
  // empty
}

void Log::SetLogLevel(
  embb_log_level_t log_level) {
  embb_log_set_log_level(log_level);
}

void Log::SetLogFunction(
  void * context,
  embb_log_function_t func) {
  embb_log_set_log_function(context, func);
}

void Log::Write(
  char const * channel,
  embb_log_level_t log_level,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, log_level, message, argp);
  va_end(argp);
}

void Log::Trace(
  char const * channel,
  char const * message,
  ...) {
#if defined(EMBB_DEBUG)
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_TRACE, message, argp);
  va_end(argp);
#else
  EMBB_UNUSED(channel);
  EMBB_UNUSED(message);
#endif
}

void Log::Info(
  char const * channel,
  char const * message,
  ...) {
#if defined(EMBB_DEBUG)
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_INFO, message, argp);
  va_end(argp);
#else
  EMBB_UNUSED(channel);
  EMBB_UNUSED(message);
#endif
}

void Log::Warning(
  char const * channel,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_WARNING, message, argp);
  va_end(argp);
}

void Log::Error(
  char const * channel,
  char const * message,
  ...) {
  va_list argp;
  va_start(argp, message);
  embb_log_write_internal(channel, EMBB_LOG_LEVEL_ERROR, message, argp);
  va_end(argp);
}

} // namespace base
} // namespace embb
