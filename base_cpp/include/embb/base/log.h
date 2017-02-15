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

#ifndef EMBB_BASE_LOG_H_
#define EMBB_BASE_LOG_H_

#include <embb/base/c/log.h>

/**
 * \defgroup CPP_LOG Logging
 * \ingroup CPP_BASE
 * Simple logging facilities.
 */

namespace embb {
namespace base {

/**
 * Simple logging facilities.
 *
 * \ingroup CPP_LOG
 */
class Log {
 private:
  // do not allow construction
  Log();

 public:
  /**
   * Sets the global log level.
   * This determines what messages will be shown, messages with a more detailed
   * log level will be filtered out. The default log level is EMBB_LOG_LEVEL_NONE.
   * \notthreadsafe
   */ 
  static void SetLogLevel(
    embb_log_level_t log_level           /**< [in] Log level to use for
                                              filtering */
  );

  /**
   * Sets the global logging function.
   * The logging function implements the mechanism for transferring log messages
   * to their destination. \c context is a pointer to data the user needs in the
   * function to determine where the messages should go (may be NULL if no
   * additional data is needed). The default logging function is
   * embb_log_write_file() with context set to \c stdout.
   * \see embb_log_function_t
   * \notthreadsafe
   */
  static void SetLogFunction(
    void * context,                      /**< [in] User context to supply as the
                                              first parameter of the logging
                                              function*/
    embb_log_function_t func             /**< [in] The logging function */
  );

  /**
   * Logs a message to the given channel with the specified log level.
   * If the log level is greater than the configured log level for the channel,
   * the message will be ignored.
   * \see  embb::base::Log::SetLogLevel, embb::base::Log::SetLogFunction
   * \threadsafe
   */
  static void Write(
    char const * channel,                /**< [in] User specified channel id
                                              for filtering the log later on.
                                              Might be NULL, channel identifier
                                              will be "global" in that case */
    embb_log_level_t log_level,          /**< [in] Log level to use */
    char const * message,                /**< [in] Message to convey, may use
                                              \c printf style formatting */
    ...                                  /**< Additional parameters determined
                                              by the format specifiers in
                                              \c message */
  );

  /**
   * Logs a message to the given channel with EMBB_LOG_LEVEL_TRACE.
   * In non-debug builds, this function does nothing.
   * \see embb::base::Log::Write
   * \threadsafe
   */
  static void Trace(
    char const * channel,                /**< [in] User specified channel id */
    char const * message,                /**< [in] Message to convey, may use
                                              \c printf style formatting */
    ...                                  /**< Additional parameters determined
                                              by the format specifiers in
                                              \c message */
  );

  /**
   * Logs a message to the given channel with EMBB_LOG_LEVEL_INFO.
   * In non-debug builds, this function does nothing.
   * \see embb::base::Log::Write
   * \threadsafe
   */
  static void Info(
    char const * channel,                /**< [in] User specified channel id */
    char const * message,                /**< [in] Message to convey, may use
                                              \c printf style formatting */
    ...                                  /**< Additional parameters determined
                                              by the format specifiers in
                                              \c message */
  );

  /**
   * Logs a message to the given channel with EMBB_LOG_LEVEL_WARNING.
   * \see embb::base::Log::Write
   * \threadsafe
   */
  static void Warning(
    char const * channel,                /**< [in] User specified channel id */
    char const * message,                /**< [in] Message to convey, may use
                                              \c printf style formatting */
    ...                                  /**< Additional parameters determined
                                              by the format specifiers in
                                              \c message */
  );

  /**
   * Logs a message to the given channel with EMBB_LOG_LEVEL_ERROR.
   * \see embb::base::Log::Write
   * \threadsafe
   */
  static void Error(
    char const * channel,                /**< [in] User specified channel id */
    char const * message,                /**< [in] Message to convey, may use
                                              \c printf style formatting */
    ...                                  /**< Additional parameters determined
                                              by the format specifiers in
                                              \c message */
  );
};

} // namespace base
} // namespace embb

#endif // EMBB_BASE_LOG_H_
