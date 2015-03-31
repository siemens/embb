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

#ifndef EMBB_CONTAINERS_INTERNAL_MULTIWORD_LL_SC_INL_H_
#define EMBB_CONTAINERS_INTERNAL_MULTIWORD_LL_SC_INL_H_

#include <embb/containers/internal/multiword_ll_sc-inl.h>
#include <embb/containers/internal/fixed_size_list.h>
#include <embb/base/thread.h>
#include <embb/base/atomic.h>

namespace embb { 
namespace containers {
namespace internal {

template< typename UserData, typename ValuePool >
MultiwordLLSC<UserData, ValuePool>::MultiwordLLSC(size_t max_links)
: llx_scx_(max_links) {
}

template< typename UserData, typename ValuePool >
MultiwordLLSC<UserData, ValuePool>::~MultiwordLLSC() {
}

template< typename UserData, typename ValuePool >
bool MultiwordLLSC<UserData, ValuePool>::TryLoadLinked(
  DataRecord_t * const data_record,
  UserData & user_data,
  bool & finalized) {
  return llx_scx_.TryLoadLinked(data_record, user_data, finalized);
}

template< typename UserData, typename ValuePool >
bool MultiwordLLSC<UserData, ValuePool>::TryLoadLinked(
  DataRecord_t * const data_record,
  UserData & user_data) {
  bool finalized;
  return llx_scx_.TryLoadLinked(data_record, user_data, finalized);
}

template< typename UserData, typename ValuePool >
template< typename FieldType >
bool MultiwordLLSC<UserData, ValuePool>::TryStoreConditional(
  Link & linked_dep,
  embb::base::Atomic<FieldType> * field,
  FieldType value) {
  embb::containers::internal::FixedSizeList<Link *> linked_deps(1);
  linked_deps.PushBack(&linked_dep);
  embb::containers::internal::FixedSizeList<Link *> finalize_deps(0);
  embb::base::Atomic<cas_t> * cas_field = 
    reinterpret_cast<embb::base::Atomic<cas_t> *>(field);
  cas_t cas_value = ToCASValue(value);
  return llx_scx_.TryStoreConditional(
    cas_field, cas_value, linked_deps, finalize_deps);
}

template< typename UserData, typename ValuePool >
template< typename FieldType >
bool MultiwordLLSC<UserData, ValuePool>::TryStoreConditional(
  Link & linked_dep_1,
  Link & linked_dep_2,
  embb::base::Atomic<FieldType> * field,
  FieldType value) {
  embb::containers::internal::FixedSizeList<Link *> linked_deps(2);
  linked_deps.PushBack(&linked_dep_1);
  linked_deps.PushBack(&linked_dep_2);
  embb::containers::internal::FixedSizeList<Link *> finalize_deps(0);
  embb::base::Atomic<cas_t> * cas_field = 
    reinterpret_cast<embb::base::Atomic<cas_t> *>(field);
  cas_t cas_value = ToCASValue(value);
  return llx_scx_.TryStoreConditional(
    cas_field, cas_value, linked_deps, finalize_deps);
}

#if 0

template< typename UserData, typename ValuePool >
template< typename FieldType >
bool MultiwordLLSC<UserData, ValuePool>::TryStoreConditional(
  embb::base::Atomic<FieldType> * field,
  FieldType value,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps) {
  embb::containers::internal::FixedSizeList<DataRecord_t *> finalize_deps(0);
  return TryStoreConditional(field, value, linked_deps, finalize_deps);
}

template< typename UserData, typename ValuePool >
template< typename FieldType >
bool MultiwordLLSC<UserData, ValuePool>::TryStoreConditional(
  embb::base::Atomic<FieldType*> * field,
  FieldType * value,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & finalize_deps) {
  embb::base::Atomic<cas_t> * cas_field =
    reinterpret_cast<embb::base::Atomic<cas_t> *>(field);
  cas_t cas_value = reinterpret_cast<cas_t>(value);
  return llx_scx_.TryStoreConditional(cas_field, cas_value, linked_deps, finalize_deps);
}

template< typename UserData, typename ValuePool >
template< typename FieldType >
bool MultiwordLLSC<UserData, ValuePool>::TryStoreConditional(
  embb::base::Atomic<FieldType*> * field,
  FieldType * value,
  embb::containers::internal::FixedSizeList<DataRecord_t *> & linked_deps) {
  embb::containers::internal::FixedSizeList<DataRecord_t *> finalize_deps(0);
  return TryStoreConditional(field, value, linked_deps, finalize_deps);
}

#endif

template< typename UserData, typename ValuePool >
bool MultiwordLLSC<UserData, ValuePool>::TryValidateLink(
  const DataRecord_t & field) {
  return llx_scx_.TryValidateLink(field);
}

} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_MULTIWORD_LL_SC_INL_H_
