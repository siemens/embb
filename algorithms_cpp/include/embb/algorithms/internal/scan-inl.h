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

#ifndef EMBB_ALGORITHMS_INTERNAL_SCAN_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_SCAN_INL_H_

#include <cassert>
#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#include <embb/mtapi/execution_policy.h>
#include <embb/algorithms/internal/partition.h>

namespace embb {
namespace algorithms {
namespace internal {

template<typename RAIIn, typename RAIOut, typename ReturnType,
typename ScanFunction, typename TransformationFunction>
class ScanFunctor {
 public:
  ScanFunctor(RAIIn first, RAIIn last, RAIOut output_iterator,
              ReturnType neutral, ScanFunction scan,
              TransformationFunction transformation,
              const embb::mtapi::ExecutionPolicy& policy,
              size_t block_size, ReturnType* tree_values, size_t node_id,
              bool going_down)
    : policy_(policy), first_(first), last_(last),
      output_iterator_(output_iterator), scan_(scan),
      transformation_(transformation),
      neutral_(neutral), block_size_(block_size), tree_values_(tree_values),
      node_id_(node_id), parent_value_(neutral), is_first_pass_(going_down)  {
  }

  void Action(mtapi::TaskContext&) {
    if (first_ == last_) {
      return;
    }
    size_t distance = static_cast<size_t>(std::distance(first_, last_));
    if (distance <= block_size_) {  // leaf case -> do work
      if (is_first_pass_) {
        RAIIn iter_in = first_;
        RAIOut iter_out = output_iterator_;
        ReturnType result = transformation_(*first_);
        *iter_out = result;
        ++iter_in;
        ++iter_out;
        while (iter_in != last_) {
          result = scan_(result, transformation_(*iter_in));
          *iter_out = result;
          ++iter_in;
          ++iter_out;
        }
        SetTreeValue(result);
      } else { // Second pass
        RAIIn iter_in = first_;
        RAIOut iter_out = output_iterator_;
        while (iter_in != last_) {
          *iter_out = scan_(parent_value_, *iter_out);
          ++iter_in;
          ++iter_out;
        }
      }
    } else {
      internal::ChunkPartitioner<RAIIn> partitioner(first_, last_, 2);
      ScanFunctor functor_l(partitioner[0].GetFirst(), partitioner[0].GetLast(),
                            output_iterator_, neutral_, scan_, transformation_,
                            policy_, block_size_, tree_values_, node_id_,
                            is_first_pass_);
      ScanFunctor functor_r(partitioner[1].GetFirst(), partitioner[1].GetLast(),
                            output_iterator_, neutral_, scan_, transformation_,
                            policy_, block_size_, tree_values_, node_id_,
                            is_first_pass_);
      functor_l.SetID(1);
      functor_r.SetID(2);
      std::advance(functor_r.output_iterator_,
                   std::distance(functor_l.first_, functor_r.first_));
      if (!is_first_pass_) {
        functor_l.parent_value_ = parent_value_;
        functor_r.parent_value_ = functor_l.GetTreeValue() + parent_value_;
      }
      mtapi::Node& node = mtapi::Node::GetInstance();
      mtapi::Task task_l = node.Spawn(mtapi::Action(base::MakeFunction(
                                      functor_l, &ScanFunctor::Action),
                                      policy_));
       mtapi::Task task_r = node.Spawn(mtapi::Action(base::MakeFunction(
                                       functor_r, &ScanFunctor::Action),
                                       policy_));
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
  const embb::mtapi::ExecutionPolicy& policy_;
  RAIIn first_;
  RAIIn last_;
  RAIOut output_iterator_;
  ScanFunction scan_;
  TransformationFunction transformation_;
  ReturnType neutral_;
  size_t block_size_;
  ReturnType* tree_values_;
  size_t node_id_;
  ReturnType parent_value_;
  bool is_first_pass_;

  void SetID(int is_left) {
    if (is_left == 1) {
      node_id_ = 2 * node_id_ + 1;
    } else if (is_left == 2) {
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
  if (distance <= 0) {
    return;
  }
  mtapi::Node& node = mtapi::Node::GetInstance();
  ReturnType values[MTAPI_NODE_MAX_TASKS_DEFAULT];
  size_t used_block_size = block_size;
  if (block_size == 0) {
    used_block_size = static_cast<size_t>(distance) / node.GetCoreCount();
    if (used_block_size == 0) used_block_size = 1;
  }
  if (((distance / used_block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Number of computation tasks required in scan "
               "exceeds MTAPI maximum number of tasks");
  }

  // first pass. Calculates prefix sums for leaves and when recursion returns
  // it creates the tree.
  typedef ScanFunctor<RAIIn, RAIOut, ReturnType, ScanFunction,
                      TransformationFunction> Functor;
  Functor functor_down(first, last, output_iterator, neutral, scan,
                       transformation, policy, used_block_size, values, 0,
                       true);
  mtapi::Task task_down = node.Spawn(mtapi::Action(base::MakeFunction(
                          functor_down, &Functor::Action),
                          policy));
  task_down.Wait(MTAPI_INFINITE);

  // Second pass. Gives to each leaf the part of the prefix missing
  Functor functor_up(first, last, output_iterator, neutral, scan,
                     transformation, policy, used_block_size, values, 0, false);
  mtapi::Task task_up = node.Spawn(mtapi::Action(base::MakeFunction(
                        functor_up, &Functor::Action),
                        policy));
  task_up.Wait(MTAPI_INFINITE);
}

}  // namespace internal

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
