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

#ifndef EMBB_CONTAINERS_INTERNAL_LLX_SCX_INL_H_
#define EMBB_CONTAINERS_INTERNAL_LLX_SCX_INL_H_

#include <embb/containers/internal/llx_scx.h>
#include <embb/base/thread.h>
#include <embb/base/atomic.h>
#include <embb/base/memory_allocation.h>
#include <algorithm>

namespace embb { 
namespace containers {
namespace internal {

template< typename UserData, typename ValuePool >
unsigned int LlxScx<UserData, ValuePool>::ThreadId() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);
  if (return_val != EMBB_SUCCESS)
  EMBB_THROW(embb::base::ErrorException, "Could not get thread id");
  return thread_index;
}

template< typename UserData, typename ValuePool >
LlxScx<UserData, ValuePool>::LlxScx(size_t max_links)
: max_links_(max_links),
  max_threads_(embb::base::Thread::GetThreadsMaxCount()),
  scx_record_list_pool_(max_threads_ * max_threads_ * 2),
  scx_record_pool_(max_threads_ * max_threads_ * 2),
  // Disable "this is used in base member initializer" warning.
  // We explicitly want this.
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  delete_operation_callback(*this, &self_t::DeleteOperationCallback),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
  hp(delete_operation_callback, NULL, 2) {
  typedef embb::containers::internal::FixedSizeList<LlxResult>
    llx_result_list_t;
  typedef embb::containers::internal::FixedSizeList<ScxRecord_t>
    scx_record_list_t;
  // Allocate a list of LLX results for every thread:
  thread_llx_results_ = static_cast<llx_result_list_t **>(
    embb::base::Allocation::AllocateCacheAligned(
      max_threads_ * sizeof(llx_result_list_t*)));
  // Using Allocation::New to create every list instance as FixedSizeList
  // does not provide a default constructor and Allocation::Allocate does
  // not allow constructor arguments.
  for (unsigned int thread_idx = 0; thread_idx < max_threads_; ++thread_idx) {
    thread_llx_results_[thread_idx] =
      embb::base::Allocation::New<llx_result_list_t>(max_links_);
  }
}

template< typename UserData, typename ValuePool >
LlxScx<UserData, ValuePool>::~LlxScx() {
  // Delete thread-specific lists of LLX results:
  for (unsigned int thread_idx = 0; thread_idx < max_threads_; ++thread_idx) {
    embb::base::Allocation::Delete(thread_llx_results_[thread_idx]);
  }
  // Delete array of list pointers:
  embb::base::Allocation::FreeAligned(thread_llx_results_);
}

template< typename UserData, typename ValuePool >
bool LlxScx<UserData, ValuePool>::TryLoadLinked(
  DataRecord_t * const data_record,
  UserData & user_data,
  bool & finalized) {
  finalized = false;
  unsigned int thread_id = ThreadId();
  // Order of initialization matters:
  volatile bool marked_1 = data_record->IsMarkedForFinalize();
  ScxRecord_t * curr_scx = data_record->ScxInfo().Load();
  // Guard active SCX record of this data record using guard 1.
  // When calling help with this SCX record, it will be guarded
  // using guard 0 again.
  // This hazard pointer is validated in the nested if-block below.
  hp.GuardPointer(1, curr_scx);
  volatile OperationState curr_state = curr_scx->state;
  bool marked_2 = data_record->IsMarkedForFinalize();
  if (curr_state == OperationState::Aborted ||
      (curr_state == OperationState::Comitted && !marked_2)) {
    // read mutable fields into local variable:
    UserData user_data_local(data_record->Data());
    if (data_record->ScxInfo().Load() == curr_scx) {
      // Active SCX record of data record did not change.
      // Store <r, curr_scx, user_data_local> in
      // the thread-specific table.
      // LLX results do not need to be guarded as they local to the
      // thread.
      LlxResult llx_result;
      llx_result.data_record = data_record;
      llx_result.scx_record  = curr_scx;
      llx_result.user_data   = user_data_local;
      assert(thread_llx_results_[thread_id]->PushBack(llx_result));
      // Set return value:
      user_data = user_data_local;
      return true;
    }
  }
  // Active SCX record of data record has been changed in between
  if (marked_1 && 
      (curr_scx->state == OperationState::Comitted ||
       (curr_scx->state == OperationState::InProgress && Help(curr_scx)))) {
    // Successfully completed the data record's active SCX but failed to
    // complete the LLX operation because the data record has been finalized:
    finalized = true;
    return false;
  }
  if (data_record->ScxInfo().Load()->state == OperationState::InProgress) {
    // Help active SCX.
    // This SCX record has been guarded above.
    ScxRecord_t * data_record_scx = data_record->ScxInfo().Load();
    Help(data_record_scx);
  }
  return false;
}

template< typename UserData, typename ValuePool >
void LlxScx<UserData, ValuePool>::ClearLinks() {
  // Clear thread-local list of LLX results
  unsigned int thread_id = ThreadId();
  thread_llx_results_[thread_id]->clear();
}

template< typename UserData, typename ValuePool >
bool LlxScx<UserData, ValuePool>::TryStoreConditional(
  embb::base::Atomic<cas_t> * cas_field,
  cas_t cas_value,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & finalize_deps) {
  typedef embb::containers::internal::FixedSizeList<DataRecord_t *>
    dr_list_t;
  typedef embb::containers::internal::FixedSizeList<ScxRecord_t *>
    scx_op_list_t;
  typedef embb::containers::internal::FixedSizeList<LlxResult>
    llx_result_list_t;
  // Preconditions:
  // 1. For each r in linked_deps, this thread has performed an invocation
  //    I_r of LLX(r) linked to this SCX.
  // 2. Given value is not initial value of field.
  // 3. For each r in V, no SCX(V',R',field,value) has been linearized before
  //    any I_r was linearized.
  unsigned int thread_id = ThreadId();
  // Let info_fields be a table in shared memory containing for each r in V,
  // a copy of r's info value in this threads local table of LLX results.
  // In brief: A list of the SCX record of all linked deps.
  scx_op_list_t * info_fields = scx_record_list_pool_.Allocate(max_links_);
  if (info_fields == NULL) {    
    EMBB_THROW(embb::base::ErrorException,
      "Could not allocate SCX record list");
  }
  dr_list_t::const_iterator data_record;
  dr_list_t::const_iterator end;
  end = linked_deps.end();
  // Copy SCX operation of all LLX results of link dependencies into a list.
  // For each r in linked_deps ...
  for (data_record = linked_deps.begin(); data_record != end; ++data_record) {
    llx_result_list_t::iterator llx_result_it =
      thread_llx_results_[thread_id]->begin();
    llx_result_list_t::iterator llx_result_end =
      thread_llx_results_[thread_id]->end();
    // Find LLX result of data_record (r) in thread-local LLX results:
    while (llx_result_it != llx_result_end &&
           llx_result_it->data_record != *data_record) {
      ++llx_result_it;
    }
    if (llx_result_it->data_record != *data_record) {
      // Missing LLX result for given linked data record, user did not
      // load-link a data record this SCX depends on.
      EMBB_THROW(embb::base::ErrorException,
        "Missing preceding LLX on a data record used as SCX dependency");
    }
    // Copy SCX operation from LLX result of link dependency into list: 
    assert(info_fields->PushBack(
      llx_result_it->data_record->ScxInfo().Load()));
  }
  // Clear thread-local list of LLX results
  thread_llx_results_[thread_id]->clear();
  // Announce SCX operation. Lists linked_deps and finalize_dep are 
  // guaranteed to remain on the stack until this announced operation
  // is completed, so no allocation/pool is necessary.
  // The SCX operation description must be allocated from a pool as
  // LLX data records might reference it after this operation has been
  // completed.
  ScxRecord_t new_scx(
    linked_deps,
    finalize_deps,
    // target field:
    cas_field,
    // new value:
    cas_value,
    // old value:
    cas_field->Load(),
    // list of the SCX record of all linked deps
    info_fields,
    // initial operation state:
    OperationState::InProgress);
  // Allocate from pool as this operation description is global:
  ScxRecord_t * scx = scx_record_pool_.Allocate(new_scx);
  // Try to complete the operation. It will also be helped in failing
  // TryLoadLinked operations.
  bool result = Help(scx);
  // Release all load-links this SCX operation depended on
  ClearLinks();
  // Release guards, but do not enqueue instance scx for deletion as it
  // is still referenced in data records.
  hp.GuardPointer(0, NULL);
  hp.GuardPointer(1, NULL);;
  return result;
}

template< typename UserData, typename ValuePool >
bool LlxScx<UserData, ValuePool>::TryValidateLink(
  embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps) {
  typedef embb::containers::internal::FixedSizeList<DataRecord_t *>
    dr_list_t;
  typedef embb::containers::internal::FixedSizeList<LlxResult>
    llx_result_list_t;
  unsigned int thread_id = ThreadId();
  // Iterate over given list of data records V:
  dr_list_t::iterator linked_it  = linked_deps.begin();
  dr_list_t::iterator linked_end = linked_deps.end();
  // For each r in V ...
  for (; linked_it != linked_end; ++linked_it) {
    llx_result_list_t::iterator llx_result_it =
      thread_llx_results_[thread_id]->begin();
    llx_result_list_t::iterator llx_result_end =
      thread_llx_results_[thread_id]->end();
    // Find LLX result of data_record (r) in thread-local LLX results:
    while (llx_result_it != llx_result_end &&
           llx_result_it->data_record != *linked_it) {
      ++llx_result_it;
    }
    if (llx_result_it->data_record != *linked_it) {
      // Missing LLX result for given linked data record, user did not
      // load-link a data record this SCX depends on.
      EMBB_THROW(embb::base::ErrorException,
        "Missing preceding LLX on a data record used as SCX dependency");
    }
    if (llx_result_it->scx_record != (*linked_it)->ScxInfo()) {
      return false;
    }
  }
  return true;
}

// ScxRecord

template< typename UserData, typename ValuePool >
bool LlxScx<UserData, ValuePool>::Help(
  ScxRecord_t * scx) {
  hp.GuardPointer(0, scx);
  // We ensure that an SCX S does not change a data record 
  // while it is frozen for another SCX S'. Instead, S uses 
  // the information in the SCX record of S' to help S'
  // complete, so that the data record can be unfrozen.
  typedef embb::containers::internal::FixedSizeList<DataRecord_t *> dr_list_t;
  typedef embb::containers::internal::FixedSizeList<ScxRecord_t *> op_list_t;
  // Freeze all data records in data_records (i.e. reserve them for this 
  // SCX operation) to protect their mutable fields from being changed by 
  // other SCXs:
  dr_list_t::iterator linked_it  = scx->linked_data_records->begin();
  dr_list_t::iterator linked_end = scx->linked_data_records->end();
  op_list_t::iterator scx_op_it  = scx->scx_ops->begin();
  op_list_t::iterator scx_op_end = scx->scx_ops->end();
  for (; linked_it != linked_end && scx_op_it != scx_op_end;
       ++linked_it, ++scx_op_it) {
    DataRecord_t * r = *linked_it;
    ScxRecord<DataRecord_t> * rinfo_old = *scx_op_it;
    hp.GuardPointer(1, rinfo_old);
    // Try to freeze the data record by setting its SCX info field
    // to this SCX operation description.
    // r->ScxInfo() is not an ABA hazard as it is local to instance scx
    // which is already guarded.
    if (r->ScxInfo().CompareAndSwap(rinfo_old, scx)) {
      // Do not try to delete the sentinel scx record:
      if (rinfo_old != &DataRecord_t::DummyScx) {
        hp.EnqueuePointerForDeletion(rinfo_old);
      }
    } else {
      if (r->ScxInfo().Load() != scx) {
        // could not freeze r because it is frozen for another SCX:
        if (scx->all_frozen) {
          // SCX already completed by any other thread:
          return true;
        }
        // Atomically unfreeze all nodes frozen for this SCX (see LLX):
        scx->state = ScxRecord_t::Aborted;
        return false;
      }
    } 
  }
  // finished freezing data records
  assert(scx->state == ScxRecord_t::InProgress ||
         scx->state == ScxRecord_t::Comitted);
  // frozen step:
  scx->all_frozen = true;
  // mark step:
  dr_list_t::iterator finalize_it  = scx->finalize_data_records->begin();
  dr_list_t::iterator finalize_end = scx->finalize_data_records->end();
  for (; finalize_it != finalize_end; ++finalize_it) {
    (*finalize_it)->MarkForFinalize();
  }
  // update CAS:
  cas_t expected_old_value = scx->old_value;
  // scx->old_value_ is not an ABA hazard as it is local to the instance
  // scx which is already guarded.
  scx->field->CompareAndSwap(expected_old_value, scx->new_value);
  // Commit step.
  // Finalizes all r in data_records within finalize range and
  // unfreezes all r in data_records outside of finalize range. 
  // Linearization point of this operation.
  scx->state = ScxRecord_t::Comitted;
  return true;
}

template< typename UserData, typename ValuePool >
void LlxScx<UserData, ValuePool>::DeleteOperationCallback(
  ScxRecord_t * scx_record) {
  scx_record_list_pool_.Free(scx_record->scx_ops);
  scx_record_pool_.Free(scx_record);
}

// LlxScxRecord

template< typename UserData >
LlxScxRecord<UserData>::LlxScxRecord()
: marked_for_finalize_(false) {
  scx_op_.Store(&DummyScx);
}

template< typename UserData >
LlxScxRecord<UserData>::LlxScxRecord(
  const UserData & user_data)
: user_data_(user_data),
  marked_for_finalize_(false) {
  scx_op_.Store(&DummyScx);
}

template< typename UserData >
ScxRecord< LlxScxRecord<UserData> > 
LlxScxRecord<UserData>::DummyScx =
  ScxRecord< LlxScxRecord<UserData> >();

} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_LLX_SCX_INL_H_
