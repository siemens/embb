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

#include <embb/base/rwlock.h>
#include <embb/base/c/rwlock.h>

namespace embb {
namespace base {

RWLock::RWLock()
    : rwlock_(),
// Disable "this is used in base member initializer" warning.
// We explicitly want this.
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
      reader_lock_(*this),
      writer_lock_(*this) {
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
  embb_rwlock_init(&rwlock_);
}

RWLock::~RWLock() {
  embb_rwlock_destroy(&rwlock_);
}

void RWLock::LockRead() {
  embb_rwlock_lock_read(&rwlock_);
}

void RWLock::LockWrite() {
  embb_rwlock_lock_write(&rwlock_);
}

bool RWLock::TryLockRead() {
  int result = embb_rwlock_try_lock_read(&rwlock_);
  return result == EMBB_SUCCESS;
}

bool RWLock::TryLockWrite() {
  int result = embb_rwlock_try_lock_write(&rwlock_);
  return result == EMBB_SUCCESS;
}

void RWLock::UnlockRead() {
  embb_rwlock_unlock_read(&rwlock_);
}

void RWLock::UnlockWrite() {
  embb_rwlock_unlock_write(&rwlock_);
}

RWLock::ReaderLock::ReaderLock(RWLock& rwlock) : rwlock_(rwlock) {}
void RWLock::ReaderLock::Lock()    { rwlock_.LockRead(); }
bool RWLock::ReaderLock::TryLock() { return rwlock_.TryLockRead(); }
void RWLock::ReaderLock::Unlock()  { rwlock_.UnlockRead(); }

RWLock::WriterLock::WriterLock(RWLock& rwlock) : rwlock_(rwlock) {}
void RWLock::WriterLock::Lock()    { rwlock_.LockWrite(); }
bool RWLock::WriterLock::TryLock() { return rwlock_.TryLockWrite(); }
void RWLock::WriterLock::Unlock()  { rwlock_.UnlockWrite(); }

RWLock::ReaderLock& RWLock::GetReaderLock() {
  return reader_lock_;
}
RWLock::WriterLock& RWLock::GetWriterLock() {
  return writer_lock_;
}

} // namespace base
} // namespace embb




