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

#ifndef EMBB_BASE_C_LOG_H_
#define EMBB_BASE_C_LOG_H_

#include <embb/base/c/internal/config.h>
#include <stdarg.h>

/**
 * \defgroup C_LOG Logging
 * \ingroup C_BASE
 * Simple logging facilities.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log levels available for filtering the log.
 * \ingroup C_LOG
 */
typedef enum {
  EMBB_LOG_LEVEL_NONE,                 /**< show no log messages */
  EMBB_LOG_LEVEL_ERROR,                /**< show errors only */
  EMBB_LOG_LEVEL_WARNING,              /**< show warnings and errors */
  EMBB_LOG_LEVEL_INFO,                 /**< show info, warnings, and errors */
  EMBB_LOG_LEVEL_TRACE                 /**< show everything */
} embb_log_level_t;

/**
 * Logging function type.
 * This function is used by embb_log_write() to transfer a log message to its
 * desired destination. The user may specify a pointer to a context that
 * contains additional data (filter rules, file handles etc.) needed to put the
 * message where it should go. This pointer might be NULL if no additional data
 * is needed.
 * \ingroup C_LOG
 * \threadsafe
 */
typedef void(*embb_log_function_t)(void * context, char const * message);

/**
 * Default logging function.
 * Writes to the given file (context needs to be a FILE*).
 * \pre \c context is not NULL.
 * \ingroup C_LOG
 * \threadsafe
 */
void embb_log_write_file(
  void * context,                      /**< [in] User data, in this case a FILE*
                                            file handle. */
  char const * message                 /**< [in] The message to write */
  );

/**
 * Sets the global log level.
 * This determines what messages will be shown, messages with a more detailed
 * log level will be filtered out. The default log level is EMBB_LOG_LEVEL_NONE.
 * \ingroup C_LOG
 * \notthreadsafe
 */
void embb_log_set_log_level(
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
 * \ingroup C_LOG
 * \notthreadsafe
 */
void embb_log_set_log_function(
  void * context,                      /**< [in] User context to supply as the
                                            first parameter of the logging
                                            function*/
  embb_log_function_t func             /**< [in] The logging function */
  );

/**
 * Logs a message to the given channel with the specified log level.
 * If the log level is greater than the configured log level for the channel,
 * the message will be ignored.
 * \see embb_log_set_log_level, embb_log_set_log_function
 * \ingroup C_LOG
 * \threadsafe
 */
void embb_log_write(
  char const * channel,                /**< [in] User specified channel id
                                            for filtering the log later on.
                                            Might be NULL, channel identifier
                                            will be "global" in that case */
  embb_log_level_t log_level,          /**< [in] Log level to use */
  char const * message,                /**< [in] Message to convey, may use
                                            \c printf style formatting */
  ...                                  /**< Additional parameters determined by
                                            the format specifiers in
                                            \c message */
  );

#if defined(EMBB_DEBUG) || defined(DOXYGEN)
/**
 * Logs a message to the given channel with EMBB_LOG_LEVEL_TRACE using
 * embb_log_write().
 * In non-debug builds, this function does nothing.
 * \see embb_log_write
 * \ingroup C_LOG
 * \threadsafe
 */
void embb_log_trace(
  char const * channel,                /**< [in] User specified channel id */
  char const * message,                /**< [in] Message to convey, may use
                                            \c printf style formatting */
  ...                                  /**< Additional parameters determined by
                                            the format specifiers in
                                            \c message */
  );

/**
 * Logs a message to the given channel with EMBB_LOG_LEVEL_INFO using
 * embb_log_write().
 * In non-debug builds, this function does nothing.
 * \see embb_log_write
 * \ingroup C_LOG
 * \threadsafe
 */
void embb_log_info(
  char const * channel,                /**< [in] User specified channel id */
  char const * message,                /**< [in] Message to convey, may use
                                            \c printf style formatting */
  ...                                  /**< Additional parameters determined by
                                            the format specifiers in
                                            \c message */
  );
#else
#define embb_log_trace(...)
#define embb_log_info(...)
#endif

/**
 * Logs a message to the given channel with EMBB_LOG_LEVEL_WARNING using
 * embb_log_write().
 * \see embb_log_write
 * \ingroup C_LOG
 * \threadsafe
 */
void embb_log_warning(
  char const * channel,                /**< [in] User specified channel id */
  char const * message,                /**< [in] Message to convey, may use
                                            \c printf style formatting */
  ...                                  /**< Additional parameters determined by
                                            the format specifiers in
                                            \c message */
  );

/**
 * Logs a message to the given channel with EMBB_LOG_LEVEL_ERROR using
 * embb_log_write().
 * \see embb_log_write
 * \ingroup C_LOG
 * \threadsafe
 */
void embb_log_error(
  char const * channel,                /**< [in] User specified channel id */
  char const * message,                /**< [in] Message to convey, may use
                                            \c printf style formatting */
  ...                                  /**< Additional parameters determined by
                                            the format specifiers in
                                            \c message */
  );

/* function for internal use only */
void embb_log_write_internal(
  char const * channel,
  embb_log_level_t log_level,
  char const * message,
  va_list argp);

#ifdef __cplusplus
}
#endif

#endif /* EMBB_BASE_C_LOG_H_ */
