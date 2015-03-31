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

#ifndef EMBB_CONTAINERS_MULTIWORD_LL_SC_H_
#define EMBB_CONTAINERS_MULTIWORD_LL_SC_H_

#include <embb/base/thread.h>
#include <embb/base/atomic.h>
#include <embb/base/function.h>
#include <embb/base/thread_specific_storage.h>
#include <embb/containers/object_pool.h>
#include <embb/containers/lock_free_tree_value_pool.h>
#include <embb/containers/internal/fixed_size_list.h>
#include <embb/containers/internal/llx_scx.h>

namespace embb { 
namespace containers {
namespace internal {

#ifdef DOXYGEN
/** 
 * Multiword LL/SC
 * 
 * Implementation of the LLX/STX primitive as presented in 
 * "Pragmatic Primitives for Non-blocking Data Structures" 
 * (Brown et al., 2013).
 *
 * \tparam UserData Type containing mutable fields
 * \tparam ValuePool Type containing mutable fields
 */
template<
  typename UserData,
  typename ValuePool = embb::containers::LockFreeTreeValuePool< bool, false >
>
class MultiwordLLSC {
 public:
  /**
   * Constructs a new instance of LlxScx.
   */
   MultiwordLLSC(
    size_t max_links
    /**< [IN] Maximum number of links depending on a single SCX operation */
  );

  /**
   * Destructor, frees memory.
   */
   ~MultiwordLLSC();

  /**
   * Tentatively performs an LLX (extended load-linked) operation on given 
   * LLX/SCX data record.
   * Returns true and stores result in given reference variable on success, 
   * otherwise returns false.
   */
  bool TryLoadLinked(
    DataRecord_t * const data_record,
    /**< [IN] Pointer to data record to load */
    UserData & data,
    /**< [OUT] Atomic snapshot of data record */
    bool & finalized
    /**< [OUT] Indicating whether requested fields have been finalized */
  );

  /**
   * Tentatively performs an LLX (extended load-linked) operation on given 
   * LLX/SCX data record.
   * Returns true and stores result in given reference variable on success, 
   * otherwise returns false.
   */
  bool TryLoadLinked(
    DataRecord_t * const data_record,
    /**< [IN] Pointer to data record to load */
    UserData & data
    /**< [OUT] Atomic snapshot of data record */
  );
 
  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    embb::base::Atomic<FieldType> * field,
    /**< [IN] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps,
    /**< [IN] Data records linked to this store operation */
    embb::containers::internal::FixedSizeList<DataRecord_t *> & finalize_deps
    /**< [IN] Data records to be finalized in this store operation */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    embb::base::Atomic<FieldType> * field,
    /**< [IN] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps
    /**< [IN] Data records linked to this store operation */
  );

  /**
   * Performs a VLX (extended validate link) operation on given LLX data
   * record. 
   * Before calling this method, the given LLX/SCX record must have been
   * linked via \c TryLoadLinked.
   *
   * \returns True if the calling thread's link obtained by its most recent
   *          invocation of SCX is still valid.
   */
  bool TryValidateLink(
    const DataRecord_t & data_record
    /**< [IN] Linked data record to validate */
  );
};

#else

/**
 * Multiword LL/SC
 * 
 * Implementation of the LLX/STX primitive as presented in 
 * "Pragmatic Primitives for Non-blocking Data Structures" 
 * (Brown et al., 2013).
 *
 * \tparam UserData Type containing mutable fields
 * \tparam ValuePool Type containing mutable fields
 */
template<
  typename UserData,
  typename ValuePool = embb::containers::LockFreeTreeValuePool< bool, false >
>
class MultiwordLLSC {
 public:
  /**
   * Wrapper of user-defined data for LLX/SCX operations
   */
  typedef internal::LlxScxRecord<UserData> Link;

 public:
  /**
   * Constructs a new instance of LlxScx.
   */
   MultiwordLLSC(
    size_t max_links
    /**< [IN] Maximum number of links depending on a single SCX operation */
  );

  /**
   * Destructor, frees memory.
   */
   ~MultiwordLLSC();

  /**
   * Tentatively performs an LLX (extended load-linked) operation on given 
   * LLX/SCX data record.
   * Returns true and stores result in given reference variable on success, 
   * otherwise returns false.
   */
  bool TryLoadLinked(
    Link * const link,
    /**< [IN] Data record to load-link */
    UserData & data,
    /**< [OUT] Atomic snapshot of data record at the time the link has been
               established */
    bool & finalized
    /**< [OUT] Indicating whether requested fields have been finalized */
  );

  /**
   * Tentatively performs an LLX (extended load-linked) operation on given 
   * LLX/SCX data record.
   * Returns true and stores result in given reference variable on success, 
   * otherwise returns false.
   */
  bool TryLoadLinked(
    Link * const link,
    /**< [IN] Data record to load-link */
    UserData & data
    /**< [OUT] Atomic snapshot of data record at the time the link has been
               established */
  );
 
  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value
    /**< [IN] Value to store */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep_1,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_2,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value
    /**< [IN] Value to store */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep_1,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_2,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_3,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value
    /**< [IN] Value to store */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    Link & finalize_dep
    /**< [IN] Data records to be finalized in this store operation */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep_1,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_2,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    Link & finalize_dep
    /**< [IN] Data records to be finalized in this store operation */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep_1,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_2,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_3,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    Link & finalize_dep
    /**< [IN] Data records to be finalized in this store operation */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    Link & finalize_dep_1,
    /**< [IN] Data record to be finalized in this store operation */
    Link & finalize_dep_2
    /**< [IN] Data record to be finalized in this store operation */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep_1,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_2,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    Link & finalize_dep_1,
    /**< [IN] Data record to be finalized in this store operation */
    Link & finalize_dep_2
    /**< [IN] Data record to be finalized in this store operation */
  );

  /**
   * Tentatively performs a single-record Store-Conditional operation on 
   * given LLX/SCX data record.
   * Returns true if the given value has been stored successfully, otherwise
   * false.
   */
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep_1,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_2,
    /**< [IN] Data record linked to this store operation */
    Link & linked_dep_3,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType> * field,
    /**< [IN,OUT] Pointer to the field the value will be stored into */
    FieldType value,
    /**< [IN] Value to store */
    Link & finalize_dep_1,
    /**< [IN] Data record to be finalized in this store operation */
    Link & finalize_dep_2
    /**< [IN] Data record to be finalized in this store operation */
  );

#if 0
  template< typename FieldType >
  bool TryStoreConditional(
    Link & linked_dep,
    /**< [IN] Data record linked to this store operation */
    embb::base::Atomic<FieldType *> * field,
    /**< [IN] Pointer to the field the value will be stored into */
    FieldType * value,
    /**< [IN] Value to store */
    Link & finalize_dep_1,
    /**< [IN] Data record to be finalized in this store operation */
    Link & finalize_dep_2
    /**< [IN] Data record to be finalized in this store operation */
    );
#endif

  /**
   * Performs a VLX (extended validate link) operation on given LLX data
   * record. 
   * Before calling this method, the given LLX/SCX record must have been
   * linked via \c TryLoadLinked.
   *
   * \returns True if the calling thread's link obtained by its most recent
   *          invocation of SCX is still valid.
   */
  bool TryValidateLink(
    const Link & link
    /**< [IN] Linked data record to validate */
  );
  
 private:
  /**
   * Maximum number of threads engaging in operations on this LLX/SCX instance.
   */
  unsigned int max_threads_;

  /**
   * Core implementation of LLX/SCX algorithm
   */
  internal::LlxScx<UserData, ValuePool> llx_scx_;

  /**
   * Prevent default construction.
   */
  MultiwordLLSC();

  /**
   * Prevent copy construction.
   */
  MultiwordLLSC(const MultiwordLLSC &);

  /**
   * Prevent assignment.
   */
  MultiwordLLSC & operator=(const MultiwordLLSC &);

  /**
   * Convert given value to type compatible with CAS
   */
  template < typename FieldType >
  inline cas_t ToCASValue(FieldType value) {
    // Value punning: memcpy given value to type compatible with CAS
    cas_t return_value;
    FieldType native_value = value;
    memcpy(&return_value, &native_value, sizeof(return_value));
    return return_value;
  }

  /**
   * Convert value referenced by given pointer to type compatible with CAS
   */
  template < typename FieldType >
  inline cas_t ToCASValue(FieldType * value) {
    // Value punning: memcpy value referenced by given pointer to type
    // compatible with CAS
    cas_t return_value;
    memcpy(&return_value, value, sizeof(return_value));
    return return_value;
  }

  template < typename FieldType >
  inline void Assign(
    const FieldType & source,
    FieldType & target) {
    source = target;
  }

  template < typename FieldType >
  inline void Assign(
    const embb::base::Atomic<FieldType> & source,
    embb::base::Atomic<FieldType> & target) {
    target.Store(source.Load());
  }
};

#endif  // DOXYGEN
} // namespace primitives
} // namespace containers
} // namespace embb

#include <embb/containers/internal/llx_scx-inl.h>

#endif  // EMBB_CONTAINERS_MULTIWORD_LL_SC_H_