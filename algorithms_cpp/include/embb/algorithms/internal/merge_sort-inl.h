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

  MergeSortFunctor(size_t chunk_first, size_t chunk_last,
                   RAITemp temporary_first, ComparisonFunction comparison,
                   const embb::mtapi::ExecutionPolicy& policy,
                   const BlockSizePartitioner<RAI>& partitioner,
                   const RAI& global_first, int depth)
    : chunk_first_(chunk_first), chunk_last_(chunk_last),
      temp_first_(temporary_first),
      comparison_(comparison), policy_(policy), partitioner_(partitioner),
      global_first_(global_first), depth_(depth) {
  }

  void Action(mtapi::TaskContext&) {
    typedef typename std::iterator_traits<RAI>::difference_type
      difference_type;
    size_t chunk_split_index = (chunk_first_ + chunk_last_) / 2;
    if (chunk_first_ == chunk_last_) {
      // Leaf case: recurse into a single chunk's elements:
      ChunkDescriptor<RAI> chunk = partitioner_[chunk_first_];
      MergeSortChunkFunctor functor(chunk.GetFirst(),
                                    chunk.GetLast(),
                                    temp_first_,
                                    global_first_,
                                    depth_);
      functor.Action();
      return;
    } 
    // Recurse further:
    // Split chunks into left / right branches:
    self_t functor_l(chunk_first_,
                     chunk_split_index,
                     temp_first_,
                     comparison_, policy_, partitioner_,
                     global_first_, depth_ + 1);
    self_t functor_r(chunk_split_index + 1,
                     chunk_last_,
                     temp_first_,
                     comparison_, policy_, partitioner_,
                     global_first_, depth_ + 1);
    mtapi::Node& node = mtapi::Node::GetInstance();
    mtapi::Task task_l = node.Spawn(
      mtapi::Action(
        base::MakeFunction(functor_l, &self_t::Action),
        policy_));
    mtapi::Task task_r = node.Spawn(
      mtapi::Action(
        base::MakeFunction(functor_r, &self_t::Action),
        policy_));
    task_l.Wait(MTAPI_INFINITE);
    task_r.Wait(MTAPI_INFINITE);

    ChunkDescriptor<RAI> chunk_f = partitioner_[chunk_first_];
    ChunkDescriptor<RAI> chunk_m = partitioner_[chunk_split_index + 1];
    ChunkDescriptor<RAI> chunk_l = partitioner_[chunk_last_];
    if(CloneBackToInput()) {
      difference_type first = std::distance(global_first_, chunk_f.GetFirst());
      difference_type mid   = std::distance(global_first_, chunk_m.GetFirst());
      difference_type last  = std::distance(global_first_, chunk_l.GetLast());
      SerialMerge(temp_first_ + first, temp_first_ + mid, temp_first_ + last,
                  chunk_f.GetFirst(),
                  comparison_);
    } else {
      SerialMerge(chunk_f.GetFirst(), chunk_m.GetFirst(), chunk_l.GetLast(),
                  temp_first_ + std::distance(global_first_, chunk_f.GetFirst()),
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
  typedef MergeSortFunctor<RAI, RAITemp, ComparisonFunction> self_t;

 private:
  /**
   * Non-parallelized part of merge sort on elements within a single chunk.
   */
  class MergeSortChunkFunctor {
   public:
    MergeSortChunkFunctor(RAI first, RAI last, 
                          RAITemp temp_first, 
                          const RAI & global_first,
                          int depth)
    : first_(first), last_(last),
      temp_first_(temp_first), global_first_(global_first),
      depth_(depth) {
    }

    void Action() {
      size_t distance = static_cast<size_t>(
        std::distance(first_, last_));
      if (distance <= 1) {
        // Leaf case:
        if(!CloneBackToInput() && distance != 0) {
          RAITemp temp_first = temp_first_;
          std::advance(temp_first, std::distance(global_first_, first_));
          *temp_first = *first_;
        }
        return;
      }
      // Recurse further. Use binary split, ignoring chunk size as this
      // recursion is serial:
      ChunkPartitioner<RAI> partitioner(first_, last_, 2);
      ChunkDescriptor<RAI> chunk_l = partitioner[0];
      ChunkDescriptor<RAI> chunk_r = partitioner[1];
      MergeSortChunkFunctor functor_l(
        chunk_l.GetFirst(),
        chunk_l.GetLast(),
        temp_first_, global_first_, depth_ + 1);
      MergeSortChunkFunctor functor_r(
        chunk_r.GetFirst(),
        chunk_r.GetLast(),
        temp_first_, global_first_, depth_ + 1);
      functor_l.Action();
      functor_r.Action();
    }

   private:
    /**
     * Determines the input and output arrays for one level in merge sort.
     *
     * \return \c true if the temporary data range is input and the array to be
     *         sorted is output. \c false, if the other way around.
     */
    bool CloneBackToInput() {
      return depth_ % 2 == 0 ? true : false;
    }

    RAI first_;
    RAI last_;
    RAITemp temp_first_;
    RAI global_first_;
    int depth_;
  };

 private:
  size_t chunk_first_;
  size_t chunk_last_;
  RAITemp temp_first_;
  ComparisonFunction comparison_;
  const embb::mtapi::ExecutionPolicy& policy_;
  const BlockSizePartitioner<RAI>& partitioner_;
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
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size
  ) {
  typedef typename std::iterator_traits<RAI>::difference_type difference_type;
  embb::mtapi::Node &node = embb::mtapi::Node::GetInstance();
  difference_type distance = last - first;
  assert(distance >= 0);

  if (block_size == 0) {
    block_size = (static_cast<size_t>(distance) / node.GetCoreCount());
    if (block_size == 0)
      block_size = 1;
  }
  if (((distance/block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Not enough MTAPI tasks available to perform the merge sort");
  }

  internal::BlockSizePartitioner<RAI> partitioner(first, last, block_size);

  internal::MergeSortFunctor<RAI, RAITemp, ComparisonFunction> functor(
      0, partitioner.Size() - 1,
      temporary_first,
      comparison,
      policy,
      partitioner,
      first,
      0);
  mtapi::Task task = node.Spawn(mtapi::Action(base::MakeFunction(functor,
    &internal::MergeSortFunctor<RAI, RAITemp, ComparisonFunction>::Action),
    policy));

  task.Wait(MTAPI_INFINITE);
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_
