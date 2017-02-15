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

#ifndef EMBB_ALGORITHMS_INTERNAL_FOR_EACH_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_FOR_EACH_INL_H_

#include <cassert>

#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>
#include <embb/algorithms/zip_iterator.h>
#include <embb/algorithms/internal/foreach_job_functor.h>

namespace embb {
namespace algorithms {

namespace internal {

template<typename RAI, typename Function>
class ForEachFunctor {
 public:
  /**
   * Constructs a for-each functor with arguments.
   */
  ForEachFunctor(size_t chunk_first, size_t chunk_last, Function unary,
                 const embb::mtapi::ExecutionPolicy& policy,
                 const BlockSizePartitioner<RAI>& partitioner)
  : chunk_first_(chunk_first), chunk_last_(chunk_last),
    unary_(unary), policy_(policy), partitioner_(partitioner) {
  }

  void Action(embb::mtapi::TaskContext&) {
    if (chunk_first_ == chunk_last_) {
      // Leaf case, recursed to single chunk. Do work on chunk:
      ChunkDescriptor<RAI> chunk = partitioner_[chunk_first_];
      RAI first = chunk.GetFirst();
      RAI last  = chunk.GetLast();
      for (RAI it = first; it != last; ++it) {
        unary_(*it);
      }
    } else {
      // Recurse further:
      size_t chunk_split_index = (chunk_first_ + chunk_last_) / 2;
      // Split chunks into left / right branches:
      self_t functor_l(chunk_first_,
                       chunk_split_index,
                       unary_, policy_, partitioner_);
      self_t functor_r(chunk_split_index + 1,
                       chunk_last_,
                       unary_, policy_, partitioner_);
      embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
      embb::mtapi::Task task_l = node.Start(
        embb::base::MakeFunction(functor_l, &self_t::Action),
        policy_);
      embb::mtapi::Task task_r = node.Start(
        embb::base::MakeFunction(functor_r, &self_t::Action),
        policy_);
      task_l.Wait(MTAPI_INFINITE);
      task_r.Wait(MTAPI_INFINITE);
    }
  }

 private:
  typedef ForEachFunctor<RAI, Function> self_t;

 private:
  size_t chunk_first_;
  size_t chunk_last_;
  Function unary_;
  const embb::mtapi::ExecutionPolicy& policy_;
  const BlockSizePartitioner<RAI>& partitioner_;

  /**
   * Disables assignment.
   */
  ForEachFunctor& operator=(const ForEachFunctor&);
};

template<typename RAI, typename Function>
void ForEachRecursive(RAI first, RAI last, Function unary,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAI>::difference_type difference_type;
  difference_type distance = std::distance(first, last);
  if (distance == 0) {
    return;
  } else if (distance < 0) {
    EMBB_THROW(embb::base::ErrorException, "Negative range for ForEach");
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
  // Check task number sufficiency
  if (((distance / block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException,
               "Not enough MTAPI tasks available for parallel foreach");
  }

  BlockSizePartitioner<RAI> partitioner(first, last, block_size);
  typedef ForEachFunctor<RAI, Function> functor_t;
  functor_t functor(0,
                    partitioner.Size() - 1,
                    unary, policy, partitioner);
  embb::mtapi::Task task = node.Start(
    embb::base::MakeFunction(functor, &functor_t::Action),
    policy);
  task.Wait(MTAPI_INFINITE);
}

template<typename RAI, typename Function>
void ForEachIteratorCheck(RAI first, RAI last, Function unary,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size,
                          std::random_access_iterator_tag) {
  return ForEachRecursive(first, last, unary, policy, block_size);
}

}  // namespace internal

template<typename RAI>
void ForEach(RAI first, const RAI last, embb::mtapi::Job unary,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  internal::ForEachIteratorCheck(first, last,
    internal::ForeachJobFunctor<
      typename std::iterator_traits<RAI>::value_type>(
        unary, policy),
    policy, block_size, category);
}

template<typename RAI, typename Function>
void ForEach(RAI first, const RAI last, Function unary,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typename std::iterator_traits<RAI>::iterator_category category;
  internal::ForEachIteratorCheck(first, last, unary, policy, block_size,
                                 category);
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_FOR_EACH_INL_H_
