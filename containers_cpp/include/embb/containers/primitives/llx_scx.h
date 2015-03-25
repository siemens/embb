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

#ifndef EMBB_CONTAINERS_PRIMITIVES_LLX_SCX_H_
#define EMBB_CONTAINERS_PRIMITIVES_LLX_SCX_H_

#include <embb/base/thread.h>
#include <embb/base/atomic.h>
#include <embb/base/thread_specific_storage.h>
#include <embb/containers/internal/fixed_size_list.h>

namespace embb { 
namespace containers {
namespace primitives {

namespace internal {

/**
 * SCX operation description. An SCX record contains all information
 * required to allow any process to complete a pending SCX operation.
 */
template< class DataRecord >
class ScxRecord {
 
 private:
  typedef size_t cas_t;
  typedef ScxRecord< DataRecord > self_t;

 public:
  /**
   * Possible states of an LLX/SCX operation.
   */
  typedef enum {
    Undefined = 0,
    Comitted,
    Aborted,
    InProgress
  } OperationState;

 public:
  /**
   * Default constructor, creates sentinel instance of ScxRecord.
   */
  ScxRecord()
  : linked_data_records_(0),
    finalize_data_records_(0)
    new_value_(0),
    old_value_(0),
    scx_ops_(0),
    state_(OperationState::Undefined),
    all_frozen_(false) {
    field_ = 0;
  }

  /**
   * Constructor.
   */
  ScxRecord(
    embb::containers::internal::FixedSizeList<DataRecord *> & linked_data_records,
    embb::containers::internal::FixedSizeList<DataRecord *> & finalize_data_records,
    embb::base::Atomic<cas_t> * field,
    cas_t new_value,
    cas_t old_value,
    embb::containers::internal::FixedSizeList<self_t *> * scx_ops,
    OperationState operation_state)
  : linked_data_records_(&linked_data_records),
    finalize_data_records_(&finalize_data_records),
    new_value_(new_value),
    old_value_(old_value),
    scx_ops_(scx_ops),
    state_(operation_state),
    all_frozen_(false) {
    field_ = field;
  }

  OperationState State() const {
    return state_;
  }

  /**
   * Returns true if helped operation has been completed.
   */
  bool Help();

 private:
  /**
   * Sequence of load-linked data records for this SCX operation.
   * Named 'V' in original publication.
   */
   const embb::containers::internal::FixedSizeList<DataRecord *> *
    linked_data_records_;

  /**
   * Sequence of data records to be finalized in this SCX operation.
   * Named 'R' in original publication.
   */
   const embb::containers::internal::FixedSizeList<DataRecord *> *
    finalize_data_records_;

  /**
   * Pointer to a mutable field of a data record in data_records the
   * new value is to be stored.
   * Named 'fld' in original publication.
   */
  embb::base::Atomic<cas_t> * field_;

  /**
   * Value to be written in field referenced by field_index. 
   * Required to be compatible with atomic operations.
   */
  cas_t new_value_;

  /**
   * Value previously read from field referenced by field_index.
   * Required to be compatible with atomic operations.
   */
  cas_t old_value_;

  /**
   * List of SCX operation descriptions associated with data records
   * linked with this SCX operation.
   */
  embb::containers::internal::FixedSizeList<self_t *> scx_ops_;

  /**
   * Current state of this SCX record.
   */
  OperationState state_;

  /**
   * Whether all fields are currently frozen, initially false. 
   * Set to true after all data records in data_records V have 
   * been frozen for the SCX.
   */
  bool all_frozen_;

}; /* class ScxRecord */

}  // namespace internal

/**
 * Wraps user-defined data with fields required for LLX/SCX algorithm.
 * Mutable fields must each be contained in a single word.
 */
template< class UserData >
class LlxScxRecord {

 private:
  typedef LlxScxRecord<UserData> self_t;
  typedef internal::ScxRecord<self_t> ScxRecord_t;
  typedef typename ScxRecord_t::OperationState OperationState;

  /**
   * The dummy SCX record always has state = Aborted.
   */
  static ScxRecord_t dummy_scx;

 public:
  /**
   * Default constructor.
   */
  LlxScxRecord();

  /**
   * Constructor. Creates an instance of \c DataRecord_t wrapping a user
   * data object.
   */
  LlxScxRecord(const UserData & user_data);

  /**
   * Copy constructor
   */
  LlxScxRecord(const LlxScxRecord & other)
  : user_data_(other.user_data_) {
    scx_op_.Store(other.scx_info_.Load());
    marked_for_finalize_ = other.marked_for_finalize_;
  }

  /**
   * Assignment operator.
   */
  LlxScxRecord & operator=(const LlxScxRecord & rhs) {
    user_data_ = rhs.user_data_;
    scx_op_.Store(rhs.scx_info_.Load());
    marked_for_finalize_ = rhs.marked_for_finalize_;
  }

  /**
   * Destructor.
   */
  ~LlxScxRecord() {
  }
    
  /**
   * Returns list of \c NumMutableElements mutable fields in this 
   * data record.
   */
  UserData & Data() {
    return user_data_;
  }

  UserData * operator*() {
    return &user_data_;
  }

  UserData * operator->() {
    return &user_data_;
  }

  /**
   * While this SCX is active, the info field acts as a kind of lock
   * on the data record, granting exclusive access to this SCX, rather
   * than to a process.
   * A data record r is frozen for an SCX-record U if r.info points to
   * U and either U.state is InProgress, or U.state is Committed and r
   * is marked.
   * While a data record r is frozen for an SCX record U, a mutable
   * field f of r can be changed only if f is the field pointed to by
   * U.fld, and it can only be changed by a process helping the SCX
   * that created U.
   */
  inline bool IsFrozenFor(const ScxRecord_t & scx) const {
    return scx_op_ == &scx &&
            (scx.State() == InProgress ||
            (scx.State() == Comitted && marked_));
  }

  /**
   * Returns the pointer to the SCX record holding exclusive access to
   * this data record.
   */
  inline embb::base::Atomic<ScxRecord_t *> & ScxInfo() {
    return scx_op_;
  }

  /** 
   * Mark this data record for finalizing.
   */
  inline void MarkForFinalize() {
    marked_for_finalize_ = true;
  }

  /** 
   * Mark this data record for finalizing.
   */
  inline bool IsMarkedForFinalize() const {
    return marked_for_finalize_;
  }
    
  private:
  /**
   * Instance of the user-defined data type containing mutable 
   * fields.
   */
  UserData user_data_;
  
  /**
   * Pointer to SCX record that describes the last SCX that accessed this
   * data record, initialized with dummy SCX record.
   */
  embb::base::Atomic<ScxRecord_t *> scx_op_;
    
  /**
   * Marked flag, whether this data record has been finalized.
   * The marked bit is initially false and only ever changes from false
   * to true.     
   */
  bool marked_for_finalize_;

};  // class LlxScxRecord

/** 
 * Multiword LL/SC
 * 
 * Implementation of the LLX/STX primitive as presented in 
 * "Pragmatic Primitives for Non-blocking Data Structures" 
 * (Brown et al., 2013).
 *
 * \tparam MaxLinks Maximum number of active LL-dependencies per thread
 * \tparam UserData Type containing mutable fields
 */
template< class UserData >
class LlxScx {

 private:  
  typedef size_t cas_t;
  typedef LlxScxRecord<UserData> DataRecord_t;
  typedef internal::ScxRecord< LlxScxRecord<UserData> > ScxRecord_t;
  typedef typename ScxRecord_t::OperationState OperationState;

 public:
  /**
   * Constructs a new instance of LlxScx.
   */
  LlxScx(
    size_t max_links
    /**< [IN] Maximum number of links depending on a single SCX operation */
  );

  /**
   * Destructor, frees memory.
   */
  ~LlxScx();

  /**
   * Tentatively performs an LLX (extended load-linked) operation on given 
   * LLX/SCX data record.
   * Returns true and stores result in given reference variable on success, 
   * otherwise returns false.
   */
  bool TryLoadLinked(
    DataRecord_t * const data_record,
    /**< [IN] Pointer to data record to load */
    DataRecord_t & data,
    /**< [OUT] Atomic snapshot of \c NumMutableFields fields in data
               record at given index */
    bool & finalized
    /**< [OUT] Indicating whether requested fields have been finalized */
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
   * Performs a VLX (extended validate link) operation on given LLX data
   * record. 
   * Returns true if the calling thread's link obtained by its most recent
   * invocation of SCX is still valid.
   *
   * Precondition: 
   * - for each data record r in data_records, the active thread
   *   has performed and r.LLX linked to this VLX.
   */
  bool TryValidateLink(
    const DataRecord_t & data_record
    /**< [IN] Linked data record to validate */
  );
 
 private:

  typedef struct {
    DataRecord_t * data_record;
    ScxRecord_t * scx_record;
    UserData user_data;
  } LlxResult;

  /**
   * Resolves the calling thread's Id.
   */
  unsigned int ThreadId();

  /**
   * Maximum number of active links created via TryLoadLinked per thread.
   */
  size_t max_links_;

  /**
   * Shared table containing for each r in V, a copy of r's info 
   * value in this thread's local table of LLX results.
   * 
   *  thread_id -> {
   *    r_1 in V -> *ScxRecord(thread_llx_results_[r_1].data_record.ScxInfo()),
   *    ...
   *    r_i in V -> *ScxRecord(thread_llx_results_[r_i].data_record.ScxInfo())
   *  }
   */
  embb::containers::internal::FixedSizeList<ScxRecord_t> * info_fields_;

  /**
   * Thread-specific list of LLX results performed by the thread.
   */
  embb::base::ThreadSpecificStorage<
    embb::containers::internal::FixedSizeList<LlxResult> >
      thread_llx_results_;

  /**
   * Prevent default construction.
   */
  LlxScx();
  /**
   * Prevent copy construction.
   */
  LlxScx(const LlxScx &);
  /**
   * Prevent assignment.
   */
  LlxScx & operator=(const LlxScx &);

};

} // namespace primitives
} // namespace containers
} // namespace embb

#include <embb/containers/internal/primitives/llx_scx-inl.h>

#endif  // EMBB_CONTAINERS_PRIMITIVES_LLX_SCX_H_