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

#ifndef EMBB_ALGORITHMS_REDUCE_H_
#define EMBB_ALGORITHMS_REDUCE_H_

#include <embb/tasks/execution_policy.h>
#include <embb/algorithms/identity.h>

namespace embb {
namespace algorithms {

/**
 * \defgroup CPP_ALGORITHMS_REDUCTION Reduction
 * Parallel reduction computation
 * \ingroup CPP_ALGORITHMS
 * \{
 */

#ifdef DOXYGEN

/**
 * Performs a parallel reduction operation on a range of elements.
 *
 * The range consists of the elements from \c first to \c last, excluding the
 * last element. The type of the result (\c ReturnType) is deduced from the
 * \c neutral element.
 *
 * \return
 * <tt>reduction(transformation(*first), ..., transformation(*(last-1)))</tt>
 * where the reduction function is applied pairwise.
 * \throws embb::base::ErrorException if not enough MTAPI tasks can be created
 *         to satisfy the requirements of the algorithm.
 * \threadsafe if the elements in the range are not modified by another thread
 *             while the algorithm is executed.
 * \note No guarantee is given on the order in which the functions \c reduction
 *       and \c transformation are applied to the elements.\n
 *       For all \c x of type \c ReturnType it must hold that
 *       <tt>reduction(x, neutral) == x</tt>. \n
 *       The reduction operation need not be commutative but must be
 *       associative, i.e., <tt>reduction(x, reduction(y, z)) ==
 *       reduction(reduction(x, y), z))</tt> for all \c x, \c y, \c z of type
 *       \c ReturnType.
 * \see embb::mtapi::ExecutionPolicy, ZipIterator, Identity
 * \tparam RAI Random access iterator
 * \tparam ReturnType Type of result of reduction operation, deduced from
 *         \c neutral
 * \tparam ReductionFunction Binary reduction function object with signature
 *         <tt>ReturnType ReductionFunction(ReturnType, ReturnType)</tt>.
 * \tparam TransformationFunction Unary transformation function object with
 *         signature <tt>ReturnType TransformationFunction(typename
 *         std::iterator_traits<RAI>::value_type)</tt>
 */
template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
ReturnType Reduce(
  RAI first,
  /**< [IN] Random access iterator pointing to the first element of the range */
  RAI last,
  /**< [IN] Random access iterator pointing to the last plus one element of the
            range */
  ReturnType neutral,
  /**< [IN] Neutral element of the reduction operation. */
  ReductionFunction reduction,
  /**< [IN] Reduction operation to be applied to the elements of the range */
  TransformationFunction transformation = Identity(),
  /**< [IN] Transforms the elements of the range before the reduction operation
            is applied */
  const embb::mtapi::ExecutionPolicy& policy = embb::mtapi::ExecutionPolicy(),
  /**< [IN] embb::mtapi::ExecutionPolicy for the reduction computation */
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
template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
ReturnType Reduce(
  RAI first,
  RAI last,
  ReturnType neutral,
  ReductionFunction reduction,
  TransformationFunction transformation,
  const embb::tasks::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ReturnType, typename ReductionFunction>
ReturnType Reduce(
  RAI first,
  RAI last,
  ReturnType neutral,
  ReductionFunction reduction
  ) {
  return Reduce(first, last, neutral, reduction, Identity(),
    embb::tasks::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
ReturnType Reduce(
  RAI first,
  RAI last,
  ReturnType neutral,
  ReductionFunction reduction,
  TransformationFunction transformation
  ) {
  return Reduce(first, last, neutral, reduction, transformation,
    embb::tasks::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
ReturnType Reduce(
  RAI first,
  RAI last,
  ReturnType neutral,
  ReductionFunction reduction,
  TransformationFunction transformation,
  const embb::tasks::ExecutionPolicy& policy
  ) {
  return Reduce(first, last, neutral, reduction, transformation, policy, 0);
}

#endif // else DOXYGEN

/**
 * \}
 */

}  // namespace algorithms
}  // namespace embb

#include <embb/algorithms/internal/reduce-inl.h>

#endif  // EMBB_ALGORITHMS_REDUCE_H_
