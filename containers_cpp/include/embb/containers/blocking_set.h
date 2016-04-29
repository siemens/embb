/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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

#ifndef EMBB_CONTAINERS_BLOCKING_SET_H_
#define EMBB_CONTAINERS_BLOCKING_SET_H_


#include <embb/base/base.h>
#include <set>


namespace embb {
namespace containers {

/*
 * Blocking set.
 *
 * \tparam Type Element type.
 */
template < typename Type >
class BlockingSet {
  typedef embb::base::Mutex Mutex;
  typedef embb::base::LockGuard<Mutex> LockGuard;

 private:
  /**
   * Internal set from the standard library
   */
  std::set<Type> internalSet;

  /**
   * Mutex for synchronizing the accesses
   * to the structure.
   */
  Mutex mutex;

 public:
  /**
   * Creates an empty set. 
   */
  BlockingSet();

  /**
   * Inserts an element in the set.
   *
   * \return \c true if the element has been inserted (i.e. 
   * it was not already in the set), \c false otherwise.
   */
  bool Insert(
    const Type& element
    /**< [IN] Reference to the element to insert. */);

  /**
   * Erases an element from the set.
   *
   * \return \c true if the element has been erased (which means
   * that it was in the set) \c false otherwise.
   */
  bool Erase(
    const Type& element
    /**< [IN] Reference to the element to erase. */);

  /**
   * Checks if the an element is in the set.
   *
   * \return \c true if the element is in the set,
   * \c false otherwise.
   */
  bool Contains(
    const Type& element
    /**< [IN] Reference to the element to search for. */);
};

}  // namespace containers
}  // namespace embb

#include <embb/containers/internal/blocking_set-inl.h>

#endif  // EMBB_CONTAINERS_BLOCKING_CONTAINER_