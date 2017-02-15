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

#ifndef EMBB_ALGORITHMS_ZIP_ITERATOR_H_
#define EMBB_ALGORITHMS_ZIP_ITERATOR_H_

#include<iterator>
#include <utility>
#include <cassert>

namespace embb {
namespace algorithms {
/**
 * \defgroup CPP_ALGORITHMS_ZIP_ITERATOR Zip Iterator
 * Zip two iterators
 * \ingroup CPP_ALGORITHMS
 * \{
 */

/**
 * Container for the values of two dereferenced iterators.
 * The values contained are of type
 * <tt>std::iterator_traits<Iterator>::reference</tt>.
 * \notthreadsafe
 * \tparam TypeA Type of the first value
 * \tparam TypeB Type of the first value
 *
 */
template<typename TypeA, typename TypeB>
class ZipPair {
 public:
  /**
   * Constructs a pair from two values.
   */
  ZipPair(
    TypeA first,
    /**< [IN] First value*/
    TypeB second
    /**< [IN] Second value*/
    )
  :first_(first), second_(second) {}

  /**
   * Copies a pair.
   */
  ZipPair(
    const ZipPair& other
    /**< [IN] pair to copy */
    )
  :first_(other.first_), second_(other.second_) {}

  /**
   * Returns the first value of the pair.
   *
   * \return The first value of the pair.
   */
  TypeA First() {
    return first_;
  }

  /**
   * Returns the second value of the pair.
   *
   * \return The second value of the pair
   */
  TypeB Second() {
      return second_;
  }

  /**
   * Returns the first value of the pair.
   *
   * \return The first value of the pair.
   */
  const TypeA First() const {
    return first_;
  }

  /**
   * Returns the second value of the pair.
   *
   * \return The second value of the pair
   */
  const TypeB Second() const {
      return second_;
  }

 private:
  TypeA first_;
  TypeB second_;

  /**
   * Disable assignment since references cannot be assigned.
   */
  ZipPair &operator=(const ZipPair&);
};

/**
 * Zip container for two iterators.
 * This container allows zipping two iterators together, thus enabling them
 * to be used in situations where only one iterator can be used. Any operation
 * applied to the zip iterator will subsequently be applied to the contained
 * iterators. Dereferencing the iterator will return a ZipPair containing the
 * values referenced by the iterators.
 * \notthreadsafe
 * \note It is required that the two iterators have the same \c difference_type
 * or that at least the first iterator's \c difference_type can be implicitly
 * converted to the second iterator's \c difference_type. Moreover, when
 * calculating the distance between two ZipIterators, the distances between both
 * pairs of iterators are equal.
 * \see ZipPair
 * \tparam IteratorA First iterator
 * \tparam IteratorB Second iterator
 */
template<typename IteratorA, typename IteratorB>
class  ZipIterator{
 public:
  /**
   * \name Iterator Typedefs
   * Necessary typedefs for iterators (std compatibility).
   * \{
   */
  typedef std::random_access_iterator_tag iterator_category;
  typedef typename std::iterator_traits<IteratorA>::difference_type
      difference_type;
  typedef typename std::iterator_traits<IteratorA>::reference RefA;
  typedef typename std::iterator_traits<IteratorB>::reference RefB;
  typedef typename std::iterator_traits<IteratorA>::value_type ValueA;
  typedef typename std::iterator_traits<IteratorB>::value_type ValueB;
  typedef ZipPair<ValueA, ValueB>  value_type;
  typedef ZipPair<RefA, RefB>  reference;
  typedef ZipPair<RefA, RefB>  pointer;
  /**
   * \}
   */

  /**
   * Constructs a zip iterator from two iterators of any type.
   */
  ZipIterator(
    IteratorA iter_a,
    /**< [IN] First iterator*/
    IteratorB iter_b
    /**< [IN] Second iterator*/
    )
  :iter_a_(iter_a), iter_b_(iter_b) {}

  /**
   * Compares two zip iterators for equality.
   *
   * \return \c true if zip iterators are equal, otherwise \c false
   */
  bool operator==(
    const ZipIterator &other
    /**< [IN] Reference to right-hand side of equality operator */
    ) const {
    return iter_a_ == other.iter_a_ && iter_b_ == other.iter_b_;
  }

  /**
   * Compares two zip iterators for inequality.
   *
   * \return \c true if any iterator doesn't equal the other, otherwise \c false
   */
  bool operator!=(
    const ZipIterator &other
    /**< [IN] Reference to right-hand side of inequality operator */
    ) const {
    return iter_a_ != other.iter_a_ || iter_b_ != other.iter_b_;
  }

  /**
   * Applies prefix increment on both iterators.
   */
  void operator++() {
    ++iter_a_;
    ++iter_b_;
  }

  /**
   * Applies prefix decrement on both iterators.
   */
  void operator--() {
    --iter_a_;
    --iter_b_;
  }

  /**
   * Returns an instance of a zip iterator where both iterators have been
   * advanced by the specified distance.
   *
   * \return New zip iterator containing the advanced iterators
   */
  ZipIterator<IteratorA, IteratorB> operator+(
    difference_type distance
    /**< [IN] Number of elements to advance the iterators */
    ) const {
    ZipIterator<IteratorA, IteratorB> new_iterator(*this);
    new_iterator.iter_a_ += distance;
    new_iterator.iter_b_ += distance;

    return new_iterator;
  }

  /**
   * Returns an instance of a zip iterator where both iterators have been
   * regressed by the specified distance.
   *
   * \return New zip iterator containing the regressed iterators
   */
  ZipIterator<IteratorA, IteratorB> operator-(
    difference_type distance
    /**< [IN] Number of elements to regress the iterators */
    ) const {
    ZipIterator<IteratorA, IteratorB> new_iterator(*this);
    new_iterator.iter_a_ -= distance;
    new_iterator.iter_b_ -= distance;

    return new_iterator;
  }

  /**
   * Advances both iterators by the specified distance.
   *
   * \return Reference to \c *this
   */
  ZipIterator<IteratorA, IteratorB>& operator+=(
    difference_type distance
    /**< [IN] Number of elements to advance the iterators */
    ) {
    iter_a_ += distance;
    iter_b_ += distance;
    return *this;
  }

  /**
   * Regresses both iterators by the specified distance.
   *
   * \return Reference to \c *this
   */
  ZipIterator<IteratorA, IteratorB>& operator-=(
    difference_type distance
    /**< [IN] Number of elements to regress the iterators */
    ) {
    iter_a_ -= distance;
    iter_b_ -= distance;
    return *this;
  }

  /**
   * Computes the distance between two zip iterators.
   * It is assumed that both iterator pairs have the same distance.
   *
   * \return The distance between the zip iterators
   */
  difference_type operator-(
    const ZipIterator<IteratorA,
      IteratorB> &other
      /**< [IN] Reference to right-hand side of subtraction operator */
      ) const {
    assert(iter_a_ - other.iter_a_ == iter_b_ - other.iter_b_);
    return iter_a_ - other.iter_a_;
  }

  /**
   * Dereferences the zip iterator.
   *
   * \return ZipPair containing the dereferenced values.
   */
  reference operator*() const {
    return ZipPair<RefA, RefB>(*iter_a_, *iter_b_);
  }

 private:
  IteratorA iter_a_;
  IteratorB iter_b_;
};

/**
 * Creates a zip iterator from two iterators.
 * This is a convenience function which avoids calling the constructor of the
 * templated class.
 *
 * \return Constructed zip iterator
 * \tparam IteratorA Type of first iterator
 * \tparam IteratorB Type of second iterator
 */
template<typename IteratorA, typename IteratorB>
ZipIterator<IteratorA, IteratorB> Zip(
  IteratorA iter_a,
  /**< [IN] First iterator */
  IteratorB iter_b
  /**< [IN] Second iterator */
  ) {
  return ZipIterator<IteratorA, IteratorB>(iter_a, iter_b);
}

/**
 * \}
 */
}  // namespace algorithms
}  // namespace embb

#endif  //  EMBB_ALGORITHMS_ZIP_ITERATOR_H_
