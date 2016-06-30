/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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

#ifndef EMBB_DATAFLOW_INTERNAL_SCHEDULER_MTAPI_H_
#define EMBB_DATAFLOW_INTERNAL_SCHEDULER_MTAPI_H_

#include <embb/dataflow/internal/action.h>
#include <embb/dataflow/internal/scheduler.h>
#include <embb/mtapi/mtapi.h>
#include <embb/base/function.h>

#include <algorithm>

namespace embb {
namespace dataflow {
namespace internal {

#define EMBB_DATAFLOW_JOB_ID 1

class SchedulerMTAPI : public Scheduler {
 public:
  explicit SchedulerMTAPI(int slices)
    : slices_(slices) {
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

    int tl = std::min(
      static_cast<int>(node.GetTaskLimit()),
      static_cast<int>(node.GetGroupCount()));
    if (tl < slices_) {
      slices_ = tl;
    }

    job_ = node.GetJob(EMBB_DATAFLOW_JOB_ID);
    action_ = node.CreateAction(EMBB_DATAFLOW_JOB_ID,
      SchedulerMTAPI::action_func);

    group_ = reinterpret_cast<embb::mtapi::Group*>(
      embb::base::Allocation::Allocate(
      sizeof(embb::mtapi::Group)*slices_));

    for (int ii = 0; ii < slices_; ii++) {
      group_[ii] = node.CreateGroup();
    }

    queue_count_ = std::min(
      static_cast<int>(node.GetQueueCount()),
      static_cast<int>(node.GetWorkerThreadCount()) );
    queue_ = reinterpret_cast<embb::mtapi::Queue*>(
      embb::base::Allocation::Allocate(
      sizeof(embb::mtapi::Queue)*queue_count_));

    embb::mtapi::QueueAttributes queue_attr;
    queue_attr
      .SetPriority(0)
      .SetOrdered(true);
    for (int ii = 0; ii < queue_count_; ii++) {
      queue_[ii] = node.CreateQueue(job_, queue_attr);
    }
  }
  virtual ~SchedulerMTAPI() {
    if (embb::mtapi::Node::IsInitialized()) {
      // only destroy groups and queues if there still is an instance
      for (int ii = 0; ii < slices_; ii++) {
        group_[ii].WaitAll(MTAPI_INFINITE);
        group_[ii].Delete();
      }
      for (int ii = 0; ii < queue_count_; ii++) {
        queue_[ii].Delete();
      }
      // delete action as well
      action_.Delete();
    }
    embb::base::Allocation::Free(group_);
    embb::base::Allocation::Free(queue_);
  }
  virtual void Start(
    Action & action,
    embb::mtapi::ExecutionPolicy const & policy) {
    const int idx = action.GetClock() % slices_;
    embb::mtapi::TaskAttributes task_attr;
    task_attr.SetPolicy(policy);
    group_[idx].Start(job_, &action, static_cast<void*>(NULL),
      task_attr);
  }
  virtual void Enqueue(
    int process_id,
    Action & action,
    embb::mtapi::ExecutionPolicy const & policy) {
    const int idx = action.GetClock() % slices_;
    const int queue_id = process_id % queue_count_;
    embb::mtapi::TaskAttributes task_attr;
    task_attr.SetPolicy(policy);
    queue_[queue_id].Enqueue(&action, static_cast<void*>(NULL),
      task_attr, group_[idx]);
  }
  virtual void WaitForSlice(int slice) {
    group_[slice].WaitAll(MTAPI_INFINITE);
    // group is invalid now, recreate
    embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
    group_[slice] = node.CreateGroup();
  }
  virtual int GetSlices() { return slices_; }

 private:
  static void action_func(
    const void* args,
    mtapi_size_t /*args_size*/,
    void* /*result_buffer*/,
    mtapi_size_t /*result_buffer_size*/,
    const void* /*node_local_data*/,
    mtapi_size_t /*node_local_data_size*/,
    mtapi_task_context_t * context) {
    Action * action =
      reinterpret_cast<Action*>(const_cast<void*>(args));
    embb::mtapi::TaskContext task_context(context);
    action->RunMTAPI(task_context);
  }

  embb::mtapi::Action action_;
  embb::mtapi::Job job_;
  embb::mtapi::Group * group_;
  embb::mtapi::Queue * queue_;
  int queue_count_;
  int slices_;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_SCHEDULER_MTAPI_H_
