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

#ifndef EMBB_CONTAINERS_WAIT_FREE_ARRAY_VALUE_POOL_H_
#define EMBB_CONTAINERS_WAIT_FREE_ARRAY_VALUE_POOL_H_

#include <embb/base/atomic.h>
#include <embb/base/memory_allocation.h>
#include <iterator>
#include <utility>

namespace embb {
namespace containers {
/**
 * \defgroup CPP_CONCEPTS_VALUE_POOL Value Pool Concept
 * Concept for thread-safe value pools
 *
 * \ingroup CPP_CONCEPT
 * \{
 * \par Description
 * A value pool is a multi-set of elements, where each element has a unique,
 * continuous (starting with 0) index. The elements cannot be modified and are
 * given at construction time by providing first/last iterators.
 *
 * \par
 * A value pool provides two primary operations: \c Allocate and \c Free. \c
 * Allocate allocates an element/index "pair" (index via return, element via
 * reference parameter) from the pool, and \c Free returns an element/index pair
 * to the pool. To guarantee linearizability, \c element is not allowed to be
 * modified between \c Allocate and \c Free. It is only allowed to free elements
 * that have previously been allocated. The \c Allocate function does not
 * guarantee an order on which indices are allocated. The count of elements that
 * can be allocated with \c Allocate might be smaller than the count of
 * elements, the pool is initialized with. This might be because of
 * implementation details and respective concurrency effects: for example, if
 * indices are managed within a queue, one has to protect queue elements from
 * concurrency effects (reuse and access). As long as a thread potentially
 * accesses a node (and with that an index), the respective index cannot not be
 * given out to the user, even if being logically not part of the pool anymore.
 * However, the user might want to guarantee a certain amount of indices to the
 * user. Therefore, the static \c GetMinimumElementCountForGuaranteedCapacity
 * method is used. The user passes the count of indices to this method that
 * shall be guaranteed by the pool. The method returns the count on indices, the
 * pool has to be initialized with in order to guarantee this count on indices.
 *
 * \par Requirements
 * - Let \c Pool be the pool class
 * - Let \c Type be the element type of the pool. Atomic operations must be
 *   possible on \c Type.
 * - Let \c b, d be objects of type \c Type
 * - Let \c i, j be forward iterators supporting \c std::distance.
 * - Let \c c be an object of type \c Type&
 * - Let \c e be a value of type \c int
 * - Let \c f be a value of type \c int
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
 *     the range \c [i, j]. A concrete class satisfying the value pool concept
 *     might provide additional template parameters for specifying allocators.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} Allocate(c) \endcode</td>
 *     <td>\c int</td>
 *     <td>
 *     Allocates an element/index "pair" from the pool. Returns -1, if no
 *     element is available, i.e., the pool is empty. Otherwise, returns the
 *     index of the element in the pool. The value of the pool element is
 *     written into parameter reference \c c.
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
 *   <tr>
 *     <td>\code{.cpp} GetMinimumElementCountForGuaranteedCapacity(f)
 *     \endcode</td>
 *     <td>\c void</td>
 *     <td>Static method, returns the count of indices, the user has to
 *     initialize the pool with in order to guarantee a count of \c f elements
 *     (irrespective of concurrency effects).
 *      </td>
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
  int size_;
  embb::base::Atomic<Type>* pool_array_;
  WaitFreeArrayValuePool();
  Allocator allocator_;

  // Prevent copy-construction
  WaitFreeArrayValuePool(const WaitFreeArrayValuePool&);

  // Prevent assignment
  WaitFreeArrayValuePool& operator=(const WaitFreeArrayValuePool&);

 public:
  /**
   * Forward iterator to iterate over the allocated elements of the pool.
   * \note Iterators are invalidated by any change to the pool
   *       (Allocate and Free calls).
   */
  class Iterator : public std::iterator<
    std::forward_iterator_tag, std::pair<int, Type> > {
   private:
    explicit Iterator(WaitFreeArrayValuePool * pool);
    Iterator(WaitFreeArrayValuePool * pool, int index);
    void Advance();

    WaitFreeArrayValuePool * pool_;
    int index_;

    friend class WaitFreeArrayValuePool;

   public:
    /**
     * Constructs an invalid iterator.
     * \waitfree
     */
    Iterator();

    /**
     * Copies an iterator.
     * \waitfree
     */
    Iterator(
      Iterator const & other           /**< [IN] Iterator to copy. */
    );

    /**
     * Copies an iterator.
     *
     * \returns Reference to this iterator.
     * \waitfree
     */
    Iterator & operator =(
      Iterator const & other           /**< [IN] Iterator to copy. */
    );

    /**
     * Pre-increments an iterator.
     *
     * \returns Reference to this iterator.
     * \notthreadsafe
     */
    Iterator & operator ++();

    /**
     * Post-increments an iterator.
     *
     * \returns Copy of this iterator before increment.
     * \notthreadsafe
     */
    Iterator operator ++(int);

    /**
     * Compares two iterators for equality.
     *
     * \returns \c true, if the two iterators are equal,
     *          \c false otherwise.
     * \waitfree
     */
    bool operator ==(
      Iterator const & rhs             /**< [IN] Iterator to compare to. */
    );

    /**
     * Compares two iterators for inequality.
     *
     * \returns \c true, if the two iterators are not equal,
     *          \c false otherwise.
     * \waitfree
     */
    bool operator !=(
      Iterator const & rhs             /**< [IN] Iterator to compare to. */
    );

    /**
     * Dereferences the iterator.
     *
     * \returns A pair consisting of index and value of the element pointed to.
     * \waitfree
     */
    std::pair<int, Type> operator *();
  };

  /**
   * Gets a forward iterator to the first allocated element in the pool.
   *
   * \returns a forward iterator pointing to the first allocated element.
   * \waitfree
   */
  Iterator Begin();

  /**
   * Gets a forward iterator pointing after the last allocated element in
   * the pool.
   *
   * \returns a forward iterator pointing after the last allocated element.
   * \waitfree
   */
  Iterator End();

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
   * Due to concurrency effects, a pool might provide less elements than managed
   * by it. However, usually one wants to guarantee a minimal capacity. The
   * count of elements that must be given to the pool when to guarantee \c
   * capacity elements is computed using this function.
   *
   * \return count of indices the pool has to be initialized with
   */
  static size_t GetMinimumElementCountForGuaranteedCapacity(
    size_t capacity
    /**< [IN] count of indices that shall be guaranteed */);

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
