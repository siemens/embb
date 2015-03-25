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

#include <cstddef>

#include <embb/base/memory_allocation.h>
#include <embb/base/function.h>
#include <embb/tasks/tasks.h>

#include <continuationstage.h>

namespace embb {
namespace tasks {

Continuation::Continuation(Action action) {
  first_ = last_ = embb::base::Allocation::New<ContinuationStage>();
  first_->action = action;
  first_->next = NULL;
}

Continuation::Continuation(Continuation const & cont)
  : first_(cont.first_)
  , last_(cont.last_) {
}

Continuation::~Continuation() {
}

void Continuation::ExecuteContinuation(TaskContext &) {
  ContinuationStage * stage = first_;
  Node & node = Node::GetInstance();
  while (NULL != stage) {
    Task task = node.Spawn(stage->action);
    task.Wait(MTAPI_INFINITE);
    stage = stage->next;
  }

  // delete stages
  stage = first_;
  while (NULL != stage) {
    ContinuationStage * next = stage->next;
    embb::base::Allocation::Delete(stage);
    stage = next;
  }
}

Continuation & Continuation::Then(Action action) {
  ContinuationStage * cur = embb::base::Allocation::New<ContinuationStage>();
  cur->action = action;
  cur->next = NULL;

  last_->next = cur;
  last_ = cur;

  return *this;
}

Task Continuation::Spawn() {
  return Spawn(ExecutionPolicy());
}

Task Continuation::Spawn(ExecutionPolicy execution_policy) {
  Node & node = Node::GetInstance();
  return node.Spawn(
    Action(
      embb::base::MakeFunction(*this, &Continuation::ExecuteContinuation),
      ExecutionPolicy(execution_policy)));
}

} // namespace tasks
} // namespace embb
