/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_SPINLOCK_H_
#define EMBB_BASE_SPINLOCK_H_

#include <embb/base/internal/platform.h>
#include <embb/base/exceptions.h>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_SPINLOCK Spinlock
 *
 * Spinlock for thread synchronization.
 *
 * \ingroup CPP_BASE
 */

/**
 * Spinlock
 *
 * \ingroup CPP_BASE_SPINLOCK
 */
class Spinlock {
 public:
  /**
   * Creates a spinlock which is in unlocked state.
   *
   * \notthreadsafe
   */
  Spinlock();

  /**
   * Waits until the spinlock can be locked and locks it.
   *
   * \pre The spinlock is not locked by the current thread.
   * \post The spinlock is locked
   * \threadsafe
   * \see TryLock(), Unlock()
   */
  void Lock();

  /**
   * Tries to lock the spinlock for \c number_spins times and returns.
   *
   * \pre The spinlock is not locked by the current thread.
   * \post If successful, the spinlock is locked.
   * \return \c true if the spinlock could be locked, otherwise \c false.
   * \threadsafe
   * \see Lock(), Unlock()
   */
  bool TryLock(unsigned int number_spins = 0);

  /**
   * Unlocks the spinlock.
   *
   * \pre The spinlock is locked by the current thread
   * \post The spinlock is unlocked
   * \threadsafe
   * \see Lock(), TryLock()
   */
  void Unlock();

 private:
  /**
   * Disables copy construction and assignment.
   */
  Spinlock(const Spinlock&);
  Spinlock& operator=(const Spinlock&);

};

} // namespace base
} // namespace embb

#endif // EMBB_BASE_SPINLOCK_H_
