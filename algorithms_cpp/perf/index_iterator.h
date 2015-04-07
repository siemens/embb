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

#ifndef EMBB_ALGORITHMS_PERF_INTERNAL_INDEX_ITERATOR_H_
#define EMBB_ALGORITHMS_PERF_INTERNAL_INDEX_ITERATOR_H_

#include <iterator>

namespace embb {
namespace algorithms {
namespace perf {

/**
 * Auxiliary helper for performance tests. Iterator returns index value
 * on dereferencing instead of resolving an underlying value. Prevents 
 * memory access in CPU-only test cases as parallel algorithms in embb 
 * currently do not support indices as RAI.
 */
template<typename T> 
class IndexIterator :
public ::std::iterator< ::std::random_access_iterator_tag, T >
{
private:
  typedef IndexIterator self_type;

public:
  typedef ::std::random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef int difference_type;
  typedef int distance_type;
  typedef T * pointer;
  typedef T & reference;

  IndexIterator() : _ptr(0), _value(static_cast<T>(0)) { }
  IndexIterator(int * rhs) : _ptr(*rhs), _value(static_cast<T>(0)) { }
  IndexIterator(int rhs) : _ptr(rhs), _value(static_cast<T>(0)) { }
  IndexIterator(const self_type &rhs) : _ptr(rhs._ptr), _value(rhs._value) { }

  inline self_type& operator+=(const difference_type& rhs) { 
    _ptr += rhs; return *this; 
  }
  inline self_type& operator-=(const difference_type& rhs) { 
    _ptr -= rhs; return *this; 
  }
  inline T & operator*() { 
    _value = static_cast<T>(_ptr);
    return _value;
  }
  inline T * operator->() {
    _value = static_cast<T>(_ptr);
    return _value;
  }
  inline T& operator[](const difference_type& rhs) { 
    return rhs; 
  }
  inline self_type& operator++() { 
    ++_ptr; return *this; 
  }
  inline self_type& operator--() { 
    --_ptr; return *this; 
  }
  inline difference_type operator+(const self_type& rhs) { 
    return difference_type(_ptr + rhs._ptr);
  }
  inline self_type operator+(const difference_type& rhs) const { 
    return self_type(_ptr + rhs); 
  }
  inline self_type operator+(difference_type& rhs) const {
    return self_type(_ptr + rhs);
  }
  inline self_type operator+=(const difference_type& rhs) const {
    return self_type(_ptr + rhs);
  }
  inline difference_type operator-(const self_type& rhs) {
    return difference_type(_ptr - rhs._ptr);
  }
  inline self_type operator-(const difference_type& rhs) const { 
    return self_type(_ptr - rhs); 
  }
  inline self_type operator-(difference_type& rhs) const {
    return self_type(_ptr - rhs);
  }
  inline self_type operator-=(const difference_type& rhs) const {
    return self_type(_ptr - rhs);
  }
  friend inline self_type operator+(const difference_type& lhs, const self_type& rhs) { 
    return self_type(lhs) + rhs; 
  }
  friend inline self_type operator+=(const difference_type& lhs, const self_type& rhs) {
    return self_type(lhs) + rhs;
  }
  friend inline self_type operator-(const difference_type& lhs, const self_type& rhs) { 
    return self_type(lhs) - rhs;
  }
  friend inline self_type operator-=(const difference_type& lhs, const self_type& rhs) {
    return self_type(lhs) - rhs;
  }
  inline bool operator==(const self_type& rhs) { 
    return _ptr == rhs._ptr; 
  }
  inline bool operator!=(const self_type& rhs) { 
    return _ptr != rhs._ptr; 
  }
  inline bool operator>(const self_type& rhs) { 
    return _ptr > rhs._ptr; 
  }
  inline bool operator<(const self_type& rhs) { 
    return _ptr < rhs._ptr; 
  }
  inline bool operator>=(const self_type& rhs) { 
    return _ptr >= rhs._ptr; 
  }
  inline bool operator<=(const self_type& rhs) { 
    return _ptr <= rhs._ptr; 
  }
protected:
  int _ptr;
  T _value;
};

} // namespace perf
} // namespace algorithms
} // namespace embb

#endif /* EMBB_ALGORITHMS_PERF_INTERNAL_INDEX_ITERATOR_H_ */
