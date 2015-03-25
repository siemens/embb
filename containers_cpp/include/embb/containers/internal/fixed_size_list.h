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

#ifndef EMBB_CONTAINERS_INTERNAL_FIXED_SIZE_LIST_H_
#define EMBB_CONTAINERS_INTERNAL_FIXED_SIZE_LIST_H_

namespace embb {
namespace containers {
namespace internal {

/**
 * A list with fixed size, implemented as an array. Replaces std::vector that
 * was used in previous hazard pointer implementation.
 *
 * Provides iterators, so we can apply algorithms from the STL.
 *
 * \tparam ElementT Type of the elements contained in the list.
 */
template< typename ElementT >
class FixedSizeList {
 private:
  /**
   * Capacity of the list
   */
  size_t max_size_;

  /**
   * Size of the list
   */
  size_t size_;

  /**
   * Pointer to the array containing the list
   */
  ElementT* elementsArray;

 public:
  /**
   * Definition of an iterator
   */
  typedef ElementT * iterator;

  /**
   * Definition of a const iterator
   */
  typedef const ElementT * const_iterator;

  /**
   * Constructor, initializes list with given capacity
   */
  FixedSizeList(
    size_t capacity
    /**< [IN] Capacity of the list */
  );
    
  /**
   * Copy constructor.
   */
  FixedSizeList(
    const FixedSizeList &
    /**< [IN] Other list */
  );

  /**
   * Copies the elements of another list to this list. The capacity of
   * this list has to be greater than or equal to the size of the other list.
   */
  FixedSizeList & operator=(
    const FixedSizeList & other
    /**< [IN] Other list */
  );

  /**
   * Gets the current number of element in the list
   *
   * \return Size of the list
   */
  inline size_t GetSize() const;

  /**
   * Gets the maximum number of elements that the list can hold
   *
   * \return The element capacity of the list
   */
  inline size_t GetMaxSize() const;

  /**
   * Removes all elements from the list without changing the capacity
   */
  inline void clear();

  /**
   * Iterator pointing to the first element
   *
   * \return Begin iterator
   */
  iterator begin() const;

  /**
   * Iterator pointing beyond the last element
   *
   * \return End iterator
   */
  iterator end() const;

  /**
   * Appends an element to the end of the list
   *
   * \return \c false if the operation was not successful because the list is
   *         full, otherwise \c true.
   */
  bool PushBack(
    const ElementT & el
    /**< [IN] Element to append to the list */
  );

  /**
   * Destructs the list.
   */
  ~FixedSizeList();
};

} // namespace internal
} // namespace containers
} // namespace embb

#include <embb/containers/internal/fixed_size_list-inl.h>

#endif  // EMBB_CONTAINERS_INTERNAL_FIXED_SIZE_LIST_H_
