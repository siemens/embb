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

#ifndef EMBB_ALGORITHMS_INTERNAL_SCAN_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_SCAN_INL_H_

#include <cassert>
#include <embb/base/exceptions.h>
#include <embb/base/function.h>
#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>
#include <embb/algorithms/internal/transformation_job_functor.h>
#include <embb/algorithms/internal/scan_job_functor.h>

namespace embb {
namespace algorithms {
namespace internal {

template<typename RAIIn, typename RAIOut, typename ReturnType,
typename ScanFunction, typename TransformationFunction>
class ScanFunctor {
 public:
  ScanFunctor(size_t chunk_first, size_t chunk_last, RAIOut output_iterator,
              ReturnType neutral, ScanFunction scan,
              TransformationFunction transformation,
              const embb::mtapi::ExecutionPolicy& policy,
              const BlockSizePartitioner<RAIIn>& partitioner,
              ReturnType* tree_values, size_t node_id,
              bool going_down)
    : policy_(policy), chunk_first_(chunk_first), chunk_last_(chunk_last),
      output_iterator_(output_iterator), scan_(scan),
      transformation_(transformation),
      neutral_(neutral), partitioner_(partitioner), tree_values_(tree_values),
      node_id_(node_id), parent_value_(neutral), is_first_pass_(going_down)  {
  }

  void Action(embb::mtapi::TaskContext&) {
    if (chunk_first_ == chunk_last_) {
      ChunkDescriptor<RAIIn> chunk = partitioner_[chunk_first_];
      RAIIn iter_in = chunk.GetFirst();
      RAIIn last_in = chunk.GetLast();
      RAIOut iter_out = output_iterator_;
      // leaf case -> do work
      if (is_first_pass_) {
        ReturnType result = transformation_(*iter_in);
        *iter_out = result;
        ++iter_in;
        ++iter_out;
        for (; iter_in != last_in; ++iter_in, ++iter_out) {
          result = scan_(result, transformation_(*iter_in));
          *iter_out = result;
        }
        SetTreeValue(result);
      } else {
        // Second pass
        for (; iter_in != last_in; ++iter_in, ++iter_out) {
          *iter_out = scan_(parent_value_, *iter_out);
        }
      }
    } else {
      // recurse further
      size_t chunk_split_index = (chunk_first_ + chunk_last_) / 2;
      // Split chunks into left / right branches:
      ScanFunctor functor_l(
        chunk_first_, chunk_split_index,
        output_iterator_, neutral_, scan_, transformation_,
        policy_, partitioner_, tree_values_, node_id_,
        is_first_pass_);
      ScanFunctor functor_r(
        chunk_split_index + 1, chunk_last_,
        output_iterator_, neutral_, scan_, transformation_,
        policy_, partitioner_, tree_values_, node_id_,
        is_first_pass_);
      functor_l.SetID(LEFT);
      functor_r.SetID(RIGHT);
      // Advance output iterator of right branch:
      ChunkDescriptor<RAIIn> chunk_left  = partitioner_[chunk_first_];
      ChunkDescriptor<RAIIn> chunk_right = partitioner_[chunk_split_index + 1];
      std::advance(functor_r.output_iterator_,
          std::distance(chunk_left.GetFirst(), chunk_right.GetFirst()));
      if (!is_first_pass_) {
        functor_l.parent_value_ = parent_value_;
        functor_r.parent_value_ = functor_l.GetTreeValue() + parent_value_;
      }
      // Spawn tasks to recurse:
      embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
      embb::mtapi::Task task_l = node.Start(
        base::MakeFunction(functor_l, &ScanFunctor::Action),
        policy_);
      embb::mtapi::Task task_r = node.Start(
        base::MakeFunction(functor_r, &ScanFunctor::Action),
        policy_);
      // Wait for tasks to complete:
      task_l.Wait(MTAPI_INFINITE);
      task_r.Wait(MTAPI_INFINITE);
      SetTreeValue(scan_(functor_l.GetTreeValue(), functor_r.GetTreeValue()));
    }
  }

  ReturnType GetTreeValue() {
    return tree_values_[node_id_];
  }

  void SetTreeValue(ReturnType value) {
    tree_values_[node_id_] = value;
  }

 private:
  static const int LEFT  = 1;
  static const int RIGHT = 2;
  const embb::mtapi::ExecutionPolicy& policy_;
  size_t chunk_first_;
  size_t chunk_last_;
  RAIOut output_iterator_;
  ScanFunction scan_;
  TransformationFunction transformation_;
  ReturnType neutral_;
  const BlockSizePartitioner<RAIIn>& partitioner_;
  ReturnType* tree_values_;
  size_t node_id_;
  ReturnType parent_value_;
  bool is_first_pass_;

  void SetID(int branch) {
    if (branch == LEFT) {
      node_id_ = 2 * node_id_ + 1;
    } else if (branch == RIGHT) {
      node_id_ = 2 * node_id_ + 2;
    }
  }

  /**
   * Disables assignment.
   */
  ScanFunctor& operator=(const ScanFunctor&);

  /**
   * Disables copying.
   */
  ScanFunctor(const ScanFunctor&);
};

template<typename RAIIn, typename RAIOut, typename ReturnType,
typename ScanFunction, typename TransformationFunction>
void ScanIteratorCheck(RAIIn first, RAIIn last, RAIOut output_iterator,
                       ReturnType neutral, ScanFunction scan,
                       TransformationFunction transformation,
                       const embb::mtapi::ExecutionPolicy& policy,
                       size_t block_size,
                       std::random_access_iterator_tag) {
  typedef typename std::iterator_traits<RAIIn>::difference_type difference_type;
  difference_type distance = std::distance(first, last);
  if (distance == 0) {
    return;
  } else if (distance < 0) {
    EMBB_THROW(embb::base::ErrorException, "Negative range for Scan");
  }
  unsigned int num_cores = policy.GetCoreCount();
  if (num_cores == 0) {
    EMBB_THROW(embb::base::ErrorException, "No cores in execution policy");
  }

  ReturnType values[MTAPI_NODE_MAX_TASKS_DEFAULT];
  if (block_size == 0) {
    block_size = static_cast<size_t>(distance) / num_cores;
    if (block_size == 0) {
      block_size = 1;
    }
  }
  if (((distance / block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Not enough MTAPI tasks available for parallel scan");
  }

  // first pass. Calculates prefix sums for leaves and when recursion returns
  // it creates the tree.
  typedef ScanFunctor<RAIIn, RAIOut, ReturnType, ScanFunction,
                      TransformationFunction> Functor;
  embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();

  BlockSizePartitioner<RAIIn> partitioner_down(first, last, block_size);
  Functor functor_down(0, partitioner_down.Size() - 1, output_iterator,
                       neutral, scan, transformation, policy, partitioner_down,
                       values, 0, true);
  embb::mtapi::Task task_down = node.Start(
    base::MakeFunction(functor_down, &Functor::Action),
    policy);
  task_down.Wait(MTAPI_INFINITE);

  // Second pass. Gives to each leaf the part of the prefix missing
  BlockSizePartitioner<RAIIn> partitioner_up(first, last, block_size);
  Functor functor_up(0, partitioner_up.Size() - 1, output_iterator,
                     neutral, scan, transformation, policy, partitioner_up,
                     values, 0, false);
  embb::mtapi::Task task_up = node.Start(
    base::MakeFunction(functor_up, &Functor::Action),
    policy);
  task_up.Wait(MTAPI_INFINITE);
}

}  // namespace internal

template<typename RAIIn, typename RAIOut, typename ReturnType>
void Scan(RAIIn first, RAIIn last, RAIOut output_iterator, ReturnType neutral,
          embb::mtapi::Job scan,
          embb::mtapi::Job transformation,
          const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAIIn>::iterator_category category;
  internal::ScanIteratorCheck(first, last, output_iterator, neutral,
    internal::ScanJobFunctor<ReturnType>(scan, policy),
    internal::TransformationJobFunctor<ReturnType,
      typename std::iterator_traits<RAIIn>::value_type>(
        transformation, policy),
    policy, block_size, category());
}

template<typename RAIIn, typename RAIOut, typename ReturnType,
         typename ScanFunction>
void Scan(RAIIn first, RAIIn last, RAIOut output_iterator, ReturnType neutral,
          ScanFunction scan, embb::mtapi::Job transformation,
          const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAIIn>::iterator_category category;
  internal::ScanIteratorCheck(first, last, output_iterator, neutral,
    scan, internal::TransformationJobFunctor<ReturnType,
      typename std::iterator_traits<RAIIn>::value_type>(
        transformation, policy),
    policy, block_size, category());
}

template<typename RAIIn, typename RAIOut, typename ReturnType,
         typename TransformationFunction>
void Scan(RAIIn first, RAIIn last, RAIOut output_iterator, ReturnType neutral,
          embb::mtapi::Job scan, TransformationFunction transformation,
          const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAIIn>::iterator_category category;
  internal::ScanIteratorCheck(first, last, output_iterator, neutral,
    internal::ScanJobFunctor<ReturnType>(scan, policy), transformation,
    policy, block_size, category());
}

template<typename RAIIn, typename RAIOut, typename ReturnType,
         typename ScanFunction, typename TransformationFunction>
void Scan(RAIIn first, RAIIn last, RAIOut output_iterator, ReturnType neutral,
          ScanFunction scan, TransformationFunction transformation,
          const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAIIn>::iterator_category category;
  internal::ScanIteratorCheck(first, last, output_iterator, neutral,
      scan, transformation, policy, block_size, category());
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_SCAN_INL_H_
