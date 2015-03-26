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

#ifndef EMBB_CONTAINERS_MULTISET_H_
#define EMBB_CONTAINERS_MULTISET_H_

#include <embb/base/atomic.h>
#include <embb/containers/wait_free_array_value_pool.h>
#include <embb/containers/lock_free_tree_value_pool.h>
#include <embb/containers/object_pool.h>
#include <embb/containers/primitives/llx_scx.h>
#include <embb/containers/internal/fixed_size_list.h>

namespace embb {
namespace containers {

namespace internal {

template< typename Type >
class MultisetNode {
 private:
  typedef primitives::LlxScxRecord< internal::MultisetNode<Type> > *
    node_ptr_t;

 public:
  /**
   * Default constructor, creates an empty node
   */
  inline MultisetNode();

  /**
   * Constructor, creates a multiset entry as node in a linked list
   */
  inline MultisetNode(const Type & key, size_t count, node_ptr_t next);
  
  /**
   * Copy constructor
   */
  inline MultisetNode(const MultisetNode<Type> & other);

  /**
   * Assignment operator
   */
  inline MultisetNode<Type> & operator=(MultisetNode<Type> & rhs);

  /**
   * Returns the key value contained in this multiset element
   */
  inline Type & Key() const;

  /**
   * Returns the count number of this multiset element
   */
  inline embb::base::Atomic<size_t> * Count();

  /**
   * Returns a CAS-enabled pointer to this element's successor element
   */
  inline embb::base::Atomic<node_ptr_t> * Next();

 private:
  Type key_;
  embb::base::Atomic<size_t> count_;
  embb::base::Atomic<node_ptr_t> next_;
};

}  // namespace internal

template<
  typename Type,
  Type UndefinedKey,
  typename ValuePool = embb::containers::LockFreeTreeValuePool< bool, false >
>
class Multiset {

 private:
  typedef primitives::LlxScxRecord< internal::MultisetNode<Type> > node_t;
  typedef primitives::LlxScxRecord< internal::MultisetNode<Type> > * node_ptr_t;
  static const node_ptr_t UNDEFINED_POINTER;
  static const size_t NUM_LLX_SCX_LINKS = 3;

 public:
  /**
   * Constructor, creates a multiset for a maximum number \c capacity of
   * elements.
   */
  Multiset(size_t capacity);

  /**
   * Returns the number of occurrences of the given key in the multiset
   *
   * \returns The number of occurrences of the key, or 0 if the key is not
   *          contained in the multiset
   */
  size_t Get(
    Type key
    ///< [IN] The element key to search in the multiset
  );

  /**
   * Adds an element \c key with \c count occurrences to the multiset.
   */
  void Insert(
    Type key,
    ///< [IN] The element key to add to the multiset */
    size_t count
    ///< [IN] The amount of occurrences of the key to add */
  );

  /**
   * Tentatively removes \c count occurrences of element \c key from the
   * multiset
   *
   * \returns True if the given amount of occurrences have been removed
   *          successfully, or false if the multiset did not contain the
   *          given occurrences of \c key
   */
  bool TryDelete(
    Type key,
    ///< [IN] The element key to remove from the multiset */
    size_t count
    ///< [IN] The amount of occurrences to remove */
  );

 private:
  bool Search(
    const Type key,
    ///< [IN] The element key to search in the multiset
    node_ptr_t & node,
    ///< [OUT] The node in the multiset representing the given key
    node_ptr_t & next
    ///< [OUT] The next pointer of the node in the multiset representing the given key
  );

 private:
  /**
   * Prevent default construction
   */
  Multiset();

  ObjectPool< node_t, ValuePool > node_pool_;
  node_ptr_t head_;
  node_ptr_t tail_;
  primitives::LlxScx< internal::MultisetNode<Type> > llx_scx_;
};

} // namespace containers
} // namespace embb

#include <embb/containers/internal/multiset-inl.h>

#endif  // EMBB_CONTAINERS_MULTISET_H_
