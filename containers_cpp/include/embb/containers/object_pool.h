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

#ifndef EMBB_CONTAINERS_OBJECT_POOL_H_
#define EMBB_CONTAINERS_OBJECT_POOL_H_

#include <embb/base/atomic.h>
#include <embb/containers/wait_free_array_value_pool.h>

#include <limits>
#include <stdexcept>

namespace embb {
namespace containers {
/**
 * \defgroup CPP_CONTAINERS_POOLS Pools
 * Concurrent pools
 *
 * \ingroup CPP_CONTAINERS
 */

/**
 * Pool for thread-safe management of arbitrary objects.
 *
 * \ingroup CPP_CONTAINERS_POOLS
 *
 * \tparam Type Element type
 * \tparam ValuePool Type of the underlying value pool, determines whether
 *         the object pool is wait-free or lock-free
 * \tparam ObjectAllocator Type of allocator used to allocate objects
 */
template<class Type,
  typename ValuePool    =
    embb::containers::WaitFreeArrayValuePool< bool, false >,
  class ObjectAllocator = embb::base::Allocator<Type> >
class ObjectPool {
 private:
  /**
   * Allocator used to allocate elements of the object pool
   */
  ObjectAllocator object_allocator_;

  /**
   * Capacity of the object pool
   */
  size_t capacity_;

  /**
   * The size of the underlying value pool. This is also the size of the object
   * array in this class. It is assumed that the valuepool manages indices in
   * range [0;value_pool_size_-1].
   */
  size_t value_pool_size_;

  /**
   * Underlying value pool
   */
  ValuePool value_pool_;

  /**
   * Array holding the allocated object
   */
  Type* objects_array_;

  /**
   * Helper providing a virtual iterator that just returns true in each
   * iteration step. Used for filling the value pool. Implements the normal
   * C++ iterator concept. Not further documented here.
   */
  class ReturningTrueIterator {
   public:
    typedef ReturningTrueIterator self_type;
    typedef bool value_type;
    typedef bool& reference;
    typedef bool* pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    explicit ReturningTrueIterator(size_t count_value);
    self_type operator++();
    self_type operator++(int);
    reference operator*();
    pointer operator->();
    bool operator==(const self_type& rhs);
    bool operator!=(const self_type& rhs);

   private:
    size_t count_value;
    bool ret_value;
  };

  bool IsContained(const Type &obj) const;
  int GetIndexOfObject(const Type &obj) const;
  Type* AllocateRaw();

 public:
  /**
   * Constructs an object pool with capacity \c capacity.
   *
   * \memory Allocates \c capacity elements of type \c Type.
   *
   * \notthreadsafe
   */
  ObjectPool(
    size_t capacity
    /**< [IN] Number of elements the pool can hold */
  );

  /**
   * Destructs the pool.
   *
   * \notthreadsafe
   */
  ~ObjectPool();

  /**
   * Returns the capacity of the pool.
   *
   * \return Number of elements the pool can hold.
   *
   * \waitfree
   */
  size_t GetCapacity();

 /**
  * Returns an element to the pool.
  *
  * If the underlying value pool is wait-free/lock-free, this operation is
  * also wait-free/lock-free, respectively.
  *
  * \note The element must have been allocated with Allocate().
  */
  void Free(
    Type* obj
    /**< [IN] Pointer to the object to be freed */
  );

#ifdef DOXYGEN
  /**
   * Allocates an element from the pool.
   *
   * If the underlying value pool is wait-free/lock-free, this operation is
   * also wait-free/lock-free, respectively.
   *
   * \return Pointer to the allocated object if successful, otherwise \c NULL.
   *
   * \param ... Arguments of arbitrary type, passed to the object's constructor
   */
  Type* Allocate(...);
#else
  Type* Allocate();

  template<typename Param1>
  Type* Allocate(Param1 const& param1);

  template<typename Param1, typename Param2>
  Type* Allocate(Param1 const& param1, Param2 const& param2);

  template<typename Param1, typename Param2, typename Param3>
  Type* Allocate(Param1 const& param1, Param2 const& param2,
    Param3 const& param3);

  template<typename Param1, typename Param2, typename Param3, typename Param4>
  Type* Allocate(Param1 const& param1, Param2 const& param2,
    Param3 const& param3, Param4 const& param4);

#endif
};
} // namespace containers
} // namespace embb

#include <embb/containers/internal/object_pool-inl.h>

#endif  // EMBB_CONTAINERS_OBJECT_POOL_H_
