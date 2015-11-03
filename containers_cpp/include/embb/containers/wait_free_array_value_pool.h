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

#ifndef EMBB_CONTAINERS_WAIT_FREE_ARRAY_VALUE_POOL_H_
#define EMBB_CONTAINERS_WAIT_FREE_ARRAY_VALUE_POOL_H_

#include <embb/base/atomic.h>
#include <embb/base/memory_allocation.h>

namespace embb {
namespace containers {
/**
 * \defgroup CPP_CONCEPTS_VALUE_POOL Value Pool Concept
 * Concept for thread-safe value pools
 *
 * \ingroup CPP_CONCEPT
 * \{
 * \par Description
 * A value pool is a fixed-size multiset of elements, where each element has a
 * unique index. The elements cannot be modified and are given at construction
 * time (by providing first/last iterators). A value pool provides two
 * operations: \c Allocate and \c Free. \c Allocate removes an element from the
 * pool, and \c Free returns an element to the pool. It is only allowed to
 * free elements that have previously been allocated.
 *
 * \par Requirements
 * - Let \c Pool be the pool class
 * - Let \c Type be the element type of the pool. Atomic operations must be
 *   possible on \c Type.
 * - Let \c b, d be objects of type \c Type
 * - Let \c i, j be forward iterators supporting \c std::distance.
 * - Let \c c be an object of type \c Type&
 * - Let \c e be a value of type \c int
 *
 * \par Valid Expressions
 *
 * <table>
 *   <tr>
 *     <th>Expression</th>
 *     <th>Return type</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} Pool<Type, b>(i, j) \endcode
 *     </td>
 *     <td>Nothing</td>
 *     <td>
 *     Constructs a value pool holding elements of type \c Type, where \c b is
 *     the bottom element. The bottom element cannot be stored in the pool, it
 *     is exclusively used to mark empty cells. The pool initially contains
 *     \c std::distance(i, j) elements which are copied during construction from
 *     the range \c [i, j). A concrete class satisfying the value pool concept
 *     might provide additional template parameters for specifying allocators.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} Allocate(c) \endcode</td>
 *     <td>\c int</td>
 *     <td>
 *     Gets an element from the pool. Returns -1, if no element is available,
 *     i.e., the pool is empty. Otherwise, returns the index of the element in
 *     the pool. The value of the pool element is written into reference \c c.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} Free(d, e) \endcode</td>
 *     <td>\c void</td>
 *     <td>Returns an element \c d to the pool, where \c e is its index. The
 *     values of \c d and \c e have to match the values of the previous call to
 *     \c Allocate. For each allocated element, \c Free must be called exactly
 *     once.</td>
 *   </tr>
 * </table>
 *
 * \}
 */

/**
 * Wait-free value pool using array construction
 *
 * \concept{CPP_CONCEPTS_VALUE_POOL}
 *
 * \ingroup CPP_CONTAINERS_POOLS
 *
 * \see LockFreeTreeValuePool
 *
 * \tparam Type Element type (must support atomic operations such as \c int).
 * \tparam Undefined Bottom element (cannot be stored in the pool)
 * \tparam Allocator Allocator used to allocate the pool array
 */
template<typename Type,
  Type Undefined,
  class Allocator = embb::base::Allocator< embb::base::Atomic<Type> > >
class WaitFreeArrayValuePool {
 private:
  int size;
  embb::base::Atomic<Type>* pool;
  WaitFreeArrayValuePool();
  Allocator allocator;

  // Prevent copy-construction
  WaitFreeArrayValuePool(const WaitFreeArrayValuePool&);

  // Prevent assignment
  WaitFreeArrayValuePool& operator=(const WaitFreeArrayValuePool&);

 public:
  /**
   * Constructs a pool and fills it with the elements in the specified range.
   *
   * \memory Dynamically allocates <tt>n*sizeof(embb::base::Atomic<Type>)</tt>
   *         bytes, where <tt>n = std::distance(first, last)</tt> is the number
   *         of pool elements.
   *
   * \notthreadsafe
   *
   * \see CPP_CONCEPTS_VALUE_POOL
   */
  template<typename ForwardIterator>
  WaitFreeArrayValuePool(
    ForwardIterator first,
    /**< [IN] Iterator pointing to the first element of the range the pool is
              filled with */
    ForwardIterator last
    /**< [IN] Iterator pointing to the last plus one element of the range the
              pool is filled with */
  );

  /**
   * Destructs the pool.
   *
   * \notthreadsafe
   */
  ~WaitFreeArrayValuePool();

  /**
   * Allocates an element from the pool.
   *
   * \return Index of the element if the pool is not empty, otherwise \c -1.
   *
   * \waitfree
   *
   * \see CPP_CONCEPTS_VALUE_POOL
   */
  int Allocate(
    Type & element
    /**< [IN,OUT] Reference to the allocated element. Unchanged, if the
                  operation was not successful. */
  );

  /**
   * Returns an element to the pool.
   *
   * \note The element must have been allocated with Allocate().
   * 
   * \waitfree
   *
   * \see CPP_CONCEPTS_VALUE_POOL
   */
  void Free(
    Type element,
    /**< [IN] Element to be returned to the pool */
    int index
    /**< [IN] Index of the element as obtained by Allocate() */
  );
};
} // namespace containers
} // namespace embb

#include <embb/containers/internal/wait_free_array_value_pool-inl.h>

#endif  // EMBB_CONTAINERS_WAIT_FREE_ARRAY_VALUE_POOL_H_
