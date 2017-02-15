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

#ifndef EMBB_BASE_C_THREAD_SPECIFIC_STORAGE_H_
#define EMBB_BASE_C_THREAD_SPECIFIC_STORAGE_H_

/**
 * \defgroup C_BASE_THREADSPECIFIC Thread-Specific Storage
 * Thread-specific storage.
 *
 * Implements thread-specific storage (TSS), that is, memory locations that are
 * individual for each thread. Each thread has its own slot for a memory
 * address that can point to a (thread-specific) value. The value pointed to
 * has to be managed, i.e., created and deleted, by the user.
 *
 * \ingroup C_BASE
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <embb/base/c/errors.h>

/**
 * Opaque type representing a TSS.
 */
#ifdef DOXYGEN
typedef opaque_type embb_tss_t;
#else
typedef struct embb_tss_t {
  void** values;
  unsigned int size;
} embb_tss_t;
#endif /* else defined(DOXYGEN) */

/**
 * Creates thread-specific storage (TSS) pointer slots.
 *
 * \pre The given TSS has not yet been created or has already been deleted.
 * \return EMBB_SUCCESS if storage could be created \n
 *         EMBB_NOMEM if not enough memory was available
 * \memory embb_thread_get_max_count() pointers
 * \notthreadsafe
 * \see embb_tss_delete(), embb_thread_get_max_count()
 */
int embb_tss_create(
  embb_tss_t* tss
  /**< [OUT] Pointer to TSS */
  );

/**
 * Sets thread-specific slot value of the current thread.
 *
 * The value pointed to needs to be managed (created, deleted) by the user.
 *
 * \pre The given TSS has been created
 * \return EMBB_SUCCESS if value could be set \n
 *         EMBB_ERROR if no thread index could be obtained, that is, the
 *         maximum number of threads has been exceeded.
 * \lockfree
 * \see embb_tss_get(), embb_thread_get_max_count()
 */
int embb_tss_set(
  embb_tss_t* tss,
  /**< [IN/OUT] Pointer to TSS */
  void* value
  /**< [IN] Pointer to be stored in TSS */
  );

/**
 * Gets thread-specific TSS slot value of the current thread.
 *
 * \pre The given TSS has been created
 * \return Thread-specific value if embb_tss_set() has previously been called
 *         with a valid address. NULL, if no value was set or the calling thread
 *         could not obtain a thread-specific index.
 * \lockfree
 * \see embb_tss_set()
 */
void* embb_tss_get(
  const embb_tss_t* tss
  /**< [IN] Pointer to TSS */
  );

/**
 * Deletes all slots of the given TSS.
 *
 * Does not delete the values pointed to.
 *
 * \pre \c tss has been created successfully and is not NULL.
 * \post All slots are deleted
 * \notthreadsafe
 * \see embb_tss_create()
 */
void embb_tss_delete(
  embb_tss_t* tss
  /**< [IN/OUT] Pointer to TSS */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_THREAD_SPECIFIC_STORAGE_H_ */
