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

#ifndef EMBB_ALGORITHMS_INTERNAL_REDUCE_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_REDUCE_INL_H_

#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>
#include <embb/algorithms/internal/transformation_job_functor.h>
#include <embb/algorithms/internal/reduce_job_functor.h>

#include <functional>
#include <embb/base/exceptions.h>
#include <cassert>

namespace embb {
namespace algorithms {
namespace internal {

template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
class ReduceFunctor {
 public:
  ReduceFunctor(size_t chunk_first, size_t chunk_last,
                ReturnType neutral,
                ReductionFunction reduction,
                TransformationFunction transformation,
                const embb::mtapi::ExecutionPolicy& policy,
                const BlockSizePartitioner<RAI>& partitioner,
                ReturnType& result)
  : chunk_first_(chunk_first), chunk_last_(chunk_last), neutral_(neutral),
    reduction_(reduction), transformation_(transformation), policy_(policy),
    partitioner_(partitioner), result_(result) {
  }

  void Action(embb::mtapi::TaskContext&) {
    if (chunk_first_ == chunk_last_) {
      // Leaf case, recursed to single chunk. Do work on chunk:
      ChunkDescriptor<RAI> chunk = partitioner_[chunk_first_];
      RAI first = chunk.GetFirst();
      RAI last  = chunk.GetLast();
      ReturnType result(neutral_);
      for (RAI it = first; it != last; ++it) {
        result = reduction_(result, transformation_(*it));
      }
      result_ = result;
    } else {
      // Recurse further:
      size_t chunk_split_index = (chunk_first_ + chunk_last_) / 2;
      // Split chunks into left / right branches:
      ReturnType result_l(neutral_);
      ReturnType result_r(neutral_);
      self_t functor_l(chunk_first_,
                       chunk_split_index,
                       neutral_, reduction_, transformation_, policy_,
                       partitioner_,
                       result_l);
      self_t functor_r(chunk_split_index + 1,
                       chunk_last_,
                       neutral_, reduction_, transformation_, policy_,
                       partitioner_,
                       result_r);
      embb::mtapi::Task task_l = embb::mtapi::Node::GetInstance().Start(
        base::MakeFunction(functor_l, &self_t::Action),
        policy_);
      embb::mtapi::Task task_r = embb::mtapi::Node::GetInstance().Start(
        base::MakeFunction(functor_r, &self_t::Action),
        policy_);
      task_l.Wait(MTAPI_INFINITE);
      task_r.Wait(MTAPI_INFINITE);
      result_ = reduction_(result_l, result_r);
    }
  }

 private:
  typedef ReduceFunctor<RAI, ReturnType,
                        ReductionFunction,
                        TransformationFunction> self_t;

 private:
  size_t chunk_first_;
  size_t chunk_last_;
  ReturnType neutral_;
  ReductionFunction reduction_;
  TransformationFunction transformation_;
  const embb::mtapi::ExecutionPolicy& policy_;
  const BlockSizePartitioner<RAI>& partitioner_;
  ReturnType& result_;

  /**
   * Disables assignment and copy-construction.
   */
  ReduceFunctor& operator=(const ReduceFunctor&);
  ReduceFunctor(const ReduceFunctor&);
};

template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
ReturnType ReduceRecursive(RAI first, RAI last, ReturnType neutral,
                           ReductionFunction reduction,
                           TransformationFunction transformation,
                           const embb::mtapi::ExecutionPolicy& policy,
                           size_t block_size) {
  typedef typename std::iterator_traits<RAI>::difference_type difference_type;
  difference_type distance = std::distance(first, last);
  if (distance == 0) {
    return neutral;
  } else if (distance < 0) {
    EMBB_THROW(embb::base::ErrorException, "Negative range for Reduce");
  }
  unsigned int num_cores = policy.GetCoreCount();
  if (num_cores == 0) {
    EMBB_THROW(embb::base::ErrorException, "No cores in execution policy");
  }
  embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
  // Determine actually used block size
  if (block_size == 0) {
    block_size = (static_cast<size_t>(distance) / num_cores);
    if (block_size == 0) {
      block_size = 1;
    }
  }
  // Perform check of task number sufficiency
  if (((distance / block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Number of computation tasks required in reduction would "
               "exceed MTAPI maximum number of tasks");
  }
  typedef ReduceFunctor<RAI, ReturnType, ReductionFunction,
                        TransformationFunction> Functor;
  BlockSizePartitioner<RAI> partitioner(first, last, block_size);
  ReturnType result = neutral;
  Functor functor(0,
                  partitioner.Size() - 1,
                  neutral,
                  reduction, transformation,
                  policy,
                  partitioner,
                  result);
  embb::mtapi::Task task = node.Start(
    base::MakeFunction(functor, &Functor::Action),
    policy);
  task.Wait(MTAPI_INFINITE);
  return result;
}

template<typename RAI, typename TransformationFunction,
  typename ReductionFunction, typename ReturnType>
ReturnType ReduceIteratorCheck(RAI first, RAI last, ReductionFunction reduction,
                               TransformationFunction transformation,
                               ReturnType neutral,
                               const embb::mtapi::ExecutionPolicy& policy,
                               size_t block_size,
                               std::random_access_iterator_tag) {
    return ReduceRecursive(first, last, neutral, reduction, transformation,
                           policy, block_size);
}

}  // namespace internal

template<typename RAI, typename ReturnType>
ReturnType Reduce(RAI first, RAI last, ReturnType neutral,
  embb::mtapi::Job reduction,
  embb::mtapi::Job transformation,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  return internal::ReduceIteratorCheck(
    first, last,
    internal::ReductionJobFunctor<
      typename std::iterator_traits<RAI>::value_type>(
        reduction, policy),
    internal::TransformationJobFunctor<ReturnType,
      typename std::iterator_traits<RAI>::value_type>(transformation, policy),
    neutral, policy, block_size, category);
}

template<typename RAI, typename ReturnType, typename ReductionFunction>
ReturnType Reduce(
  RAI first, RAI last, ReturnType neutral,
  ReductionFunction reduction,
  embb::mtapi::Job transformation,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  return internal::ReduceIteratorCheck(
    first, last,
    reduction,
    internal::TransformationJobFunctor<ReturnType,
      typename std::iterator_traits<RAI>::value_type>(transformation, policy),
    neutral, policy, block_size, category);
}

template<typename RAI, typename ReturnType, typename TransformationFunction>
ReturnType Reduce(
  RAI first, RAI last, ReturnType neutral,
  embb::mtapi::Job reduction,
  TransformationFunction transformation,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  return internal::ReduceIteratorCheck(
    first, last,
    internal::ReductionJobFunctor<
      typename std::iterator_traits<RAI>::value_type>(
        reduction, policy),
    transformation,
    neutral, policy, block_size, category);
}


template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction>
ReturnType Reduce(
  RAI first, RAI last, ReturnType neutral,
  ReductionFunction reduction,
  TransformationFunction transformation,
  const embb::mtapi::ExecutionPolicy& policy,
  size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  return internal::ReduceIteratorCheck(
    first, last,
    reduction,
    transformation,
    neutral, policy, block_size, category);
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_REDUCE_INL_H_
