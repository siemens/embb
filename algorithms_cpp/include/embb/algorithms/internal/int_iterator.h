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

#ifndef EMBB_ALGORITHMS_INTERNAL_INT_ITERATOR_H_
#define EMBB_ALGORITHMS_INTERNAL_INT_ITERATOR_H_

#include <iterator>

namespace embb {
namespace algorithms {

namespace internal {

template <typename Integer>
class IntIterator : public
  std::iterator<std::random_access_iterator_tag, Integer> {
 public:
  typedef typename std::iterator_traits<IntIterator>::difference_type
    Difference;

 private:
  Integer value_;
  Difference stride_;

 public:
  explicit IntIterator(Integer value)
    : value_(value), stride_(1) {
    // emtpy
  }

  explicit IntIterator(Integer value, Difference stride)
    : value_(value), stride_(stride) {
    // emtpy
  }

  IntIterator(IntIterator const & other)
    : value_(other.value_), stride_(other.stride_) {
    // emtpy
  }

  IntIterator & operator =(IntIterator const & other) {
    value_ = other.value_;
    stride_ = other.stride_;
    return *this;
  }

  IntIterator & operator +=(Difference n) {
    value_ = Integer(value_ + n * stride_);
    return *this;
  }

  IntIterator & operator -=(Difference n) {
    value_ = Integer(value_ - n * stride_);
    return *this;
  }

  IntIterator & operator ++() {
    value_ = Integer(value_ + stride_);
    return *this;
  }

  IntIterator operator ++(int) {
    Integer result = value_;
    value_ = Integer(value_ + stride_);
    return IntIterator(result, stride_);
  }

  IntIterator & operator --() {
    value_ -= stride_;
    return *this;
  }

  IntIterator operator --(int) {
    Integer result = value_;
    value_ = Integer(value_ - stride_);
    return IntIterator(result, stride_);
  }

  IntIterator operator +(Difference n) const {
    return IntIterator(Integer(value_ + n * stride_), stride_);
  }

  Difference operator -(IntIterator const & it) const {
    return Difference(value_ - it.value_) / stride_;
  }

  IntIterator operator -(Difference n) const {
    return IntIterator(Integer(value_ - n * stride_), stride_);
  }

  bool operator ==(IntIterator const & it) const {
    return value_ == it.value_;
  }

  bool operator !=(IntIterator const & it) const {
    return value_ != it.value_;
  }

  bool operator <(IntIterator const & it) const {
    return value_ < it.value_;
  }

  bool operator <=(IntIterator const & it) const {
    return value_ <= it.value_;
  }

  bool operator >(IntIterator const & it) const {
    return value_ > it.value_;
  }

  bool operator >=(IntIterator const & it) const {
    return value_ >= it.value_;
  }

  Integer & operator *() {
    return value_;
  }

  Integer operator *() const {
    return value_;
  }

  Integer operator[](Difference n) const {
    return Integer(value_ + n * stride_);
  }
};

template <typename Integer>
inline IntIterator<Integer> operator +(
  typename IntIterator<Integer>::Difference n,
  IntIterator<Integer> const & it) {
  return it + n;
}

}  // namespace internal

}  // namespace algorithms
}  // namespace embb

#endif // EMBB_ALGORITHMS_INTERNAL_INT_ITERATOR_H_
