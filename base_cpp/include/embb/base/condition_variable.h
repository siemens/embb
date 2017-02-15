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

#ifndef EMBB_BASE_CONDITION_VARIABLE_H_
#define EMBB_BASE_CONDITION_VARIABLE_H_

#include <embb/base/internal/platform.h>
#include <embb/base/mutex.h>
#include <embb/base/time.h>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_CONDITION Condition Variable
 *
 * Condition variables for thread synchronization.
 *
 * \ingroup CPP_BASE
 */

/**
 * Represents a condition variable for thread synchronization.
 *
 * Provides an abstraction from platform-specific condition variable
 * implementations. Condition variables can be waited for with timeouts using
 * relative durations and absolute time points.
 *
 * This class is essentially a wrapper for the underlying C implementation.
 *
 * \ingroup CPP_BASE_CONDITION
 */
class ConditionVariable {
 public:
  /**
   * Creates a condition variable.
   *
   * \throws embb::base::ErrorException if initialization failed
   *
   * \memory Potentially allocates dynamic memory
   *
   * \notthreadsafe
   */
  ConditionVariable();

  /**
   * Wakes up one waiting thread.
   *
   * \throws embb::base::ErrorException if notification failed
   *
   * \threadsafe
   *
   * \see NotifyAll(), Wait()
   */
  void NotifyOne();

  /**
   * Wakes up all waiting threads.
   *
   * \throws embb::base::ErrorException if notification failed
   *
   * \threadsafe
   *
   * \see NotifyOne(), Wait()
   */
  void NotifyAll();

  /**
   * Releases the lock and waits until the thread is woken up.
   *
   * \pre The lock has been acquired by the calling thread.
   * \post The lock has been re-acquired by the calling thread.
   *
   * \throws embb::base::ErrorException if waiting failed
   *
   * \threadsafe
   *
   * \see NotifyOne(), NotifyAll()
   *
   * \note It is strongly recommended checking the condition in a loop in order
   *       to deal with spurious wakeups and situations where another thread has
   *       locked the mutex between notification and wakeup.
   */
  void Wait(
    UniqueLock<Mutex>& lock
    /**< [IN,OUT] Lock to be released and re-acquired */
    );

  /**
   * Releases the lock and waits until the thread is woken up or the specified
   * time point has passed.
   *
   * \pre The lock has been acquired by the calling thread.
   * \post The lock has been re-acquired by the calling thread.
   *
   * \return \c true if the thread was woken up before the specified time point
   *         has passed, otherwise \c false.
   *
   * \throws embb::base::ErrorException if an error occurred
   *
   * \threadsafe
   *
   * \note It is strongly recommended checking the condition in a loop in order
   *       to deal with spurious wakeups and situations where another thread has
   *       locked the mutex between notification and wakeup.
   */
  bool WaitUntil(
    UniqueLock<Mutex>& lock,
    /**< [IN,OUT] Lock to be released and re-acquired */
    const Time& time
    /**< [IN] Absolute time point until which the thread maximally waits */
    );

  /**
   * Releases the lock and waits until the thread is woken up or the specified
   * duration has passed.
   *
   * \pre The lock has been acquired by the calling thread.
   * \post The lock has been re-acquired by the calling thread.
   *
   * \return \c true if the thread was woken up before the specified duration
   *         has passed, otherwise \c false.
   *
   * \throws embb::base::ErrorException if an error occurred
   *
   * \threadsafe
   *
   * \tparam Tick Type of tick of the duration. See Duration.
   *
   * \note It is strongly recommended checking the condition in a loop in order
   *       to deal with spurious wakeups and situations where another thread has
   *       locked the mutex between notification and wakeup.
   */
  template<typename Tick>
  bool WaitFor(
    UniqueLock<Mutex>& lock,
    /**< [IN,OUT] Lock to be released and re-acquired */
    const Duration<Tick>& duration
    /**< [IN] Relative time duration the thread maximally waits */
    );

 private:
  /**
   * Disables copying and assigment.
   */
  ConditionVariable(const ConditionVariable&);
  ConditionVariable& operator=(const ConditionVariable&);

  /**
   * Holds actual condition variable.
   */
  internal::ConditionVariableType condition_var_;
};

} // namespace base
} // namespace embb

#include <embb/base/internal/condition_variable-inl.h>

#endif // EMBB_BASE_CONDITION_VARIABLE_H_
