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

#ifndef EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_

#include <cassert>
#include <iterator>
#include <functional>

#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>

namespace embb {
namespace algorithms {

namespace internal {

/**
 * Contains the merge sort MTAPI action function and data needed there.
 */
template <typename RAI, typename RAITemp, typename ComparisonFunction>
class MergeSortFunctor {
 public:
  typedef typename std::iterator_traits<RAI>::value_type value_type;

  MergeSortFunctor(RAI first, RAI last, RAITemp temporary_first,
                   ComparisonFunction comparison, const ExecutionPolicy& policy,
                   size_t block_size, const RAI& global_first, int depth)
    : first_(first), last_(last), temp_first_(temporary_first),
      comparison_(comparison), policy_(policy), block_size_(block_size),
      global_first_(global_first), depth_(depth) {
  }

  void Action(mtapi::TaskContext& context) {
    typedef typename std::iterator_traits<RAI>::difference_type difference_type;
    size_t distance = static_cast<size_t>(std::distance(first_, last_));
    if (distance <= 1) {
      if(!CloneBackToInput() && distance != 0) {
        RAITemp temp_first = temp_first_;
        temp_first += std::distance(global_first_, first_);
        *temp_first = *first_;
      }
      return;
    }
    internal::ChunkPartitioner<RAI> partitioner(first_, last_, 2);
    MergeSortFunctor<RAI, RAITemp, ComparisonFunction> functorL(
        partitioner[0].GetFirst(), partitioner[0].GetLast(), temp_first_,
        comparison_, policy_, block_size_, global_first_, depth_ + 1);
    MergeSortFunctor<RAI, RAITemp, ComparisonFunction> functorR(
      partitioner[1].GetFirst(), partitioner[1].GetLast(), temp_first_,
      comparison_, policy_, block_size_, global_first_, depth_ + 1);

    if (distance <= block_size_) {
      functorL.Action(context);
      functorR.Action(context);
    } else {
      mtapi::Node& node = mtapi::Node::GetInstance();
      mtapi::Task taskL = node.Spawn(mtapi::Action(base::MakeFunction(functorL,
          &MergeSortFunctor<RAI, RAITemp, ComparisonFunction>::Action),
          policy_.GetAffinity()), policy_.GetPriority());
      mtapi::Task taskR = node.Spawn(mtapi::Action(base::MakeFunction(functorR,
          &MergeSortFunctor<RAI, RAITemp, ComparisonFunction>::Action),
          policy_.GetAffinity()), policy_.GetPriority());
      taskL.Wait(MTAPI_INFINITE);
      taskR.Wait(MTAPI_INFINITE);
    }

    if(CloneBackToInput()) {
      difference_type first = std::distance(global_first_, functorL.first_);
      difference_type mid = std::distance(global_first_, functorR.first_);
      difference_type last = std::distance(global_first_, functorR.last_);
      SerialMerge(temp_first_ + first, temp_first_ + mid,
                  temp_first_ + last, functorL.first_, comparison_);
    } else {
      SerialMerge(functorL.first_, functorR.first_, functorR.last_,
                  temp_first_ + std::distance(global_first_, functorL.first_),
                  comparison_);
    }
  }

  /**
   * Determines the input and output arrays for one level in merge sort.
   *
   * \return \c true if the temporary data range is input and the array to be
   *         sorted is output. \c false, if the other way around.
   */
  bool CloneBackToInput() {
    return depth_ % 2 == 0 ? true : false;
  }

 private:
  RAI first_;
  RAI last_;
  RAITemp temp_first_;
  ComparisonFunction comparison_;
  const ExecutionPolicy &policy_;
  size_t block_size_;
  const RAI& global_first_;
  int depth_;

  MergeSortFunctor(const MergeSortFunctor&);
  MergeSortFunctor& operator=(const MergeSortFunctor&);

  template<typename RAIIn, typename RAIOut>
  void SerialMerge(RAIIn first, RAIIn mid, RAIIn last, RAIOut out,
                   ComparisonFunction comparison) {
    RAIIn save_mid = mid;
    while ((first != save_mid) && (mid != last)) {
      if (comparison(*first, *mid)) {
        *out = *first;
        ++out;
        ++first;
      } else {
        *out = *mid;
        ++out;
        ++mid;
      }
    }
    while (first != save_mid) {
      *out = *first;
      ++out;
      ++first;
    }
    while(mid != last) {
      *out = *mid;
      ++out;
      ++mid;
    }
  }
};

}  // namespace internal

template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSort(
  RAI first,
  RAI last,
  RAITemp temporary_first,
  ComparisonFunction comparison,
  const ExecutionPolicy& policy,
  size_t block_size
  ) {
  typedef typename std::iterator_traits<RAI>::difference_type difference_type;
  embb::mtapi::Node &node = embb::mtapi::Node::GetInstance();
  difference_type distance = last - first;
  assert(distance >= 0);

  assert(block_size == 0); // TODO(Georgios Christodoulou):
                           // Take block size into account

  if (block_size == 0) {
    block_size= (static_cast<size_t>(distance) / node.GetCoreCount());
    if (block_size == 0)
      block_size = 1;
  }
  if (((distance/block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Not enough MTAPI tasks available to perform the merge sort");
  }

  internal::MergeSortFunctor<RAI, RAITemp, ComparisonFunction> functor(
      first, last, temporary_first, comparison, policy, block_size, first, 0);
  mtapi::Task task = node.Spawn(mtapi::Action(base::MakeFunction(functor,
    &internal::MergeSortFunctor<RAI, RAITemp, ComparisonFunction>::Action),
    policy.GetAffinity()), policy.GetPriority());

  task.Wait(MTAPI_INFINITE);
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_
