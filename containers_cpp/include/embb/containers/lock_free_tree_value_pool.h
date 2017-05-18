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

#ifndef EMBB_CONTAINERS_LOCK_FREE_TREE_VALUE_POOL_H_
#define EMBB_CONTAINERS_LOCK_FREE_TREE_VALUE_POOL_H_

#include <embb/base/atomic.h>
#include <embb/base/memory_allocation.h>
#include <iterator>
#include <utility>

namespace embb {
namespace containers {
/**
 * Lock-free value pool using binary tree construction.
 *
 * \concept{CPP_CONCEPTS_VALUE_POOL}
 *
 * \ingroup CPP_CONTAINERS_POOLS
 *
 * \see WaitFreeArrayValuePool
 *
 * \tparam Type Element type (must support atomic operations such as \c int).
 * \tparam Undefined Bottom element (cannot be stored in the pool)
 * \tparam PoolAllocator Allocator used to allocate the pool array
 * \tparam TreeAllocator Allocator used to allocate the array representing the
 *         binary tree.
 */
template<typename Type,
  Type Undefined,
class PoolAllocator = embb::base::Allocator< embb::base::Atomic<Type> >,
class TreeAllocator = embb::base::Allocator < embb::base::Atomic<int> >
>
class LockFreeTreeValuePool {
  /*
   * Description of the algorithm:
   *
   * The binary tree is split into two parts, the leaves and the binary tree
   * above the leaves. Example:
   *
   *                   b
   *               b       b
   *             b   b   b   b
   *            l l l l l l l l
   *
   * The elements b are the elements "above", the leaves l are the pool
   * elements. The elements b are represented by the array tree, the elements l
   * be the array pool.
   *
   * A binary tree for storing n elements has k = 2^j leaves, where j is the
   * smallest number such that n <= 2^j holds. The variable with name size
   * stores k. The variable tree_size holds the number of elements b, the tree
   * above. It is size - 1. The array pool is not constructed with size k, but
   * with the real size n, as only the first n elements from the pool array are
   * accessed. The tree is used as if elements l and b form a binary tree
   * together and would be stored in one array. That makes the algorithm
   * easier.
   *
   * The elements b store the number of not allocated cells below. The pool
   * elements l are either not allocated, and store the respective element, or
   * are allocated and contain the element Undefined.
   *
   * A tree storing the elements a,b,c,d,e would therefore look like this:
   *
   *                   8
   *               4       1
   *             2   2   1   0
   *  pool[]:   a b c d e
   *
   * tree = {8,4,1,2,2,1,0}
   * pool = {'a','b','c','d','e'}
   * size = 8
   * tree_size = 7
   *
   * The algorithm for allocating an element starts at the root node and
   * recursively traverses the tree. It tries to decrement a node (a decrement
   * is actually a conditional decrement, i.e., a node is decremented if the
   * result is not less than 0. This is the place, where the algorithm is not
   * wait-free anymore, as this cannot be implemented atomically.) and if
   * successful, calls itself on the left child, if not successful, on the right
   * child. If the algorithm encounters a leaf, it tries to reserve it by doing
   * a CAS to Undefined. If that is successful, the element together with its
   * pool index are returned. Otherwise, no element is returned.
   *
   * The algorithm for freeing an element is much more simple. The element is
   * stored at the pool position it was allocated from and then, the algorithm
   * walks the tree towards the root, thereby increasing the value of each
   * visited node.
   *
   * For future work, the memory consumption could be further reduced by
   * "stripping" away unused cells in the binary tree. In the example above,
   * that would be cell 0 in the row "2   2   1   0".
   */
 private:
  // Private constructor
  LockFreeTreeValuePool();

  // Prevent copy-construction
  LockFreeTreeValuePool(const LockFreeTreeValuePool&);

  // Prevent assignment
  LockFreeTreeValuePool& operator=(const LockFreeTreeValuePool&);

  // See algorithm description above
  int size_;

  // See algorithm description above
  int tree_size_;

  // See algorithm description above
  int real_size_;

  // The tree above the pool
  embb::base::Atomic<int>* tree_;

  // The actual pool
  embb::base::Atomic<Type>* pool_;

  // respective allocator
  PoolAllocator pool_allocator_;

  // respective allocator
  TreeAllocator tree_allocator_;

  /**
   * Computes smallest power of two fitting the specified value
   *
   * \return Let k be the smallest number so that value <= 2^k. Return 2^k.
   */
  static int GetSmallestPowerByTwoValue(int value);

  /**
   * Checks if a given node is a leaf
   *
   * \return \c true if node is a leaf, otherwise \c false
   */
  bool IsLeaf(
    int node
    /**< [IN] Node index */
  );

  /**
   * Checks if node is valid
   *
   * \return \c true if node is valid, otherwise \c false.
   */
  bool IsValid(
    int node
    /**< [IN] Node index */
  );

  /**
   * Gets the index of the left child.
   *
   * \pre The node has to be valid
   * \return Index of the left child
   */
  int GetLeftChildIndex(
    int node
    /**< [IN] Node index */
  );

  /**
   * Gets the index of the right child.
   *
   * \pre The node has to be valid
   * \return Index of the right child
   */
  int GetRightChildIndex(
    int node
    /**< [IN] Node index */
  );

  /**
   * The leaves represent the pool. They are indexed from 0 to pool_size. If
   * a node is a leaf, this index is returned.
   */
  int NodeIndexToPoolIndex(
    int node
    /**< [IN] Node index */
  );

  /**
   * Gets the leaf node index for a pool index.
   * Inverse function for NodeIndexToPoolIndex.
   */
  int PoolIndexToNodeIndex(int index);

  /**
   * Checks if node index is root
   *
   * \return \c true if node index is root element, otherwise \c false
   */
  bool IsRoot(
    int node
    /**< [IN] Node index */
  );

  /**
   * Get the parent node.
   *
   * \return Index of parent node
   */
  int GetParentNode(
    int node
    /**< [IN] Node index */
  );

  /**
   * Tries to allocate an element starting from node \c node. If successful,
   * the index of the node is returned and the allocated element is written to
   * \c element. If the allocation is not successful, -1 is returned.
   *
   * \return Index of allocated element, or -1 if not successful.
   *
   */
  int allocate_rec(
    int node,
    /**< [IN] Node index */
    Type& element
    /**< [IN,OUT] Allocated element, if there is any */
  );

  /**
   * Instead of "freeing" each element for initializing the tree, we do this in
   * one pass, as this is much faster. Supposed to be called recursively from
   * the root node. Does not initialize the pool elements, only the managing
   * binary tree.
   */
  void Fill(
    int node,
    /**< [IN] Node index */
    int elementsToStore,
    /**< [IN] Number of elements to be stored in the current branch */
    int power2Value
    /**< [IN] Maximum number of elements this branch can hold */
  );

 public:
  /**
   * Forward iterator to iterate over the allocated elements of the pool.
   * \note Iterators are invalidated by any change to the pool
   *       (Allocate and Free calls).
   */
  class Iterator : public std::iterator<
    std::forward_iterator_tag, std::pair<int, Type> > {
   private:
    explicit Iterator(LockFreeTreeValuePool * pool);
    Iterator(LockFreeTreeValuePool * pool, int index);
    void Advance();

    LockFreeTreeValuePool * pool_;
    int index_;

    friend class LockFreeTreeValuePool;

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
   * \memory Let <tt>n = \c std::distance(first, last))</tt> and \c k be the
   * minimum number such that <tt>n <= 2^k holds</tt>. Then,
   * <tt>((2^k)-1) * sizeof(embb::Atomic<int>) +
   * n*sizeof(embb::Atomic<Type>)</tt> bytes of memory are allocated.
   *
   * \notthreadsafe
   *
   * \see CPP_CONCEPTS_VALUE_POOL
   */
  template<typename ForwardIterator>
  LockFreeTreeValuePool(
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
  ~LockFreeTreeValuePool();

  /**
   * Allocates an element from the pool.
   *
   * \return Index of the element if the pool is not empty, otherwise \c -1.
   *
   * \lockfree
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
   * \lockfree
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

#include <embb/containers/internal/lock_free_tree_value_pool-inl.h>

#endif  // EMBB_CONTAINERS_LOCK_FREE_TREE_VALUE_POOL_H_
