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

#ifndef EMBB_CONTAINERS_INTERNAL_MULTISET_INL_H_
#define EMBB_CONTAINERS_INTERNAL_MULTISET_INL_H_

#include <embb/containers/multiset.h>
#include <embb/base/atomic.h>
#include <embb/containers/primitives/llx_scx.h>

namespace embb {
namespace containers {

namespace internal {

template< typename Type >
inline MultisetNode<Type>::MultisetNode() :
  key_() {
  count_.Store(0);
  next_.Store(0);
}

template< typename Type >
inline MultisetNode<Type>::MultisetNode(
  const Type & key, size_t count, node_ptr_t next) :
  key_(key), count_(count), next_(next) {
}

template< typename Type >
inline MultisetNode<Type>::MultisetNode(
  const MultisetNode<Type> & other) :
  key_(other.key_) {
  count_.Store(other.count_.Load());
  next_.Store(other.next_.Load());
}

template< typename Type >
inline MultisetNode<Type> & MultisetNode<Type>::operator=(
  MultisetNode<Type> & rhs) {
  key_ = rhs.key_;
  count_.Store(rhs.count_.Load());
  next_.Store(rhs.next_.Load());
}

template< typename Type >
inline Type & MultisetNode<Type>::Key() const {
  return key_;
}

template< typename Type >
inline embb::base::Atomic<size_t> * MultisetNode<Type>::Count() {
  return &count_;
}

template< typename Type >
inline embb::base::Atomic<
  primitives::LlxScxRecord< internal::MultisetNode<Type> > *
> * MultisetNode<Type>::Next() {
  return &next_;
}

}  // namespace internal

template< typename Type, Type UndefinedKey, class ValuePool >
Multiset<Type, UndefinedKey, ValuePool>::Multiset(size_t capacity) :
  node_pool_(capacity),
  llx_scx_(NUM_LLX_SCX_LINKS) {
  internal::MultisetNode<Type> sentinel_node(UndefinedKey, 0, UNDEFINED_POINTER);
  head_ = node_pool_.Allocate(sentinel_node);
  tail_ = node_pool_.Allocate(sentinel_node);
  (*head_)->Next()->Store(tail_);
  (*tail_)->Next()->Store(UNDEFINED_POINTER);
}

template< typename Type, Type UndefinedKey, class ValuePool >
bool Multiset<Type, UndefinedKey, ValuePool>::Search(
  const Type key,
  primitives::LlxScxRecord< internal::MultisetNode<Type> > * & node,
  primitives::LlxScxRecord< internal::MultisetNode<Type> > * & next) {
  node = head_;
  next = tail_;
  while (key > next->Key() && next->Key() != UndefinedKey) {
    node = next;
    next = next->Next();
  }
  return (next->Key() != UndefinedKey);
}

template< typename Type, Type UndefinedKey, class ValuePool >
size_t Multiset<Type, UndefinedKey, ValuePool>::Get(Type key) {
  node_ptr_t node, next;
  if (Search(key, node, next)) {
    return (*next)->Count()->Load();
  }
  return 0;
}

template< typename Type, Type UndefinedKey, class ValuePool >
void Multiset<Type, UndefinedKey, ValuePool>::Insert(Type key, size_t count) {
  if (count == 0) {
    return;
  }
  for (;;) {
    node_ptr_t node, next, local_next;
    Search(key, node, next);
    // Key value is stored in node->next as head node
    // is sentinel.
    if (key == (*next)->Key()) {
      node_ptr_t local_next;
      // Key already present in multiset, increment count of its node:
      // LLX(r:next)
      if (llx_scx_.TryLoadLinked(next, *local_next)) {
        internal::FixedSizeList<node_ptr_t> linked_deps(1);
        linked_deps.PushBack(next);
        // SCX(fld:next->count, value:next->count + count, V:<next>);
        if (llx_scx_.TryStoreConditional(
          (*next)->Count(),
          (*local_next)->Count()->Load() + count,
          linked_deps)) {
          return;
        }
      }
    } else {
      // Key not present in multiset yet, add node:
      node_ptr_t local_node;
      if (llx_scx_.TryLoadLinked(node, local_node) && next == (*local_node)->Next()) {
        internal::FixedSizeList<node_ptr_t> linked_deps(1);
        linked_deps.PushBack(node);
        node_t new_node(internal::MultisetNode<Type>(key, count, next));
        node_ptr_t new_node_ptr = node_pool_.Allocate(new_node);
        if (llx_scx_.TryStoreConditional(node->Next(), new_node_ptr, linked_deps)) {
          return;
        } else {
          // Insert failed, return new node object to pool:
          node_pool_.Free(new_node_ptr);
        }
      }
    }
  }
}

template< typename Type, Type UndefinedKey, class ValuePool >
bool Multiset<Type, UndefinedKey, ValuePool>::TryDelete(Type key, size_t count) {
  if (count == 0) {
    return false;
  }
  for (;;) {
    node_ptr_t node, next, local_node, local_next;
    Search(key, node, next);
    if (llx_scx_.TryLoadLinked(node, local_node) &&
      llx_scx_.TryLoadLinked(next, local_next) &&
      next == local_node->Next()) {
      if (key != next->Key() || local_next->Count() < count) {
        // Key not present in multiset or existing count less
        // than amount of elements requested for deletion:
        return false;
      } else if (local_next->Count() > count) {
        // Inserting a new node with decremented count instead
        // of decrementing the exising node's count to reduce
        // number of linked dependencies in SCX.
        internal::MultisetNode<Type> new_node(key, count, next);
        node_ptr_t new_node_ptr = node_pool_.Allocate(new_node);
        internal::FixedSizeList<node_ptr_t> linked_deps(1);
        linked_deps.PushBack(node);
        internal::FixedSizeList<node_ptr_t> finalize_deps(1);
        finalize_deps.PushBack(next);
        if (llx_scx_.TryStoreConditional(
          node->Next(), new_node_ptr, linked_deps, finalize_deps)) {
          return true;
        }
      } else {
        assert(local_next->Count() == count);
        node_ptr_t local_next_next;
        if (llx_scx_.TryLoadLinked(next->Next(), local_next_next)) {
          node_ptr_t new_next_next_ptr = node_pool_.Allocate(*local_next_next);
          internal::FixedSizeList<node_ptr_t> linked_deps(3);
          linked_deps.PushBack(node);
          linked_deps.PushBack(next);
          linked_deps.PushBack(local_next_next);
          internal::FixedSizeList<node_ptr_t> finalize_deps(2);
          finalize_deps.PushBack(next);
          finalize_deps.PushBack(local_next_next);
          if (llx_scx_.TryStoreConditional(
            node->Next(), new_next_next_ptr, linked_deps, finalize_deps)) {
            return true;
          }
          else {
            // Delete failed, return new node object to pool and retry:
            node_pool_.Free(new_next_next_ptr);
          }
        }
      }
    }
  }
}

} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_MULTISET_INL_H_