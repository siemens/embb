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

#ifndef EMBB_CONTAINERS_PRIMITIVES_LLX_SCX_INL_H_
#define EMBB_CONTAINERS_PRIMITIVES_LLX_SCX_INL_H_

#include <embb/containers/primitives/llx_scx.h>
#include <embb/base/thread.h>
#include <embb/base/atomic.h>
#include <embb/base/memory_allocation.h>
#include <vector>
#include <stdarg.h>

/** 
 * Implementation of the LX/STX primitive as presented in 
 * "Pragmatic Primitives for Non-blocking Data Structures" 
 * (Brown et al., 2013).
 */

namespace embb { 
namespace containers {
namespace primitives {

namespace internal {

#if 0
/**
 * RAII-style implementation of a simplified hazard
 * pointer scheme. SmartHazardPointer instances should
 * reside only on the stack, never the heap, and T*
 * values should reside only in the heap. A NodePtr can
 * be automatically converted to a Node*, but an explicit
 * constructor is needed to go the other way.
 *
 * Example:
 *   SmartHazardPointer<Node> shp = SmartHazardPointer(
 *     nodePool?>allocate());
 *   // assign via default copy constructor:
 *   *shp = Node(...);
 */
template< typename T >
class SmartHazardPointer {

 public:
  SmartHazardPointer(T ** node) {
    while (true) {
      ptr = *node;
      table_->add(ptr);
      // Full fence:
      embb_atomic_memory_barrier();
      T * reread = *node;
      // @TODO: Prove practical wait-freedom
      if (read == reread) {
        return;
      }
    }
  }

  SmartHazardPointer(T * node) {
    ptr = node;
  }

  /**
   * Dereference operator.
   */
  T * operator->() {
    return ptr;
  }

  /**
   * Dereference lvalue.
   */
  T & operator*() {
    return *ptr;
  }

  /**
   * Equality test with regular pointer
   */
  bool operator==(T * const other) {
    return ptr == other;
  }

  /**
   * Equality test with another SmartHazardPointer
   */
  bool operator==(const SmartHazardPointer & other) {
    return this->ptr == other->ptr;
  }

  /**
   * Destructor, retires hazard pointer.
   */
  ~SmartHazardPointer() {
    table_->remove(ptr);
  }

  /**
   * Conversion to regular pointer.
   */
  operator T*() {
    return ptr;
  }

 private:
  static HazardPointerTable * table_;
  T * ptr;

};

#endif

}  // namespace internal

template< class UserData >
unsigned int LlxScx<UserData>::ThreadId() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);
  if (return_val != EMBB_SUCCESS)
  EMBB_THROW(embb::base::ErrorException, "Could not get thread id!");
  return thread_index;
}

template< class UserData >
LlxScx<UserData>::LlxScx(size_t max_links)
: max_links_(max_links) {
  typedef embb::containers::internal::FixedSizeList<LlxResult>
    llx_result_list_t;
  typedef embb::containers::internal::FixedSizeList<ScxRecord_t>
    scx_record_list_t;
  unsigned int num_threads = embb::base::Thread::GetThreadsMaxCount();
  // Table in shared memory containing for each r in TryStoreConditional's
  // dependent links, a copy of r's info value in this threads local table
  // of LLX results.
  scx_record_list_t empty_scx_list(3);
  info_fields_ = static_cast<scx_record_list_t *>(
    embb::base::Allocation::AllocateCacheAligned(
      num_threads * sizeof(empty_scx_list)));
}

template< class UserData >
LlxScx<UserData>::~LlxScx() {
  embb::base::Allocation::FreeAligned(info_fields_);
}

template< class UserData >
bool LlxScx<UserData>::TryLoadLinked(
  DataRecord_t * const data_record,
  DataRecord_t & user_data,
  bool & finalized) {
  finalized = false;
  // Order of initialization matters:
  bool           marked_1   = data_record->IsMarkedForFinalize();
  ScxRecord_t *  curr_scx   = data_record->ScxInfo().Load();
  OperationState curr_state = curr_scx->State();
  bool           marked_2   = data_record->IsMarkedForFinalize();
  if (curr_state == OperationState::Aborted ||
      (curr_state == OperationState::Comitted && !marked_2)) {
    // read mutable fields into local variable:
    UserData user_data_local(data_record->Data());
    if (data_record->ScxInfo() == curr_scx) {
      // store <r, curr_scx, user_data_local> in
      // the thread-specific table:
      LlxResult llx_result;
      llx_result.data_record = data_record;
      llx_result.scx_record  = curr_scx;
      llx_result.user_data   = user_data_local;
      thread_llx_results_.Get().PushBack(llx_result);
      // Set return value:
      user_data = user_data_local;
      return true;
    }
  }
  // @TODO: Re-check if logical precedence is okay here (see paper):
  if (curr_scx->State() == OperationState::Comitted ||
      (curr_scx->State() == OperationState::InProgress &&
       curr_scx->Help() &&
       marked_1)) {
    // Successfully completed active SCX:
    finalized = true;
    return false;
  }
  if (data_record->ScxInfo()->State() == OperationState::InProgress) {
    // Help active SCX:
    data_record->ScxInfo()->Help();
  }
  return false;
}

template< class UserData >
template< typename FieldType >
bool LlxScx<UserData>::TryStoreConditional(
  embb::base::Atomic<FieldType> * field,
  FieldType value,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & finalize_deps) {
  typedef embb::containers::internal::FixedSizeList<DataRecord_t *> dr_list_t;
//typedef embb::containers::internal::FixedSizeList<ScxRecord_t *> op_list_t;
  // Preconditions:
  // 1. For each r in linked_deps, this thread has performed an invocation
  //    I_r of LLX(r) linked to this SCX.
  // 2. Given value is not initial value of field.
  // 3. For each r in V, no SCX(V',R',field,value) has been linearized before
  //    any I_r was linearized.
  unsigned int thread_id = ThreadId();
  // Let info_fields be a table in shared memory containing for each r in V,
  // a copy of r's info value in this threads local table of LLX results:
  info_fields_[thread_id].clear();
  dr_list_t::const_iterator it;
  dr_list_t::const_iterator end;
  end = linked_deps.end();
  // for each r in V ...
  for (it = linked_deps.begin(); it != end; ++it) {
    // Find LLX result of r in thread-local table of LLX results:
    typedef embb::containers::internal::FixedSizeList<LlxResult>
      llx_result_list;
    llx_result_list::iterator l_it  = thread_llx_results_.Get().begin();
    llx_result_list::iterator l_end = thread_llx_results_.Get().end();
    for (; l_it != l_end && l_it->data_record != *it; ++l_it);
    if (l_it == l_end) {
      // Missing LLX result for given linked data record, user did not
      // load-link a data record this SCX depends on.
      EMBB_THROW(embb::base::ErrorException,
        "Missing preceding LLX on a data record used for SCX");
    }
    // ... copy of r's info value in this threads local table of LLX results
    ScxRecord_t scx_op(*(l_it->data_record->ScxInfo().Load()));
    info_fields_[thread_id].PushBack(scx_op);
  }
  // Announce SCX operation. Lists linked_deps and finalize_dep are 
  // guaranteed to remain on the stack until this announced operation
  // is completed, so no allocation/pool is necessary.
  ScxRecord_t scx(
    linked_deps,
    finalize_deps,
    // target field:
    reinterpret_cast<embb::base::Atomic<cas_t> *>(field),
    // new value:
    reinterpret_cast<cas_t>(value),
    // old value:
    reinterpret_cast<cas_t>(field->Load()),
    // linked SCX operations:
    &info_fields_[thread_id],
    // initial operation state:
    OperationState::InProgress);
  return scx.Help();
}

template< class UserData >
bool LlxScx<UserData>::TryValidateLink(
  const DataRecord_t & field) {
  return true; // @TODO
}

// LlxScxRecord

template< class UserData >
LlxScxRecord<UserData>::LlxScxRecord()
: marked_for_finalize_(false) {
  scx_op_.Store(&dummy_scx);
}

template< class UserData >
LlxScxRecord<UserData>::LlxScxRecord(
  const UserData & user_data)
: user_data_(user_data),
  marked_for_finalize_(false) {
  scx_op_.Store(&dummy_scx);
}

// internal::ScxRecord

template< class DataRecord >
bool internal::ScxRecord<DataRecord>::Help() {
  // We ensure that an SCX S does not change a data record 
  // while it is frozen for another SCX S'. Instead, S uses 
  // the information in the SCX record of S' to help S'
  // complete, so that the data record can be unfrozen.
  typedef embb::containers::internal::FixedSizeList<DataRecord *> dr_list_t;
  // Freeze all data records in data_records to protect their 
  // mutable fields from being changed by other SCXs:
  dr_list_t::iterator linked_it  = linked_data_records_->begin();
  dr_list_t::iterator linked_end = linked_data_records_->end();
  for (unsigned int fieldIdx = 0;
       linked_it != linked_end;
       ++linked_it, ++fieldIdx) {
    DataRecord * r = *linked_it;
    // pointer indexed by r in this->info_fields:
    ScxRecord<DataRecord> * rinfo = &info_fields_[fieldIdx];
    if (!r->ScxInfo().CompareAndSwap(rinfo, this)) {
      if (r->ScxInfo().Load() != this) {
        // could not freeze r because it is frozen for 
        // another SCX:
        if (all_frozen_) {
          // SCX already completed:
          return true;
        }
        // atomically unfreeze all nodes frozen for this SCX:
        state_ = Aborted;
        return false;
      }
    }
  }
  // finished freezing data records
  assert(state_ == InProgress || state_ == Comitted);
  // frozen step:
  all_frozen_ = true;
  // mark step:
  dr_list_t::iterator finalize_it  = finalize_data_records_->begin();
  dr_list_t::iterator finalize_end = finalize_data_records_->end();
  for (unsigned int field_idx = finalize_range_.first;
       finalize_it != finalize_end;
       ++finalize_it, ++fieldRangeIdx) {
    linked_data_records_[fieldRangeIdx]->MarkForFinalize();
  }
  // update CAS:
  cas_t expected_old_value = old_value_;
  field_->CompareAndSwap(expected_old_value, new_value_);
  // Commit step.
  // Finalizes all r in data_records within finalize range and
  // unfreezes all r in data_records outside of finalize range. 
  // Linearization point of this operation.
  state_ = Comitted;
  return true;
}

} // namespace primitives
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_PRIMITIVES_LLX_SCX_INL_H_
