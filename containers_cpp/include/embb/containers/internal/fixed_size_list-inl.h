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

#ifndef EMBB_CONTAINERS_INTERNAL_FIXED_SIZE_LIST_INL_H_
#define EMBB_CONTAINERS_INTERNAL_FIXED_SIZE_LIST_INL_H_

namespace embb {
namespace containers {
namespace internal {
template< typename ElementT >
FixedSizeList<ElementT>::FixedSizeList(size_t max_size) :
  max_size(max_size),
  size(0) {
  elementsArray = static_cast<ElementT*>(
    embb::base::Allocation::Allocate(sizeof(ElementT) *
    max_size));
}

template< typename ElementT >
inline size_t FixedSizeList<ElementT>::GetSize() const {
  return size;
}

template< typename ElementT >
inline size_t FixedSizeList<ElementT>::GetMaxSize() const {
  return max_size;
}

template< typename ElementT >
inline void FixedSizeList<ElementT>::clear() {
  size = 0;
}

template< typename ElementT >
typename FixedSizeList<ElementT>::iterator
FixedSizeList<ElementT>::begin() const {
  return &elementsArray[0];
}

template< typename ElementT >
typename FixedSizeList<ElementT>::iterator
FixedSizeList<ElementT>::end() const {
  return &elementsArray[size];
}

template< typename ElementT >
FixedSizeList< ElementT > &
FixedSizeList<ElementT>::operator= (const FixedSizeList & other) {
  size = 0;

  if (max_size < other.size) {
    EMBB_THROW(embb::base::ErrorException, "Copy target to small");
  }

  for (const_iterator it = other.begin(); it != other.end(); ++it) {
    PushBack(*it);
  }
  return *this;
}

template< typename ElementT >
bool FixedSizeList<ElementT>::PushBack(ElementT const el) {
  if (size + 1 > max_size) {
    return false;
  }
  elementsArray[size] = el;
  size++;
  return true;
}

template< typename ElementT >
FixedSizeList<ElementT>::~FixedSizeList() {
  embb::base::Allocation::Free(elementsArray);
}

} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_FIXED_SIZE_LIST_INL_H_
