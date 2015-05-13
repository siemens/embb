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

#ifndef EMBB_BASE_SHARED_MUTEX_H_
#define EMBB_BASE_SHARED_MUTEX_H_

#include <embb/base/internal/platform.h>
#include <embb/base/exceptions.h>
#include <embb/base/mutex.h>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_SHARED_MUTEX Shared mutex
 *
 * Shared mutexes for thread synchronization
 *
 * Provides an abstraction from platform-specific shared mutex implementations.
 * Shared mutexes provide two levels of access: exclusive access - only one
 * thread can own the mutex; shared access - several threads can share ownership
 * of the same mutex. Shared mutexes facilitate the "single writer, multiple
 * readers" scenarios.
 *
 * \ingroup CPP_BASE
 */

/**
 * Shared mutex providing shared-read/exclusive-write access synchronization.
 *
 * Allows simultaneous shared access for concurrent readers, but only exclusive
 * access for writers. Cannot be locked recursively. Fairness and/or FIFO order
 * guarantees are platform specific and should be assumed non-existent.
 *
 * \see Mutex
 *
 * \ingroup CPP_BASE_SHARED_MUTEX
 */
class SharedMutex {
 public:
  /**
   * Creates the shared mutex object which is initially in an unlocked state.
   *
   * \memory Potentially allocates dynamic memory
   *
   * \notthreadsafe
   */
  SharedMutex();

  /**
   * Destroys internal representation.
   */
  ~SharedMutex();

  /**
   * Waits until the shared mutex can be locked for writing and locks it.
   *
   * \pre \c shared_mutex is initialized and not locked by the current thread.
   * \post \c shared_mutex is locked for writing.
   *
   * \threadsafe
   *
   * \see TryLock(), Unlock()
   */
  void Lock();

  /**
   * Tries to lock the shared mutex for writing and returns immediately.
   *
   * \pre \c shared_mutex is not locked by the current thread.
   * \post If successful, \c shared_mutex is locked for writing.
   *
   * \return \c true if shared mutex was locked for reading \n
   *         \c false otherwise
   *
   * \threadsafe
   *
   * \see Lock(), Unlock()
   */
  bool TryLock();

  /**
   * Unlocks the shared mutex locked for writing.
   *
   * \pre \c shared_mutex has been locked for writing by the current thread.
   * \post \c shared_mutex is unlocked.
   *
   * \threadsafe
   *
   * \see Lock(), TryLock()
   */
  void Unlock();

  /**
   * Waits until the shared mutex can be locked for reading and locks it.
   *
   * \pre The \c shared_mutex is not locked by the current thread.
   * \post The \c shared_mutex is locked for reading.
   *
   * \threadsafe
   *
   * \see TryLockShared(), UnlockShared()
   */
  void LockShared();

  /**
   * Tries to lock the shared mutex for reading and returns immediately.
   *
   * \pre \c shared_mutex is not locked by the current thread.
   * \post If successful, \c shared_mutex is locked for reading.
   *
   * \return \c true if shared mutex was locked for reading \n
   *         \c false otherwise
   *
   * \threadsafe
   *
   * \see LockShared(), UnlockShared()
   */
  bool TryLockShared();

  /**
   * Unlocks the shared mutex locked for reading.
   *
   * \pre \c shared_mutex has been locked for reading by the current thread.
   * \post \c shared_mutex is unlocked.
   *
   * \threadsafe
   *
   * \see LockShared(), TryLockShared()
   */
  void UnlockShared();

 private:
  /**
   * Disables copy construction and assignment.
   */
  SharedMutex(const SharedMutex&);
  SharedMutex& operator=(const SharedMutex&);

  /** Actual shared mutex implementation from base_c */
  internal::SharedMutexType shared_mutex_;
};

/**
 * Ownership wrapper for a shared mutex that works on the shared access level
 * (i.e. it locks a shared mutex in shared "readers" mode).
 *
 * For exclusive ownership wrapping, use UniqueLock<SharedMutex>.
 *
 * \notthreadsafe
 *
 * \tparam SharedMutex Type of the shared mutex object being wrapped
 *
 * \see SharedMutex, UniqueLock
 *
 * \ingroup CPP_BASE_MUTEX
 */
template<typename SharedMutex = embb::base::SharedMutex>
class SharedLock {
 public:
  /**
   * Creates a lock without assigned shared mutex.
   *
   * A shared mutex can be assigned to the lock using the method Swap().
   */
  SharedLock();

  /**
   * Creates a lock from an unlocked shared mutex and locks it for reading.
   *
   * \pre \c shared_mutex is unlocked
   * \post \c shared_mutex is locked for reading
   *
   * \param[IN] shared_mutex Shared mutex to be managed
   */
  explicit SharedLock(SharedMutex& shared_mutex);

  /**
   * Creates a lock from an unlocked shared mutex without locking it.
   *
   * \pre \c shared_mutex is unlocked
   * \post \c shared_mutex is unlocked
   *
   * \param[IN] shared_mutex Shared mutex to be managed
   * \param[IN] tag Tag to select correct constructor
   */
  SharedLock(SharedMutex& shared_mutex, DeferLockTag tag);

  /**
   * Creates a lock from an unlocked shared mutex and tries to lock it
   * for reading.
   *
   * \pre \c shared_mutex is unlocked
   * \post If successful, \c shared_mutex is locked for reading
   *
   * \param[IN] shared_mutex Shared mutex to be managed
   * \param[IN] tag Tag to select correct constructor
   */
  SharedLock(SharedMutex& shared_mutex, TryLockTag tag);

  /**
   * Creates a lock from an already locked shared mutex.
   *
   * \pre \c shared_mutex is locked for reading
   * \post \c shared_mutex is locked for reading
   *
   * \param[IN] shared_mutex Shared mutex to be managed
   * \param[IN] tag Tag to select correct constructor
   */
  SharedLock(SharedMutex& shared_mutex, AdoptLockTag tag);

  /**
   * Unlocks the shared mutex if owned.
   */
  ~SharedLock();

  /**
   * Waits until the shared mutex can be locked for reading and locks it.
   *
   * \throws ErrorException, if no shared mutex is set or it is already locked
   */
  void LockShared();

  /**
   * Tries to lock the shared mutex for reading and returns immediately.
   *
   * \return \c true if shared mutex was locked for reading \n
   *         \c false otherwise
   *
   * \throws ErrorException, if no shared mutex is set or it is already locked
   */
  bool TryLockShared();

  /**
   * Unlocks the shared mutex locked for reading.
   *
   * \throws ErrorException, if no shared mutex is set or it is not yet locked
   */
  void UnlockShared();

  /**
   * Exchange shared mutex ownership with another shared lock
   *
   * \param other Shared lock to exchange ownership with
   */
  void Swap(SharedLock& other);

  /**
   * Gives up ownership of the shared mutex and returns a pointer to it.
   *
   * \return A pointer to the owned shared mutex. (If no shared mutex was
   *         owned, returns NULL).
   */
  SharedMutex* Release();

  /**
   * Checks whether the shared mutex is owned and locked.
   *
   * \return \c true if shared mutex is locked, otherwise \c false.
   */
  bool OwnsLock() const;

 private:
  /**
   * Disable copy construction and assignment.
   */
  SharedLock(const SharedLock&);
  SharedLock& operator=(const SharedLock&);

  /** Pointer to the owned shared mutex */
  SharedMutex* shared_mutex_;
  /** Locked flag (is true if and only if the owned shared mutex is locked) */
  bool         locked_;
};

} // namespace base
} // namespace embb

#include <embb/base/internal/shared_mutex-inl.h>

#endif // EMBB_BASE_SHARED_MUTEX_H_
