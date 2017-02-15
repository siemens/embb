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

#ifndef EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_

#include <cassert>
#include <iterator>
#include <functional>

#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>
#include <embb/algorithms/internal/comparison_job_functor.h>

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

  void Action(embb::mtapi::TaskContext&) {
    size_t chunk_split_index = (chunk_first_ + chunk_last_) / 2;
    if (chunk_first_ == chunk_last_) {
      // Leaf case: recurse into a single chunk's elements:
      ChunkDescriptor<RAI> chunk = partitioner_[chunk_first_];
      MergeSortChunk(chunk.GetFirst(), chunk.GetLast(), depth_);
    } else {
      // Recurse further, split chunks:
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
      embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
      embb::mtapi::Task task_l = node.Start(
        base::MakeFunction(functor_l, &self_t::Action),
        policy_);
      embb::mtapi::Task task_r = node.Start(
        base::MakeFunction(functor_r, &self_t::Action),
        policy_);
      task_l.Wait(MTAPI_INFINITE);
      task_r.Wait(MTAPI_INFINITE);

      ChunkDescriptor<RAI> ck_f = partitioner_[chunk_first_];
      ChunkDescriptor<RAI> ck_m = partitioner_[chunk_split_index + 1];
      ChunkDescriptor<RAI> ck_l = partitioner_[chunk_last_];
      if(CloneBackToInput(depth_)) {
        // Merge from temp into input:
        difference_type first = std::distance(global_first_, ck_f.GetFirst());
        difference_type mid   = std::distance(global_first_, ck_m.GetFirst());
        difference_type last  = std::distance(global_first_, ck_l.GetLast());
        SerialMerge(temp_first_ + first, temp_first_ + mid, temp_first_ + last,
                    ck_f.GetFirst(),
                    comparison_);
      } else {
        // Merge from input into temp:
        SerialMerge(ck_f.GetFirst(), ck_m.GetFirst(), ck_l.GetLast(),
                    temp_first_ + std::distance(global_first_, ck_f.GetFirst()),
                    comparison_);
      }
    }
  }

  /**
   * Serial merge sort of elements within a single chunk.
   */
  void MergeSortChunk(RAI first,
                      RAI last,
                      int depth) {
    size_t distance = static_cast<size_t>(
      std::distance(first, last));
    if (distance <= 1) {
      // Leaf case:
      if (!CloneBackToInput(depth) && distance != 0) {
        RAITemp temp_first = temp_first_;
        std::advance(temp_first, std::distance(global_first_, first));
        *temp_first = *first;
      }
      return;
    }
    // Recurse further. Use binary split, ignoring chunk size as this
    // recursion is serial and has leaf size 1:
    ChunkPartitioner<RAI> partitioner(first, last, 2);
    ChunkDescriptor<RAI> ck_l = partitioner[0];
    ChunkDescriptor<RAI> ck_r = partitioner[1];
    MergeSortChunk(
      ck_l.GetFirst(),
      ck_l.GetLast(),
      depth + 1);
    MergeSortChunk(
      ck_r.GetFirst(),
      ck_r.GetLast(),
      depth + 1);
    if (CloneBackToInput(depth)) {
      // Merge from temp into input:
      difference_type d_first = std::distance(global_first_, ck_l.GetFirst());
      difference_type d_mid   = std::distance(global_first_, ck_r.GetFirst());
      difference_type d_last  = std::distance(global_first_, ck_r.GetLast());
      SerialMerge(
        temp_first_ + d_first, temp_first_ + d_mid, temp_first_ + d_last,
        ck_l.GetFirst(),
        comparison_);
    } else {
      // Merge from input into temp:
      SerialMerge(
        ck_l.GetFirst(), ck_r.GetFirst(), ck_r.GetLast(),
        temp_first_ + std::distance(global_first_, ck_l.GetFirst()),
        comparison_);
    }
  }

  /**
   * Determines the input and output arrays for one level in merge sort.
   *
   * \return \c true if the temporary data range is input and the array to be
   *         sorted is output. \c false, if the other way around.
   */
  bool CloneBackToInput(int depth) {
    return depth % 2 == 0 ? true : false;
  }

 private:
  typedef MergeSortFunctor<RAI, RAITemp, ComparisonFunction> self_t;
  typedef typename std::iterator_traits<RAI>::difference_type
    difference_type;

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

template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSortIteratorCheck(
  RAI first,
  RAI last,
  RAITemp temporary_first,
  ComparisonFunction comparison,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size,
  std::random_access_iterator_tag
  ) {
  typedef typename std::iterator_traits<RAI>::difference_type difference_type;
  typedef MergeSortFunctor<RAI, RAITemp, ComparisonFunction>
    functor_t;
  difference_type distance = std::distance(first, last);
  if (distance == 0) {
    return;
  } else if (distance < 0) {
    EMBB_THROW(embb::base::ErrorException, "Negative range for MergeSort");
  }
  unsigned int num_cores = policy.GetCoreCount();
  if (num_cores == 0) {
    EMBB_THROW(embb::base::ErrorException, "No cores in execution policy");
  }
  // Determine actually used block size
  if (block_size == 0) {
    block_size = (static_cast<size_t>(distance) / num_cores);
    if (block_size == 0)
      block_size = 1;
  }
  // Check task number sufficiency
  if (((distance / block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Not enough MTAPI tasks available to perform merge sort");
  }

  BlockSizePartitioner<RAI> partitioner(first, last, block_size);
  functor_t functor(0,
                    partitioner.Size() - 1,
                    temporary_first,
                    comparison,
                    policy,
                    partitioner,
                    first,
                    0);
  embb::mtapi::Task task = embb::mtapi::Node::GetInstance().Start(
    base::MakeFunction(functor, &functor_t::Action),
    policy);

  task.Wait(MTAPI_INFINITE);
}

}  // namespace internal

template<typename RAI, typename RAITemp>
void MergeSort(RAI first, RAI last, RAITemp temporary_first,
  embb::mtapi::Job comparison, const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size) {
  typedef typename std::iterator_traits<RAI>::iterator_category category;
  internal::MergeSortIteratorCheck(first, last, temporary_first,
    internal::ComparisonJobFunctor<
      typename std::iterator_traits<RAI>::value_type>(
        comparison, policy),
    policy, block_size, category());
}

template<typename RAI, typename RAITemp, typename ComparisonFunction>
void MergeSort(RAI first, RAI last, RAITemp temporary_first,
  ComparisonFunction comparison, const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size) {
  typedef typename std::iterator_traits<RAI>::iterator_category category;
  internal::MergeSortIteratorCheck(first, last, temporary_first, comparison,
    policy, block_size, category());
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_MERGE_SORT_INL_H_
