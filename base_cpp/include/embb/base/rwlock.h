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

#ifndef EMBB_BASE_RWLOCK_H_
#define EMBB_BASE_RWLOCK_H_

#include <embb/base/internal/platform.h>
#include <embb/base/exceptions.h>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_RWLOCK Readers-Writers Lock
 *
 * Shared-read/exclusive-write lock for thread synchronization.
 *
 * \ingroup CPP_BASE
 */

/**
 * Shared-read/exclusive-write (readers-writers) lock.
 *
 * Allows simultaneous shared access for concurrent readers, but only exclusive
 * access for writers. Cannot be locked recursively. Fairness and/or FIFO order
 * guaranties are platform specific and should be assumed non-existent.
 *
 * \ingroup CPP_BASE_RWLOCK
 */
class RWLock {
 public:
  /**
   * Creates an rwlock which is in unlocked state.
   *
   * \memory Potentially allocates dynamic memory
   * \notthreadsafe
   */
  RWLock();

  /**
   * Destroys internal representation.
   */
  ~RWLock();

  /**
   * Waits until the rwlock can be locked for reading and locks it.
   *
   * \pre The \c rwlock is not locked by the current thread.
   * \post The \c rwlock is locked for reading.
   *
   * \threadsafe
   *
   * \see TryLockRead(), UnlockRead()
   */
  void LockRead();

  /**
   * Waits until the rwlock can be locked for writing and locks it.
   *
   * \pre \c rwlock is initialized and not locked by the current thread.
   * \post \c rwlock is locked for writing.
   *
   * \threadsafe
   *
   * \see TryLockWrite(), UnlockWrite()
   */
  void LockWrite();

  /**
   * Tries to lock the rwlock for reading and returns immediately.
   *
   * \pre \c rwlock is not locked by the current thread.
   * \post If successful, \c rwlock is locked for reading.
   *
   * \return \c true if rwlock was locked for reading \n
   *         \c false otherwise
   *
   * \threadsafe
   *
   * \see LockRead(), UnlockRead()
   */
  bool TryLockRead();

  /**
   * Tries to lock the rwlock for writing and returns immediately.
   *
   * \pre \c rwlock is not locked by the current thread.
   * \post If successful, \c rwlock is locked for writing.
   *
   * \return \c true if rwlock was locked for reading \n
   *         \c false otherwise
   *
   * \threadsafe
   *
   * \see LockWrite(), UnlockWrite()
   */
  bool TryLockWrite();

  /**
   * Unlocks an rwlock locked for reading.
   *
   * \pre \c rwlock has been locked for reading by the current thread.
   * \post \c rwlock is unlocked.
   *
   * \threadsafe
   *
   * \see LockRead(), TryLockRead()
   */
  void UnlockRead();

  /**
   * Unlocks an rwlock locked for writing.
   *
   * \pre \c rwlock has been locked for writing by the current thread.
   * \post \c rwlock is unlocked.
   *
   * \threadsafe
   *
   * \see LockWrite(), TryLockWrite()
   */
  void UnlockWrite();


  /**
   * Adaptor class for the readers lock that can be used with "LockGuard" and/or
   * "UniqueLock" ownership wrappers.
   *
   * \see LockGuard, UniqueLock
   */
  class ReaderLock {
   public:
    ReaderLock(RWLock& rwlock);
    void Lock();
    bool TryLock();
    void Unlock();
   private:
    ReaderLock(const ReaderLock&);
    ReaderLock& operator=(const ReaderLock&);
    RWLock& rwlock_;
  };

  /**
   * Adaptor class for the writers lock that can be used with "LockGuard" and/or
   * "UniqueLock" ownership wrappers.
   *
   * \see LockGuard, UniqueLock
   */
  class WriterLock {
   public:
    WriterLock(RWLock& rwlock);
    void Lock();
    bool TryLock();
    void Unlock();
   private:
    WriterLock(const WriterLock&);
    WriterLock& operator=(const WriterLock&);
    RWLock& rwlock_;
  };

  /**
   * Returns an adaptor for the readers lock to be used with "LockGuard" and/or
   * "UniqueLock" ownership wrappers.
   */
  ReaderLock& GetReaderLock();

  /**
   * Returns an adaptor for the writes lock to be used with "LockGuard" and/or
   * "UniqueLock" ownership wrappers.
   */
  WriterLock& GetWriterLock();

 private:
  /**
   * Disables copy construction and assignment.
   */
  RWLock(const RWLock&);
  RWLock& operator=(const RWLock&);

  internal::RWLockType rwlock_; /**< Actual rwlock implementation from base_c */
  ReaderLock reader_lock_;      /**< Lock adaptor for readers */
  WriterLock writer_lock_;      /**< Lock adaptor for writers */
};

} // namespace base
} // namespace embb

#endif // EMBB_BASE_RWLOCK_H_
