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

#ifndef EMBB_BASE_THREAD_H_
#define EMBB_BASE_THREAD_H_

#include <embb/base/internal/platform.h>
#include <embb/base/internal/thread_closures.h>
#include <embb/base/mutex.h>
#include <embb/base/core_set.h>
#include <embb/base/c/thread.h>
#include <ostream>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_THREAD Thread
 *
 * Threads supporting thread-to-core affinities.
 *
 * \ingroup CPP_BASE
 */

/**
 * Represents a thread of execution.
 *
 * Provides an abstraction from platform-specific threading implementations to
 * create, manage, and join threads of execution. Support for thread-to-core
 * affinities is given on thread creation by using the core set functionality.
 *
 * This class is essentially a wrapper for the underlying C implementation.
 *
 * \ingroup CPP_BASE_THREAD
 */
class Thread {
 public:
  /**
   * Unique %ID of a thread that can be compared with other IDs.
   */
  class ID {
   public:
    /**
     * Constructs an empty (invalid) thread %ID.
     */
    ID() : id_() {}

   private:
    /**
     * Constructs an %ID from an IDType instance. Is done by the thread.
     */
    explicit ID(internal::IDType id) : id_(id) {}

    /**
     * Holds the actual %ID representation.
     */
    internal::IDType id_;

    /**
     * A thread needs to set its ID on start.
     */
    friend class Thread;

    /**
     * The streaming operator needs to access the internal %ID representation.
     */
    template<class CharT, class Traits>
    friend
    std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& os, Thread::ID id);

    /**
     * Comparison operators need to access the internal ID representation.
     */
    friend bool operator==(Thread::ID lhs, Thread::ID rhs);
    friend bool operator!=(Thread::ID lhs, Thread::ID rhs);
  };

  /**
   * Returns the maximum number of threads handled by EMB<sup>2</sup>.
   *
   * See embb_thread_get_max_count() for a description of the semantics.
   *
   * \return Maximum number of threads
   *
   * \lockfree
   * \see SetThreadsMaxCount()
   */
  static unsigned int GetThreadsMaxCount();

  /**
   * Sets the maximum number of threads handled by EMB<sup>2</sup>.
   *
   * \notthreadsafe
   * \see GetThreadsMaxCount()
   */
  static void SetThreadsMaxCount(
    unsigned int max_count
    /**< [IN] Maximum number of threads */
    );

  /**
   * Returns the %ID of the current thread.
   *
   * The %ID is only valid within the calling thread.
   *
   * \return %ID of the calling thread
   *
   * \threadsafe
   */
  static ID CurrentGetID();

  /**
   * Reschedule the current thread for later execution.
   *
   * This is only a request, the realization depends on the implementation and
   * the scheduler employed by the operating system.
   *
   * \threadsafe
   */
  static void CurrentYield();

  /**
   * Creates and runs a thread with zero-argument start function.
   *
   * \note If the function is passed as a temporary object when creating a
   * thread, this might be interpreted as a function declaration ("most vexing
   * parse"). C++11 resolves this by using curly braces for initialization.
   *
   * \throws NoMemoryException if not enough memory is available
   * \throws ErrorException in case of another error
   * \memory A small constant amount of memory to store the function. This
   *         memory is freed the thread is joined.
   * \notthreadsafe
   * \tparam Function Function object type
   */
  template<typename Function>
  explicit Thread(
    Function function
    /**< [IN] Copyable function object, callable without arguments */
    );

  /**
   * Creates and runs a thread with zero-argument start function.
   *
   * \note If the function is passed as a temporary object when creating a
   * thread, this might be interpreted as a function declaration ("most vexing
   * parse"). C++11 resolves this by using curly braces for initialization.
   *
   * \throws NoMemoryException if not enough memory is available
   * \throws ErrorException in case of another error
   * \memory A small constant amount of memory to store the function. This
   *         memory is freed the thread is joined.
   * \notthreadsafe
   * \tparam Function Function object type
   */
  template<typename Function>
  Thread(
    CoreSet& core_set,
    /**< [IN] Set of cores on which the thread shall be executed. */
    Function function
    /**< [IN] Copyable function object, callable without arguments */
    );

  /**
   * Creates and runs a thread with zero-argument start function.
   *
   * \note If the function is passed as a temporary object when creating a
   * thread, this might be interpreted as a function declaration ("most vexing
   * parse"). C++11 resolves this by using curly braces for initialization.
   *
   * \throws NoMemoryException if not enough memory is available
   * \throws ErrorException in case of another error
   * \memory A small constant amount of memory to store the function. This
   *         memory is freed the thread is joined.
   * \notthreadsafe
   * \tparam Function Function object type
   */
  template<typename Function>
  Thread(
    CoreSet& core_set,
    /**< [IN] Set of cores on which the thread shall be executed. */
    embb_thread_priority_t priority,
    /**< [IN] Priority of the new thread. */
    Function function
    /**< [IN] Copyable function object, callable without arguments */
    );

  /**
   * Creates and runs a thread with one-argument thread start function.
   *
   * \note If the function is passed as a temporary object when creating a
   * thread, this might be interpreted as a function declaration ("most vexing
   * parse"). C++11 resolves this by using curly braces for initialization.
   *
   * \throws NoMemoryException if not enough memory is available
   * \throws ErrorException in case of another error
   * \memory A small constant amount of memory to store the function. This
   *         memory is freed the thread is joined.
   * \notthreadsafe
   * \tparam Function Function object type
   * \tparam Argument Type of argument
   */
  template<typename Function, typename Arg>
  Thread(
    Function function,
    /**< [IN] Copyable function object, callable with one argument */
    Arg arg
    /**< [IN] Argument for function (must be copyable) */
    );

  /**
   * Creates and runs a thread with two-argument thread start function.
   *
   * \note If the function is passed as a temporary object when creating a
   * thread, this might be interpreted as a function declaration ("most vexing
   * parse"). C++11 resolves this by using curly braces for initialization.
   *
   * \throws NoMemoryException if not enough memory is available
   * \throws ErrorException in case of another error
   * \memory A small constant amount of memory to store the function. This
   *         memory is freed the thread is joined.
   * \notthreadsafe
   * \tparam Function Function object type
   * \tparam Arg1 Type of first argument
   * \tparam Arg2 Type of second argument
   */
  template<typename Function, typename Arg1, typename Arg2>
  Thread(
    Function function,
    /**< [IN] Copyable function object, callable with two arguments */
    Arg1 arg1,
    /**< [IN] First argument for function (must be copyable) */
    Arg2 arg2
    /**< [IN] Second argument for function (must be copyable) */
    );

  /**
   * Waits until the thread has finished execution.
   *
   * \pre The thread has been created but not yet been joined.
   * \post The thread has finished execution and dynamic memory allocated during
   *       creation has been freed.
   * \notthreadsafe
   */
  void Join();

  /**
   * Returns the thread %ID.
   *
   * \return %ID of the thread
   *
   * \threadsafe
   */
  ID GetID();

 private:
  /**
   * Performs error checks and frees resources from thread constructor.
   *
   * \tparam ThreadClosure Type of thread closure
   */
  template<typename ThreadClosure>
  void CheckThreadCreationErrors(
    int result,
    /**< [IN] Result value of creating thread with C API */
    ThreadClosure* closure
    /**< [IN] Closure used when creating thread */
    );

  /**
   * Disables copying and assignment.
   */
  Thread(const Thread&);
  Thread& operator=(const Thread&);

  /**
   * Holds native implementation thread handle.
   */
  internal::ThreadType rep_;
};

/**
 * Compares two thread IDs for equality.
 *
 * \return \c true if thread IDs are equivalent, otherwise \c false
 *
 * \ingroup CPP_BASE_THREAD
 */
bool operator==(
  Thread::ID lhs,
  /**< [IN] Left-hand side of equality sign */
  Thread::ID rhs
  /**< [IN] Right-hand side of equality sign */
  );

/**
 * Compares two thread IDs for inequality.
 *
 * \return \c true if thread IDs are not equivalent, otherwise \c false
 *
 * \ingroup CPP_BASE_THREAD
 */
bool operator!=(
    Thread::ID lhs,
    /**< [IN] Left-hand side of inequality sign */
    Thread::ID rhs
    /**< [IN] Left-hand side of inequality sign */
    );

/**
 * Writes thread %ID to stream.
 *
 * \return Reference to the stream
 *
 * \ingroup CPP_BASE_THREAD
 */
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
    operator<<(
      std::basic_ostream<CharT, Traits>& os,
      /**< [IN/OUT] Stream to which thread %ID is written */
      Thread::ID id
      /**< [IN] %Thread %ID to be written */
      );

} // namespace base
} // namespace embb

#include <embb/base/internal/thread-inl.h>

#endif // EMBB_BASE_THREAD_H_
