/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_MUTEX_H_
#define EMBB_BASE_MUTEX_H_

#include <embb/base/internal/platform.h>
#include <embb/base/exceptions.h>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_MUTEX Mutex and Lock
 *
 * Mutexes and locks for thread synchronization.
 *
 * \ingroup CPP_BASE
 */

/**
 * Forward declaration for friending.
 */
class ConditionVariable;

namespace internal {

/**
 * Provides main functionality for mutexes.
 */
class MutexBase {
 public:
  /**
   * Creates internal representation.
   *
   * \notthreadsafe
   */
  explicit MutexBase(
    int mutex_type
    /**< [IN] Mutex type as used in embb_mutex_init(). */
    );

  /**
   * Destroys internal representation.
   */
  virtual ~MutexBase() = 0;

  /**
   * Waits until the mutex can be locked and locks it.
   *
   * \pre The mutex is not locked by the current thread or is of type
   *      RecursiveMutex.
   * \post The mutex is locked
   * \threadsafe
   * \see TryLock(), Unlock()
   */
  void Lock();

  /**
   * Tries to lock the mutex and returns immediately.
   *
   * \post If successful, the mutex is locked
   * \return \c true if mutex could be locked, otherwise \c false
   * \threadsafe
   * \see Lock(), Unlock()
   */
  bool TryLock();

  /**
   * Unlocks a locked mutex.
   *
   * \pre The mutex is locked by the current thread
   * \post The mutex is unlocked if the number of unlock operations has reached
   *       the number of lock operations
   * \threadsafe
   * \see Lock(), TryLock()
   */
  void Unlock();

 private:
  /**
   * Holds the actual mutex.
   */
  internal::MutexType mutex_;

  /**
   * For access to native implementation type.
   */
  friend class embb::base::ConditionVariable;
};

} // namespace internal

/**
 * Non-recursive, exclusive mutex.
 *
 * Mutexes of this type cannot be locked recursively, that is, multiple times
 * by the same thread with unlocking it in between. Moreover, it cannot be
 * copied or assigned.
 *
 * \see RecursiveMutex
 * \ingroup CPP_BASE_MUTEX
 */
class Mutex : public internal::MutexBase {
 public:
  /**
   * Creates a mutex which is in unlocked state.
   *
   * \memory Potentially allocates dynamic memory
   * \notthreadsafe
   */
  Mutex();

#ifdef DOXYGEN

  /**
   * Waits until the mutex can be locked and locks it.
   *
   * \pre The mutex is not locked by the current thread.
   * \post The mutex is locked
   * \threadsafe
   * \see TryLock(), Unlock()
   */
  void Lock();

  /**
   * Tries to lock the mutex and returns immediately.
   *
   * \pre The mutex is not locked by the current thread.
   * \post If successful, the mutex is locked.
   * \return \c true if the mutex could be locked, otherwise \c false.
   * \threadsafe
   * \see Lock(), Unlock()
   */
  bool TryLock();

  /**
   * Unlocks the mutex.
   *
   * \pre The mutex is locked by the current thread
   * \post The mutex is unlocked
   * \threadsafe
   * \see Lock(), TryLock()
   */
  void Unlock();

#endif // DOXYGEN

 private:
  /**
   * Disables copy construction and assignment.
   */
  Mutex(const Mutex&);
  Mutex& operator=(const Mutex&);

  /**
   * For access to native implementation type.
   */
  friend class ConditionVariable;
};


/**
 * Recursive, exclusive mutex.
 *
 * Mutexes of this type can be locked recursively, that is, multiple times by
 * the same thread without unlocking it in between. It is unlocked only, if the
 * number of unlock operations has reached the number of previous lock
 * operations by the same thread. It cannot be copied or assigned.
 *
 * \see Mutex
 * \ingroup CPP_BASE_MUTEX
 */
class RecursiveMutex : public internal::MutexBase {
 public:
  /**
   * Creates a mutex which is in unlocked state.
   *
   * \memory Potentially allocates dynamic memory
   * \notthreadsafe
   */
  RecursiveMutex();

#ifdef DOXYGEN

  /**
   * Waits until the mutex can be locked and locks it.
   *
   * \post The mutex is locked
   * \threadsafe
   * \see TryLock(), Unlock()
   */
  void Lock();

  /**
   * Tries to lock the mutex and returns immediately.
   *
   * \post If successful, the given mutex is locked.
   * \return \c true if the mutex could be locked, otherwise \c false.
   * \threadsafe
   * \see Lock(), Unlock()
   */
  bool TryLock();

  /**
   * Unlocks a locked mutex.
   *
   * \pre The mutex is locked by the current thread.
   * \post The mutex is unlocked if the number of unlock operations has reached
   *       the number of lock operations.
   * \threadsafe
   * \see Lock(), TryLock()
   */
  void Unlock();

#endif // DOXYGEN

 private:
  /**
   * Disables copy construction and assignment.
   */
  RecursiveMutex(const RecursiveMutex&);
  RecursiveMutex& operator=(const RecursiveMutex&);
};


/**
 * Scoped lock (according to the RAII principle) using a mutex.
 *
 * The mutex is locked on construction and unlocked on leaving the scope of the
 * lock.
 *
 * \tparam Mutex Used mutex type
 * \see Mutex, UniqueLock
 * \ingroup CPP_BASE_MUTEX
 */
template<typename Mutex = embb::base::Mutex>
class LockGuard {
 public:
  /**
   * Creates the lock and locks the mutex.
   *
   * \pre The given mutex is unlocked
   * \notthreadsafe
   */
  explicit LockGuard(
    Mutex& mutex
    /**< [IN] Mutex to be guarded */
    ) : mutex_(mutex) {
    mutex_.Lock();
  }

  /**
   * Unlocks the mutex.
   */
  ~LockGuard() {
    mutex_.Unlock();
  }

 private:
  /**
   * Holds reference to mutex realizing the lock.
   */
  Mutex& mutex_;

  /**
   * Disable copy construction and assignment.
   */
  LockGuard(const LockGuard<Mutex>&);
  LockGuard<Mutex>& operator=(const LockGuard<Mutex>&);
};

/**
 * \name UniqueLock Tag Variables
 * \{
 */

/**
 * Tag type for deferred UniqueLock construction.
 *
 * Use the defer_lock variable in constructor calls.
 */
struct DeferLockTag {};

/**
 * Tag variable for deferred UniqueLock construction.
 *
 * \ingroup CPP_BASE_MUTEX
 */
const DeferLockTag defer_lock = DeferLockTag();

/**
 * Tag type for try-lock UniqueLock construction.
 *
 * Use the try_lock variable in constructor calls.
 */
struct TryLockTag {};

/**
 * Tag variable for try-lock UniqueLock construction.
 *
 * \ingroup CPP_BASE_MUTEX
 */
const TryLockTag try_lock = TryLockTag();

/**
 * Tag type for adopt UniqueLock constructor.
 *
 * Use the adopt_lock variable in constructor calls.
 */
struct AdoptLockTag {};

/**
 * Tag variable for adopt UniqueLock construction.
 *
 * \ingroup CPP_BASE_MUTEX
 */
const AdoptLockTag adopt_lock = AdoptLockTag();

/** \} */

/**
 * Flexible ownership wrapper for a mutex.
 *
 * Provides exception controlled locking of a mutex with non-recursive semantics,
 * that gives more flexibility than LockGuard but also has slightly increased
 * memory and processing overhead. Each instance of a UniqueLock can be used by
 * one thread only!
 *
 * \notthreadsafe
 * \see Mutex, LockGuard
 * \tparam Mutex Used mutex type
 * \ingroup CPP_BASE_MUTEX
 */
template<typename Mutex = embb::base::Mutex>
class UniqueLock {
 public:
  /**
   * Creates a lock without assigned mutex.
   *
   * A mutex can be assigned to the lock using the method Swap().
   */
  UniqueLock();

  /**
   * Creates a lock from an unlocked mutex and locks it.
   *
   * \pre \c mutex is unlocked
   */
  explicit UniqueLock(
    Mutex& mutex
    /**< [IN] Mutex to be managed. */
    );

  /**
   * Creates a lock from an unlocked mutex without locking it.
   *
   * \pre \c mutex is unlocked
   */
  UniqueLock(
    Mutex& mutex,
    /**< [IN] Mutex to be managed */
    DeferLockTag
    /**< [IN] Tag to select correct constructor */
    );

  /**
   * Creates a lock from an unlocked mutex and tries to lock it.
   *
   * \pre \c mutex is unlocked
   */
  UniqueLock(
    Mutex& mutex,
    /**< [IN] Mutex to be managed */
    TryLockTag
    /**< [IN] Tag to select correct constructor */
    );

  /**
   * Creates a lock from an already locked mutex.
   *
   * \pre \c mutex is locked
   */
  UniqueLock(
    Mutex& mutex,
    /**< [IN] Mutex to be managed */
    AdoptLockTag
    /**< [IN] Tag to select correct constructor */
    );

  /**
   * Unlocks the mutex if owned.
   */
  ~UniqueLock();

  /**
   * Waits until the mutex is unlocked and locks it.
   *
   * \throws ErrorException, if no mutex is set or it is locked
   */
  void Lock();

  /**
   * Tries to lock the mutex and returns immediately.
   *
   * \return \c true if the mutex could be locked, otherwise \c false.
   * \throws ErrorException, if no mutex is set
   */
  bool TryLock();

  /**
   * Unlocks the mutex.
   *
   * \throws ErrorException, if no mutex is set or it is not locked
   */
  void Unlock();

  /**
   * Transfers ownership of a mutex to this lock.
   */
  void Swap(
    UniqueLock<Mutex>& other
    /**< [IN/OUT] Lock from which ownership shall be transferred */
    );

  /**
   * Gives up ownership of the mutex and returns a pointer to it.
   *
   * \return A pointer to the owned mutex or NULL, if no mutex was owned
   */
  Mutex* Release();

  /**
   * Checks whether the mutex is owned and locked.
   *
   * \return \c true if mutex is locked, otherwise \c false.
   */
  bool OwnsLock() const;

 private:
  /**
   * Holds reference to mutex realizing the lock.
   */
  Mutex* mutex_;

  /**
   * Stores information about whether the unique lock has locked the mutex.
   */
  bool locked_;

  /**
   * Disable copy construction and assignment.
   */
  UniqueLock(const UniqueLock<Mutex>&);
  UniqueLock<Mutex>& operator=(const UniqueLock<Mutex>&);

  /**
   * For access to native implementation type.
   */
  friend class embb::base::ConditionVariable;
};

} // namespace base
} // namespace embb

#include <embb/base/internal/mutex-inl.h>

#endif // EMBB_BASE_MUTEX_H_
