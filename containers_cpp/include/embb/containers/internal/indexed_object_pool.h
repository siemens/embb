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

#ifndef EMBB_CONTAINERS_INTERNAL_INDEXED_OBJECT_POOL_H_
#define EMBB_CONTAINERS_INTERNAL_INDEXED_OBJECT_POOL_H_

#include <embb/base/atomic.h>
#include <embb/base/memory_allocation.h>

#include <embb/containers/lock_free_tree_value_pool.h>
#include <embb/containers/wait_free_array_value_pool.h>

namespace embb {
namespace containers {
namespace internal {

template<
  typename T,
  class IndexPool = LockFreeTreeValuePool<bool, false>,
  class Allocator = embb::base::Allocator<T>
>
class IndexedObjectPool {
 private:
  const size_t size_;
  T *          elements;
  Allocator    allocator;
  IndexPool    indexPool;
  IndexedObjectPool();
  // Prevent copy-construction
  IndexedObjectPool(const IndexedObjectPool&);
  // Prevent assignment
  IndexedObjectPool& operator=(const IndexedObjectPool&);

 public:
  /**
   * \see value_pool_concept
   *
   * \notthreadsafe
   *
   * \memory dynamically allocates
   *         \c n * (sizeof(T) + sizeof(embb::base::Atomic<bool>))
   *         bytes, where \c n is the number of elements in the pool.
   */
  template<typename RAI>
  IndexedObjectPool(
    RAI first,
     /**< [IN] first iterator to elements the pool is filled with */
    RAI last
     /**< [IN] last iterator to elements the pool is filled with */
    );

  /**
   * \see value_pool_concept
   *
   * \notthreadsafe
   *
   * \memory dynamically allocates
   *         \c n * (sizeof(T) + sizeof(embb::base::Atomic<bool>))
   *         bytes, where \c n is the number of elements in the pool.
   */
  IndexedObjectPool(
    size_t size,
     /**< [IN] Number of elements the pool is filled with */
    const T & defaultInstance
     /**< [IN] Default instance to initialize pool elements with */
    );

  /**
   * Destructor, deallocating memory
   */
  ~IndexedObjectPool();

  /**
   * Request element and index from pool.
   *
   * \return index of element
   *
   * \see object_pool_concept
   *
   */
  int Allocate(T & element);

  /**
   * Return element and index to the pool.
   *
   * \see value_pool_concept
   *
   */
  void Free(int elementIndex);

  /**
   * Return element from the pool at given index.
   *
   * \see object_pool_concept
   *
   */
  T & operator[](size_t elementIndex);
};

}  // namespace internal
}  // namespace containers
}  // namespace embb

#include <embb/containers/internal/indexed_object_pool-inl.h>

#endif  // EMBB_CONTAINERS_INTERNAL_INDEXED_OBJECT_POOL_H_

