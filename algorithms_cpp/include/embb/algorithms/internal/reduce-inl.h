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

#ifndef EMBB_ALGORITHMS_INTERNAL_REDUCE_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_REDUCE_INL_H_

#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>

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
  ReduceFunctor(RAI first, RAI last, ReturnType neutral,
                ReductionFunction reduction,
                TransformationFunction transformation,
                const embb::mtapi::ExecutionPolicy &policy, size_t block_size,
                ReturnType& result)
  :
      first_(first), last_(last), neutral_(neutral), reduction_(reduction),
      transformation_(transformation), policy_(policy),
      block_size_(block_size), result_(result) {
  }

  void Action(mtapi::TaskContext&) {
    if (first_ == last_) {
      return;
    }
    size_t distance = static_cast<size_t>(std::distance(first_, last_));
    if (distance <= block_size_) {  // leaf case -> do work
      ReturnType result(neutral_);
      for (RAI iter = first_; iter != last_; ++iter) {
        result = reduction_(result, transformation_(*iter));
      }
      result_ = result;
    } else {  // recurse further
      internal::ChunkPartitioner<RAI> partitioner(first_, last_, 2);
      ReturnType result_l(neutral_);
      ReturnType result_r(neutral_);
      ReduceFunctor functor_l(partitioner[0].GetFirst(),
                              partitioner[0].GetLast(),
                              neutral_, reduction_, transformation_, policy_,
                              block_size_, result_l);
      ReduceFunctor functor_r(partitioner[1].GetFirst(),
                              partitioner[1].GetLast(),
                              neutral_, reduction_, transformation_, policy_,
                              block_size_, result_r);
      mtapi::Node& node = mtapi::Node::GetInstance();
      mtapi::Task task_l = node.Spawn(mtapi::Action(base::MakeFunction(
          functor_l, &ReduceFunctor::Action), policy_));
      mtapi::Task task_r = node.Spawn(mtapi::Action(base::MakeFunction(
          functor_r, &ReduceFunctor::Action), policy_));
      task_l.Wait(MTAPI_INFINITE);
      task_r.Wait(MTAPI_INFINITE);
      result_ = reduction_(result_l, result_r);
    }
  }

 private:
  RAI first_;
  RAI last_;
  ReturnType neutral_;
  ReductionFunction reduction_;
  TransformationFunction transformation_;
  const embb::mtapi::ExecutionPolicy& policy_;
  size_t block_size_;
  ReturnType& result_;

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
  assert(distance > 0);

  mtapi::Node& node = mtapi::Node::GetInstance();
  size_t used_block_size = block_size;
  if (used_block_size == 0) {
      used_block_size = static_cast<size_t>(distance) / node.GetCoreCount();
      if (used_block_size == 0) used_block_size = 1;
  }

  if (((distance / used_block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Number of computation tasks required in reduction would "
               "exceed MTAPI maximum number of tasks");
  }

  ReturnType result = neutral;
  typedef ReduceFunctor<RAI, ReturnType, ReductionFunction,
                        TransformationFunction> Functor;
  Functor functor(first, last, neutral, reduction, transformation, policy,
                  used_block_size, result);
  mtapi::Task task = node.Spawn(mtapi::Action(base::MakeFunction(
      functor, &Functor::Action), policy));
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

template<typename RAI, typename ReturnType, typename ReductionFunction,
         typename TransformationFunction >
ReturnType Reduce(RAI first, RAI last, ReturnType neutral,
                  ReductionFunction reduction,
                  TransformationFunction transformation,
                  const embb::mtapi::ExecutionPolicy& policy,
                  size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  return internal::ReduceIteratorCheck(first, last, reduction, transformation,
                                       neutral, policy, block_size, category);
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_REDUCE_INL_H_
