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

#ifndef EMBB_ALGORITHMS_INTERNAL_FOR_EACH_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_FOR_EACH_INL_H_

#include <cassert>

#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#include <embb/algorithms/internal/partition.h>
#include <embb/algorithms/zip_iterator.h>

namespace embb {
namespace algorithms {

namespace internal {

template<typename RAI, typename Function>
class ForEachFunctor {
 private:
  typedef ForEachFunctor<RAI, Function> self_t;
 public:
  /**
   * Constructs a for-each functor with arguments.
   */
  ForEachFunctor(RAI first, RAI last, Function unary,
    const embb::mtapi::ExecutionPolicy& policy, size_t block_size)
    : first_(first), last_(last), unary_(unary), policy_(policy),
      block_size_(block_size) {
  }

  void Action(mtapi::TaskContext& context) {
    size_t distance = static_cast<size_t>(std::distance(first_, last_));
    if (distance == 0) return;
    if (distance <= block_size_) {  // leaf case -> do work
      for (RAI it = first_; it != last_; ++it) {
        unary_(*it);
      }
    } else {  // recurse further
      ChunkPartitioner<RAI> partitioner(first_, last_, 2);
      ChunkDescriptor<RAI> chunk_l = partitioner[0];
      ChunkDescriptor<RAI> chunk_r = partitioner[1];
      self_t functor_l(chunk_l.GetFirst(),
                       chunk_l.GetLast(),
                       unary_, policy_, block_size_);
      self_t functor_r(chunk_r.GetFirst(),
                       chunk_r.GetLast(),
                       unary_, policy_, block_size_);
      // Spawn tasks for right partition first:
      mtapi::Task task_r = mtapi::Node::GetInstance().Spawn(
        mtapi::Action(
          base::MakeFunction(
          functor_r, &ForEachFunctor<RAI, Function>::Action), 
          policy_));
      // Recurse on left partition:
      functor_l.Action(context);
      // Wait for tasks on right partition to complete:
      task_r.Wait(MTAPI_INFINITE);
    }
  }

 private:
  RAI first_;
  RAI last_;
  Function unary_;
  const embb::mtapi::ExecutionPolicy& policy_;
  size_t block_size_;

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
    EMBB_THROW(embb::base::ErrorException, "Distance for ForEach is 0");
  }
  mtapi::Node& node = mtapi::Node::GetInstance();
  // Determine actually used block size
  if (block_size == 0) {
    block_size = (static_cast<size_t>(distance) / node.GetCoreCount());
    if (block_size == 0) {
      block_size = 1;
    }
  }
  // Perform check of task number sufficiency
  if (((distance / block_size) * 2) + 1 > MTAPI_NODE_MAX_TASKS_DEFAULT) {
    EMBB_THROW(embb::base::ErrorException, "Not enough MTAPI tasks available "
               "to perform the parallel foreach loop");
  }
  ForEachFunctor<RAI, Function> functor(first, last, unary, policy, block_size);
  mtapi::Task task = node.Spawn(mtapi::Action(
                     base::MakeFunction(functor,
                       &ForEachFunctor<RAI, Function>::Action),
                     policy));
  task.Wait(MTAPI_INFINITE);
}

template<typename RAI, typename Function>
void ForEachIteratorCheck(RAI first, RAI last, Function unary,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size,
                          std::random_access_iterator_tag) {
  return ForEachRecursive(first, last, unary, policy, block_size);
}

}  // namespace internal

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
