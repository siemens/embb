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

#ifndef EMBB_BASE_THREAD_SPECIFIC_STORAGE_H_
#define EMBB_BASE_THREAD_SPECIFIC_STORAGE_H_

#include <embb/base/c/thread_specific_storage.h>
#include <embb/base/c/internal/thread_index.h>
#include <embb/base/exceptions.h>

namespace embb {
namespace base {

namespace test {
/**
 * Forward declaration for friending.
 */
class ThreadSpecificStorageTest;
}

/**
 * \defgroup CPP_BASE_TSS Thread-Specific Storage
 *
 * %Thread specific storage.
 *
 * \ingroup CPP_BASE
 */

/**
 * Represents thread-specific storage (TSS).
 *
 * Provides for each thread a separate slot storing an object of the given type.
 *
 * \tparam Type Type of the objects
 * \ingroup CPP_BASE_TSS
 */
template<typename Type>
class ThreadSpecificStorage {
 public:
  /**
   * Creates the TSS and default initializes all slots.
   *
   * \throws NoMemoryException if not enough memory is available to allocate
   *         the TSS slots
   * \memory Dynamically allocates embb::base::Thread::GetThreadsMaxCount()
   *         pointers and slots of the TSS type
   * \notthreadsafe
   */
  ThreadSpecificStorage();

#ifdef DOXYGEN
  /**
   * Creates the TSS and initializes all slots with up to four constructor
   * arguments.
   *
   * The TSS objects are created by calling their constructor as follows:
   * Type(initializer1, ...).
   *
   * \throws NoMemoryException if not enough memory is available to allocate
   *         the TSS slots
   * \memory Dynamically allocates embb::base::Thread::GetThreadsMaxCount()
   *         pointers and slots of the TSS type
   * \notthreadsafe
   */
  template<typename Initializer1, ...>
  explicit ThreadSpecificStorage(
    Initializer1 initializer1,
    /**< [IN] First argument for constructor\n ... */
    ...
    );
#else
  /**
   * See description of Doxygen variadic constructor above.
   */
  template<typename Initializer>
  explicit ThreadSpecificStorage(Initializer initializer);

  /**
   * See description of Doxygen variadic constructor above.
   */
  template<typename Initializer1, typename Initializer2>
  ThreadSpecificStorage(Initializer1 initializer1, Initializer2 initializer2);

  /**
   * See description of Doxygen variadic constructor above.
   */
  template<typename Initializer1, typename Initializer2, typename Initializer3>
  ThreadSpecificStorage(Initializer1 initializer1, Initializer2 initializer2,
                        Initializer3 initializer3);

  /**
   * See description of Doxygen variadic constructor above.
   */
  template<typename Initializer1, typename Initializer2, typename Initializer3,
           typename Initializer4>
  ThreadSpecificStorage(Initializer1 initializer1, Initializer2 initializer2,
                        Initializer3 initializer3, Initializer4 initializer4);
#endif // else defined(DOXYGEN)

  /**
   * Destroys the TSS and frees allocated memory for the TSS slots.
   */
  ~ThreadSpecificStorage();

  /**
   * Returns a reference to the TSS slot of the current thread.
   *
   * \return Reference to TSS slot
   *
   * \throws embb::base::ErrorException if the maximum number of threads has
   *         been exceeded
   * \lockfree
   * \see Get() const
   */
  Type& Get();

  /**
   * Returns a const reference to the TSS slot of the current thread.
   *
   * \return Constant reference to TSS slot
   *
   * \throws embb::base::ErrorException if the maximum number of threads has
   *         been exceeded
   * \lockfree
   * \see Get()
   */
  const Type& Get() const;

 private:
  /**
   * Common construction code.
   */
  void Prepare();

  /**
   * Representation of TSS implemented in Base C.
   */
  embb_tss_t rep_;

  /**
   * Flags to indicate the usage of a TSS value. Are set on first access.
   */
  bool* usage_flags_;

  /**
   * To allow white-box tests.
   */
  friend class test::ThreadSpecificStorageTest;
};

} // namespace base
} // namespace embb

#include <embb/base/internal/thread_specific_storage-inl.h>

#endif  // EMBB_BASE_THREAD_SPECIFIC_STORAGE_H_
