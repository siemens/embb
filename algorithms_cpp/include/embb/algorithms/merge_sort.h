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

#ifndef EMBB_ALGORITHMS_MERGE_SORT_H_
#define EMBB_ALGORITHMS_MERGE_SORT_H_

#include <functional>
#include <embb/mtapi/job.h>
#include <embb/mtapi/execution_policy.h>
#include <embb/base/memory_allocation.h>

namespace embb {
namespace algorithms {

/**
 * \defgroup CPP_ALGORITHMS_SORTING Sorting
 * Parallel merge sort and quick sort algorithms
 * \ingroup CPP_ALGORITHMS
 * \{
 */

#ifdef DOXYGEN

/**
 * Sorts a range of elements using a parallel merge sort algorithm with implicit
 * allocation of dynamic memory.
 *
 * The range consists of the elements from \c first to \c last, excluding the
 * last element. Since the algorithm does not sort in-place, it requires
 * additional memory which is implicitly allocated by the function.
 *
 * \throws embb::base::ErrorException if not enough MTAPI tasks can be created
 *         to satisfy the requirements of the algorithm.
 * \memory Array with <tt>last-first</tt> elements of type
 *         <tt>std::iterator_traits<RAI>::value_type</tt>.
 * \threadsafe if the elements in the range <tt>[first,last)</tt> are not
 *             modified by another thread while the algorithm is executed.
 * \note No guarantee is given on the execution order of the comparison
 *       operations.<br/>
 *       For nested algorithms, the task limit may be exceeded. In that case,
 *       increase the task limit of the MTAPI node.
 * \see embb::mtapi::ExecutionPolicy, MergeSort()
 * \tparam RAI Random access iterator
 * \tparam ComparisonFunction Binary predicate with both arguments of type
 *         <tt>std::iterator_traits<RAI>::value_type</tt> or an
 *         embb::mtapi::Job associated with an action function accepting a
 *         struct containing two members of type 
 *         <tt>std::iterator_traits<RAI>::value_type</tt> as its argument
 *         buffer and a struct containing one bool member as its result buffer.
 */
template<typename RAI, typename ComparisonFunction>
void MergeSortAllocate(
  RAI first,
  /**< [IN] Random access iterator pointing to the first element of the range */
  RAI last,
  /**< [IN] Random access iterator pointing to the last plus one element of the
            range */
  ComparisonFunction comparison
    = std::less<typename std::iterator_traits<RAI>::value_type>(),
  /**< [IN] Binary predicate used to establish the sorting order. An element
            \c a appears before an element \c b in the sorted range if
            <tt>comparison(a, b) == true</tt>. The default value uses the
            less-than relation. */
  const embb::mtapi::ExecutionPolicy& policy = embb::mtapi::ExecutionPolicy(),
  /**< [IN] embb::mtapi::ExecutionPolicy for the merge sort algorithm */
  size_t block_size = 0
  /**< [IN] Lower bound for partitioning the range of elements into blocks that
            are sorted in parallel. Partitioning of a block stops if its size
            is less than or equal to \c block_size. The default value 0 means
            that the minimum block size is determined automatically depending on
            the number of elements in the range divided by the number of
            available cores. */
  );

/**
 * Sorts a range of elements using a parallel merge sort algorithm without
 * implicit allocation of dynamic memory.
 *
 * The range consists of the elements from \c first to \c last, excluding the
 * last element. Since the algorithm does not sort in-place, it requires
 * additional memory which must be provided by the user. The range pointed to
 * by \c temporary_first must have the same number of elements as the range to
 * be sorted, and the elements of both ranges must have the same type.
 *
 * \throws embb::base::ErrorException if not enough MTAPI tasks can be created
 *         to satisfy the requirements of the algorithm.
 * \threadsafe if the elements in the ranges <tt>[first,last)</tt> and
 *             <tt>[temporary_first,temporary_first+(last-first)</tt> are not
 *             modified by another thread while the algorithm is executed.
 * \note No guarantee is given on the execution order of the comparison
 *       operations.<br/>
 *       For nested algorithms, the task limit may be exceeded. In that case,
 *       increase the task limit of the MTAPI node.
 * \see embb::mtapi::ExecutionPolicy, MergeSortAllocate()
 * \tparam RAI Random access iterator
 * \tparam RAITemp Random access iterator for temporary memory. Has to have the
 *         same value type as RAI.
 * \tparam ComparisonFunction Binary predicate with both arguments of type
 *         <tt>std::iterator_traits<RAI>::value_type</tt>.
 */
template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSort(
  RAI first,
  /**< [IN] Random access iterator pointing to the first element of the range */
  RAI last,
  /**< [IN] Random access iterator to last plus one element to be sorted */
  RAITemp temporary_first,
  /**< [IN] Random access iterator pointing to the last plus one element of the
            range */
  ComparisonFunction comparison
    = std::less<typename std::iterator_traits<RAI>::value_type>(),
    /**< [IN] Binary predicate used to establish the sorting order. An element
              \c a appears before an element \c b in the sorted range if
              <tt>comparison(a, b) == true</tt>. The default value uses the
              less-than relation. */
  const embb::mtapi::ExecutionPolicy& policy = embb::mtapi::ExecutionPolicy(),
  /**< [IN] embb::mtapi::ExecutionPolicy for the merge sort algorithm */
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
template<typename RAI, typename RAITemp>
void MergeSort(
  RAI first,
  RAI last,
  RAITemp temporary_first,
  embb::mtapi::Job comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy.
 */
template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSort(
  RAI first,
  RAI last,
  RAITemp temporary_first,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  );

/**
 * Overload of above described Doxygen dummy.
 */
template<typename RAI, typename ComparisonFunction>
void MergeSortAllocate(
  RAI first,
  RAI last,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  ) {
  typedef base::Allocation Alloc;
  typename std::iterator_traits<RAI>::difference_type distance = last - first;
  typedef typename std::iterator_traits<RAI>::value_type value_type;
  if (distance == 0) {
    return;
  } else if (distance < 0) {
    EMBB_THROW(embb::base::ErrorException, "Negative range for MergeSort");
  }
  value_type* temporary = static_cast<value_type*>(
                            Alloc::Allocate(distance * sizeof(value_type)));

  EMBB_TRY {
    MergeSort(first, last, temporary, comparison, policy, block_size);
  } EMBB_CATCH (embb::base::ErrorException & e) {
    // embb exception handling does not support catch(...) and rethrow yet.
    Alloc::Free(temporary);

    // Rethrow only, if exceptions are enabled... Otherwise, the parameter
    // e cannot be used, as it is not defined.
#ifdef EMBB_USE_EXCEPTIONS
    EMBB_THROW(embb::base::ErrorException, e.what());
#endif
  }
  Alloc::Free(temporary);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI>
void MergeSortAllocate(
  RAI first,
  RAI last
  ) {
  MergeSortAllocate(first, last,
                    std::less<typename std::iterator_traits<RAI>::value_type>(),
                    embb::mtapi::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ComparisonFunction>
void MergeSortAllocate(
  RAI first,
  RAI last,
  ComparisonFunction comparison
  ) {
  MergeSortAllocate(first, last, comparison, embb::mtapi::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename ComparisonFunction>
void MergeSortAllocate(
  RAI first,
  RAI last,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy
  ) {
  MergeSortAllocate(first, last, comparison, policy, 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename RAITemp>
void MergeSort(
  RAI first,
  RAI last,
  RAITemp temporary_first
  ) {
  MergeSort(first, last, temporary_first,
            std::less<typename std::iterator_traits<RAI>::value_type>(),
            embb::mtapi::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSort(
  RAI first,
  RAI last,
  RAITemp temporary_first,
  ComparisonFunction comparison
  ) {
  MergeSort(first, last, temporary_first, comparison,
    embb::mtapi::ExecutionPolicy(), 0);
}

/**
 * Overload of above described Doxygen dummy with less arguments.
 */
template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSort(
  RAI first,
  RAI last,
  RAITemp temporary_first,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy
  ) {
  MergeSort(first, last, temporary_first, comparison, policy, 0);
}

#endif // else DOXYGEN

/**
 * \}
 */

}  // namespace algorithms
}  // namespace embb

#include<embb/algorithms/internal/merge_sort-inl.h>

#endif  // EMBB_ALGORITHMS_MERGE_SORT_H_
