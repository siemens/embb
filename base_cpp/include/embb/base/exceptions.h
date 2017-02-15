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

#ifndef EMBB_BASE_EXCEPTIONS_H_
#define EMBB_BASE_EXCEPTIONS_H_

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
// Disable warning that exceptions are disabled but try/catch is used.
#pragma warning(disable : 4530)
#endif // EMBB_PLATFORM_COMPILER_MSVC

#include <string>
#include <exception>

#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif

#include <embb/base/internal/cmake_config.h>
#include <embb/base/c/errors.h>

/**
 * \defgroup CPP_BASE_EXCEPTIONS Exception
 *
 * Exception types.
 *
 * If exceptions are disabled, i.e., if the library was built without support
 * for exceptions, no exceptions will be thrown. Instead, an error message is
 * printed to \c stderr and the program exits with the code representing the
 * exception.
 *
 * \ingroup CPP_BASE
 */

/**
 * Macros to be used within EMBB when throwing and catching exceptions.
 *
 * Example:
 * - Throwing an exception:
 *   EMBB_THROW(NoMemoryException, "Could not create thread.");
 *   --> If exceptions are disabled, this will write an error to stderr,
 *       containing the exception's message, and exit the program with the code
 *       of the exception.
 * - Try/catch block:
 *   EMBB_TRY{ ... things to try ... }
 *   EMBB_CATCH(ExceptionXYZ& e){ ... things to catch ... }
 *   --> If exceptions are disabled, this will execute the try block without and
 *       replace the catch() statement by "if (false)", such that
 *       the catch block will be compiled but never executed.
 */
#ifdef EMBB_USE_EXCEPTIONS
#define EMBB_TRY try
#define EMBB_THROW(Type, Message) throw Type(Message);
#define EMBB_CATCH(Statement) catch(Statement)
#else /* EMBB_USE_EXCEPTIONS */
#include <stdio.h>
#include <stdlib.h>
#include <embb/base/c/internal/unused.h>
#define EMBB_TRY
/**
 * Concatenates the inputs.
 */
#define EMBB_CATCH_VAR_CAT2(X, Y) X##Y
/**
 * Is a necessary intermediate macro to create EMBB_CATCH_VAR.
 */
#define EMBB_CATCH_VAR_CAT1(X, Y) EMBB_CATCH_VAR_CAT2(X, Y)
/**
 * Defines a unique variable name for each line of code.
 *
 * The line number is concatenated to a base name.
 */
#define EMBB_CATCH_VAR EMBB_CATCH_VAR_CAT1(embb_catch_var_, __LINE__)
/**
 * Replaces catch(xyz) by an if-statement that is always false.
 *
 * To avoid a compiler warning that the condition is constant, a variable is
 * used instead of "false". A unique name for every catch-statement is
 * necessary, which is achieved by macro EMBB_CATCH_VAR and helper macros. To
 * avoid the unused variable warning, in addition the EMBB_UNUSED macro is
 * used.
 */
#define EMBB_CATCH(Statement) \
        int EMBB_CATCH_VAR = false; \
        EMBB_UNUSED(EMBB_CATCH_VAR); \
        if (EMBB_CATCH_VAR)

/**
 * Replaces a throw by an error message and program exit.
 */
#define EMBB_THROW(Type, Message) \
  { \
    Type e(Message); \
    fprintf(stderr, \
      "Exit program due to (not thrown) " #Type ": %s\n", e.what()); \
    exit(e.Code()); \
  }
#endif /* else EMBB_USE_EXCEPTIONS */

namespace embb {
namespace base {

/**
 * Abstract base class for exceptions.
 *
 * \ingroup CPP_BASE_EXCEPTIONS
 */
class Exception : public std::exception {
 public:
  /**
   * Constructs an exception with a custom message.
   */
  explicit Exception(
    const char* message
    /**< [IN] Error message */
    ) : message_(message) {}

  /**
   * Destructs the exception.
   */
  virtual ~Exception() throw() {}

  /**
   * Constructs an exception by copying from an existing one.
   */
  Exception(
    const Exception& e
    /**< [IN] %Exception to be copied */
    ) : message_(e.message_) {}

  /**
   * Assigns an existing exception.
   *
   * \return Reference to \c *this
   */
  Exception& operator=(
    const Exception& e
    /**< [IN] %Exception to assign */
    ) {
    message_ = e.message_;
    return *this;
  }

  /**
   * Returns the error message.
   *
   * \return Pointer to error message
   */
  virtual const char* What() const throw() {
    return message_;
  }

  /**
   * Returns an integer code representing the exception.
   *
   * \return %Exception code
   */
  virtual int Code() const = 0;

 private:
  /**
   * Holds error message
   */
  const char* message_;
};

/**
 * Indicates lack of memory necessary to allocate a resource.
 *
 * \ingroup CPP_BASE_EXCEPTIONS
 */
class NoMemoryException : public Exception {
 public:
   /**
    * Constructs an exception with the specified message
    */
  explicit NoMemoryException(
    const char* message
    /**< [IN] Error message */
    ) : Exception(message) {}
  virtual int Code() const { return EMBB_NOMEM; }
};

/**
 * Indicates business (unavailability) of a required resource.
 *
 * \ingroup CPP_BASE_EXCEPTIONS
 */
class ResourceBusyException : public Exception {
 public:
  /**
   * Constructs an exception with the specified message
   */
  explicit ResourceBusyException(
    const char* message
    /**< [IN] Error message */
    ) : Exception(message) {}
  virtual int Code() const { return EMBB_BUSY; }
};

/**
 * Indicates a numeric underflow.
 *
 * \ingroup CPP_BASE_EXCEPTIONS
 */
class UnderflowException : public Exception {
 public:
  /**
   * Constructs an exception with the specified message
   */
  explicit UnderflowException(
    const char* message
    /**< [IN] Error message */
    ) : Exception(message) {}
  virtual int Code() const { return EMBB_UNDERFLOW; }
};

/**
 * Indicates a numeric overflow.
 *
 * \ingroup CPP_BASE_EXCEPTIONS
 */
class OverflowException : public Exception {
 public:
  /**
   * Constructs an exception with the specified message
   */
  explicit OverflowException(
    const char* message
    /**< [IN] Error message */
    ) : Exception(message) {}
  virtual int Code() const { return EMBB_OVERFLOW; }
};

/**
 * Indicates a general error.
 *
 * \ingroup CPP_BASE_EXCEPTIONS
 */
class ErrorException : public Exception {
 public:
  /**
   * Constructs an exception with the specified message
   */
  explicit ErrorException(
    const char* message
    /**< [IN] Error message */
    ) : Exception(message) {}
  virtual int Code() const { return EMBB_ERROR; }
};

} // namespace base
} // namespace embb

#endif  // EMBB_BASE_EXCEPTIONS_H_
