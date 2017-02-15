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

#ifndef EMBB_ALGORITHMS_INTERNAL_QUICK_SORT_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_QUICK_SORT_INL_H_

#include <cassert>
#include <iterator>
#include <functional>
#include <utility>   // swap C++ 11
#include <algorithm> // swap C++ 98

#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>
#include <embb/algorithms/internal/comparison_job_functor.h>

namespace embb {
namespace algorithms {

namespace internal {

template <typename RAI, typename ComparisonFunction>
class QuickSortFunctor {
 public:
  /**
   * Constructs a functor.
   */
  QuickSortFunctor(RAI first, RAI last, ComparisonFunction comparison,
    const embb::mtapi::ExecutionPolicy& policy, size_t block_size)
    : first_(first), last_(last), comparison_(comparison), policy_(policy),
      block_size_(block_size) {
  }

  /**
   * MTAPI action function and starting point of the parallel quick sort.
   */
  void Action(embb::mtapi::TaskContext&) {
    Difference distance = last_ - first_;
    if (distance <= 1) {
      return;
    } else {
      Difference pivot = MedianOfNine(first_, last_);
      RAI mid = first_ + pivot;
      mid = SerialPartition(first_, last_, mid);
      if (distance <= static_cast<Difference>(block_size_)) {
        SerialQuickSort(first_, mid);
        SerialQuickSort(mid, last_);
      } else {
        embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
        QuickSortFunctor functor_l(first_, mid, comparison_, policy_,
                                   block_size_);
        embb::mtapi::Task task_l = node.Start(
          base::MakeFunction(functor_l, &QuickSortFunctor::Action),
          policy_);
        QuickSortFunctor functor_r(mid, last_, comparison_, policy_,
                                   block_size_);
        embb::mtapi::Task task_r = node.Start(
          base::MakeFunction(functor_r, &QuickSortFunctor::Action),
          policy_);
        task_l.Wait(MTAPI_INFINITE);
        task_r.Wait(MTAPI_INFINITE);
      }
    }
  }

 private:
  RAI first_;
  RAI last_;
  ComparisonFunction comparison_;
  const embb::mtapi::ExecutionPolicy& policy_;
  size_t block_size_;

  typedef typename std::iterator_traits<RAI>::difference_type Difference;

  /**
   * Computes the pseudo-median of nine by using MedianOfThree().
   */
  Difference MedianOfNine(RAI first, RAI last) {
    Difference distance = last - first;
    Difference offset = distance / static_cast<Difference>(8);
    if (offset == 0) {
      return distance / 2;
    }
    Difference pseudo_median_of_nine = MedianOfThree(
        first,
        MedianOfThree(first, static_cast<Difference>(0), offset, offset * 2),
        MedianOfThree(first, offset * 3, offset * 4, offset * 5),
        MedianOfThree(first, offset * 6, offset * 7, distance - 1));
    return pseudo_median_of_nine;
  }

  /**
   * Computes the median of three.
   */
  Difference MedianOfThree(RAI first, Difference left, Difference mid,
                           Difference right) {
    if (comparison_(*(first + left), *(first + mid))) {
      if (comparison_(*(first + mid), *(first + right))) {
        return mid;
      } else {
        if (comparison_(*(first + left), *(first + right)))
          return right;
        else
          return left;
      }
    } else {
      if (comparison_(*(first + right), *(first + mid))) {
        return mid;
      } else {
        if (comparison_(*(first + right), *(first + left)))
          return right;
        else
          return left;
      }
    }
  }

  /**
   * Performs a quick sort partitioning as serial computation.
   */
  RAI SerialPartition(RAI first, RAI last, RAI pivot) {
    while (first != last) {
      while (comparison_(*first, *pivot)) {
        ++first;
        if (first == last)
          return first;
      }
      do {
        --last;
        if (first == last) return first;
      } while (comparison_(*pivot, *last));
      std::swap(*first, *last);
      if(pivot == first) {
        pivot = last;
      } else if (pivot == last) {
        pivot = first;
      }
      ++first;
    }
    return first;
  }

  /**
   * Performs the quick sort algorithm as serial computation.
   */
  void SerialQuickSort(RAI first, RAI last) {
    if (last - first <= 1) {
      return;
    } else {
      Difference pivot = MedianOfNine(first, last);
      RAI mid = first + pivot;
      mid = SerialPartition(first, last, mid);
      SerialQuickSort(first, mid);
      SerialQuickSort(mid, last);
    }
  }

  /**
   * Disables assignment.
   */
  QuickSortFunctor& operator=(const QuickSortFunctor&);

  /**
   * Disables Copying.
   */
  QuickSortFunctor(const QuickSortFunctor&);
};

template <typename RAI, typename ComparisonFunction>
void QuickSortIteratorCheck(RAI first, RAI last,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size,
  std::random_access_iterator_tag) {
  embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
  typedef typename std::iterator_traits<RAI>::difference_type difference_type;
  difference_type distance = std::distance(first, last);
  if (distance == 0) {
    return;
  } else if (distance < 0) {
    EMBB_THROW(embb::base::ErrorException, "Negative range for QuickSort");
  }
  unsigned int num_cores = policy.GetCoreCount();
  if (num_cores == 0) {
    EMBB_THROW(embb::base::ErrorException, "No cores in execution policy");
  }
  if (block_size == 0) {
    block_size = (static_cast<size_t>(distance) / num_cores);
    if (block_size == 0)
      block_size = 1;
  }
  if (((distance / block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Not enough MTAPI tasks available for performing quick sort");
  }
  QuickSortFunctor<RAI, ComparisonFunction> functor(
      first, last, comparison, policy, block_size);
  embb::mtapi::Task task = node.Start(
    embb::base::MakeFunction(functor,
      &QuickSortFunctor<RAI, ComparisonFunction>::Action),
    policy);
  task.Wait(MTAPI_INFINITE);
}

}  // namespace internal

template <typename RAI>
void QuickSort(RAI first, RAI last, embb::mtapi::Job comparison,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAI>::iterator_category category;
  internal::QuickSortIteratorCheck(first, last,
    internal::ComparisonJobFunctor<
      typename std::iterator_traits<RAI>::value_type>(comparison, policy),
    policy, block_size, category());
}

template <typename RAI, typename ComparisonFunction>
void QuickSort(RAI first, RAI last, ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAI>::iterator_category category;
  internal::QuickSortIteratorCheck(first, last, comparison,
                                   policy, block_size, category());
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_QUICK_SORT_INL_H_
