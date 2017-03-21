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

#ifndef EMBB_ALGORITHMS_COUNT_H_
#define EMBB_ALGORITHMS_COUNT_H_

#include <embb/mtapi/job.h>
#include <embb/mtapi/execution_policy.h>
#include <iterator>

namespace embb {
namespace algorithms {

/**
 * \defgroup CPP_ALGORITHMS_COUNT Counting
 * Parallel count operation
 * \ingroup CPP_ALGORITHMS
 * \{
 */

#ifdef DOXYGEN

/**
 * Counts in parallel the number of elements in a range that are equal to
 * the specified value.
 *
 * The range consists of the elements from \c first to \c last, excluding the
 * last element.
 *
 * \return The number of elements that are equal to \c value
 * \throws embb::base::ErrorException if not enough MTAPI tasks can be created
 *         to satisfy the requirements of the algorithm.
 * \threadsafe if the elements in the range are not modified by another thread
 *             while the algorithm is executed.
 * \note No guarantee is given on the execution order of the comparison
 *       operations.<br/>
 *       For nested algorithms, the task limit may be exceeded. In that case,
 *       increase the task limit of the MTAPI node.
 * \see CountIf(), embb::mtapi::ExecutionPolicy
 * \tparam RAI Random access iterator
 * \tparam ValueType Type of \c value that is compared to the elements in the
 *         range using the \c operator==.
 */
template<typename RAI, typename ValueType>
typename std::iterator_traits<RAI>::difference_type Count(
  RAI first,
  /**< [IN] Random access iterator pointing to the first element of the range */
  RAI last,
  /**< [IN] Random access iterator pointing to the last plus one element of the
            range */
  const ValueType& value,
  /**< [IN] Value that the elements in the range are compared to using
            \c operator== */
  const embb::mtapi::ExecutionPolicy& policy = embb::mtapi::ExecutionPolicy(),
  /**< [IN] embb::mtapi::ExecutionPolicy for the counting algorithm */
  size_t block_size = 0
  /**< [IN] Lower bound for partitioning the range of elements into blocks that
            are sorted in parallel. Partitioning of a block stops if its size
            is less than or equal to \c block_size. The default value 0 means
            that the minimum block size is determined automatically depending on
            the number of elements in the range divided by the number of
            available cores. */
  );

/**
 * Counts in parallel the number of elements in a range for which the comparison
 * function returns \c true.
 *
 * The range consists of the elements from \c first to \c last, excluding the
 * last element.
 *
 * \return The number of elements for which \c comparison returns true
 * \throws embb::base::ErrorException if not enough MTAPI tasks can be created
 *         to satisfy the requirements of the algorithm.
 * \threadsafe if the elements in the range are not modified by another thread
 *             while the algorithm is executed.
 * \note No guarantee is given on the execution order of the comparison
 *       function.<br/>
 *       For nested algorithms, the task limit may be exceeded. In that case,
 *       increase the task limit of the MTAPI node.
 * \see Count(), embb::mtapi::ExecutionPolicy
 * \tparam RAI Random access iterator
 * \tparam ComparisonFunction Unary predicate with argument of type
 *         <tt>std::iterator_traits<RAI>::value_type</tt> or an
 *         embb::mtapi::Job associated with an action function accepting a
 *         struct containing one member of type 
 *         <tt>std::iterator_traits<RAI>::value_type</tt> as its argument
 *         buffer and a struct containing one bool member as its result buffer.
 */
template<typename RAI, typename ComparisonFunction>
typename std::iterator_traits<RAI>::difference_type CountIf(
  RAI first,
  /**< [IN] Random access iterator pointing to the first element of the range
            RAI last, */
  /**< [IN] Random access iterator pointing to the last plus one element of the
            range */
  ComparisonFunction comparison,
  /**< [IN] Unary predicate used to test the elements in the range. Elements for
            which \c comparison returns true are counted. */
  const embb::mtapi::ExecutionPolicy& policy = embb::mtapi::ExecutionPolicy(),
  /**< [IN] embb::mtapi::ExecutionPolicy for the counting algorithm */
  size_t block_size = 0
  /**< [IN] Lower bound for partitioning the range of elements into blocks that
            are sorted in parallel. Partitioning of a block stops if its size
            is less than or equal to \c block_size. The default value 0 means
            that the minimum block size is determined automatically depending on
            the number of elements in the range divided by the number of
            available cores. */
  );

#else // DOXYGEN

/**
 * Overload of above described Doxygen dummy.
 */
template<typename RAI, typename ValueType>
typename std::iterator_traits<RAI>::difference_type Count(
  RAI first,
  RAI last,
  const ValueType& value,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ValueType>
typename std::iterator_traits<RAI>::difference_type Count(
  RAI first,
  RAI last,
  const ValueType& value
  ) {
  return Count(first, last, value, embb::mtapi::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ValueType>
typename std::iterator_traits<RAI>::difference_type Count(
  RAI first,
  RAI last,
  const ValueType& value,
  const embb::mtapi::ExecutionPolicy& policy
  ) {
  return Count(first, last, value, policy, 0);
}

/**
 * Overload of above described Doxygen dummy.
 */
template<typename RAI>
typename std::iterator_traits<RAI>::difference_type CountIf(
  RAI first,
  RAI last,
  embb::mtapi::Job comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy.
 */
template<typename RAI, typename ComparisonFunction>
typename std::iterator_traits<RAI>::difference_type CountIf(
  RAI first,
  RAI last,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ComparisonFunction>
typename std::iterator_traits<RAI>::difference_type CountIf(
  RAI first,
  RAI last,
  ComparisonFunction comparison
  ) {
  return CountIf(first, last, comparison, embb::mtapi::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ComparisonFunction>
typename std::iterator_traits<RAI>::difference_type CountIf(
  RAI first,
  RAI last,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy
  ) {
  return CountIf(first, last, comparison, policy, 0);
}

#endif // else DOXYGEN

/**
 * \}
 */

}  // namespace algorithms
}  // namespace embb

#include <embb/algorithms/internal/count-inl.h>

#endif  // EMBB_ALGORITHMS_COUNT_H_
