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

#ifndef EMBB_ALGORITHMS_FOR_EACH_H_
#define EMBB_ALGORITHMS_FOR_EACH_H_

#include <embb/tasks/execution_policy.h>

namespace embb {
namespace algorithms {

/**
 * \defgroup CPP_ALGORITHMS_FOREACH Foreach
 * Parallel foreach loop
 * \ingroup CPP_ALGORITHMS
 * \{
 */

#ifdef DOXYGEN

/**
 * Applies a unary function to the elements of a range in parallel.
 *
 * The range consists of the elements from \c first to \c last, excluding the
 * last element.
 *
 * \throws embb::base::ErrorException if not enough MTAPI tasks can be created
 *         to satisfy the requirements of the algorithm.
 * \threadsafe if the elements in the range are not modified by another thread
 *             while the algorithm is executed.
 * \note No guarantee is given on the order in which the function is applied to
 *       the elements.
 * \see embb::mtapi::ExecutionPolicy, ZipIterator
 * \tparam RAI Random access iterator
 * \tparam Function Unary function with argument of type
 *         <tt>std::iterator_traits<RAI>::value_type</tt>.
 */
template<typename RAI, typename Function>
void ForEach(
  RAI first,
  /**< [IN] Random access iterator pointing to the first element of the range */
  RAI last,
  /**< [IN] Random access iterator pointing to the last plus one element of the
            range */
  Function unary,
  /**< [IN] Unary function applied to each element in the range */
  const embb::mtapi::ExecutionPolicy& policy = embb::mtapi::ExecutionPolicy(),
  /**< [IN] embb::mtapi::ExecutionPolicy for the foreach loop execution */
  size_t block_size = 0
  /**< [IN] Lower bound for partitioning the range of elements into blocks that
            are treated in parallel. Partitioning of a block stops if its size
            is less than or equal to \c block_size. The default value 0 means
            that the minimum block size is determined automatically depending on
            the number of elements in the range divided by the number of
            available cores. */
  );

#else // DOXYGEN

/**
 * Overload of above described Doxygen dummy.
 */
template<typename RAI, typename Function>
void ForEach(
  RAI first,
  RAI last,
  Function unary,
  const embb::tasks::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename Function>
void ForEach(
  RAI first,
  RAI last,
  Function unary
  ) {
  ForEach(first, last, unary, embb::tasks::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename Function>
void ForEach(
  RAI first,
  RAI last,
  Function unary,
  const embb::tasks::ExecutionPolicy& policy
  ) {
  ForEach(first, last, unary, policy, 0);
}

#endif // else DOXYGEN

/**
 * \}
 */

}  // namespace algorithms
}  // namespace embb

#include <embb/algorithms/internal/for_each-inl.h>

#endif  // EMBB_ALGORITHMS_FOR_EACH_H_
