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

#ifndef EMBB_CONTAINERS_INTERNAL_RETURNING_TRUE_ITERATOR_H_
#define EMBB_CONTAINERS_INTERNAL_RETURNING_TRUE_ITERATOR_H_

#include <iterator>

namespace embb {
namespace containers {
namespace internal {

/**
 * Helper providing a virtual iterator that just returns true in each
 * iteration step. Used for filling the value pool. Implements the normal
 * C++ iterator concept. Not further documented here.
 */
class ReturningTrueIterator {
 public:
  typedef ReturningTrueIterator self_type;
  typedef bool   value_type;
  typedef bool & reference;
  typedef bool * pointer;
  typedef ::std::forward_iterator_tag iterator_category;
  typedef int difference_type;

 public:
  inline explicit ReturningTrueIterator(size_t count_value) :
    count_value(count_value),
    ret_value(true)
  { }
  inline self_type operator++() {
    self_type i = *this;
    count_value++;
    return i;
  }
  inline self_type operator++(int) {
    count_value++;
    return *this;
  }
  inline reference operator*() {
    return ret_value;
  }
  inline pointer operator->() {
    return &ret_value;
  }
  inline bool operator==(const self_type & rhs) {
    return count_value == rhs.count_value;
  }
  inline bool operator!=(const self_type & rhs) {
    return count_value != rhs.count_value;
  }
  inline difference_type operator-(const self_type & rhs) {
    return static_cast<difference_type>(count_value) -
      static_cast<difference_type>(rhs.count_value);
  }

 private:
  size_t count_value;
  bool ret_value;
};
} // namespace internal
} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_RETURNING_TRUE_ITERATOR_H_

