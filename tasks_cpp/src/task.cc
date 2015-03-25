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

#include <cstring>
#include <cassert>

#include <embb/base/memory_allocation.h>
#include <embb/base/exceptions.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace tasks {

Task::Task() {
  handle_.id = 0;
  handle_.tag = 0;
}

Task::Task(Task const & task)
  : handle_(task.handle_) {
  // empty
}

Task::Task(
  Action action) {
  mtapi_status_t status;
  mtapi_task_attributes_t attr;
  ExecutionPolicy policy = action.GetExecutionPolicy();
  mtapi_taskattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_PRIORITY,
    &policy.priority_, sizeof(policy.priority_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_AFFINITY,
    &policy.affinity_, sizeof(policy.affinity_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_domain_t domain_id = mtapi_domain_id_get(&status);
  assert(MTAPI_SUCCESS == status);
  mtapi_job_hndl_t job = mtapi_job_get(TASKS_CPP_JOB, domain_id, &status);
  assert(MTAPI_SUCCESS == status);
  Action* holder = embb::base::Allocation::New<Action>(action);
  handle_ = mtapi_task_start(MTAPI_TASK_ID_NONE, job,
    holder, sizeof(Action), MTAPI_NULL, 0, &attr, MTAPI_GROUP_NONE, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Task could not be started");
  }
}

Task::Task(
  Action action,
  mtapi_group_hndl_t group) {
  mtapi_status_t status;
  mtapi_task_attributes_t attr;
  ExecutionPolicy policy = action.GetExecutionPolicy();
  mtapi_taskattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_PRIORITY,
    &policy.priority_, sizeof(policy.priority_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_AFFINITY,
    &policy.affinity_, sizeof(policy.affinity_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_domain_t domain_id = mtapi_domain_id_get(&status);
  assert(MTAPI_SUCCESS == status);
  mtapi_job_hndl_t job = mtapi_job_get(TASKS_CPP_JOB, domain_id, &status);
  assert(MTAPI_SUCCESS == status);
  Action* holder = embb::base::Allocation::New<Action>(action);
  handle_ = mtapi_task_start(MTAPI_TASK_ID_NONE, job,
    holder, sizeof(Action), MTAPI_NULL, 0, &attr, group, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Task could not be started");
  }
}

Task::Task(
  mtapi_task_id_t id,
  Action action,
  mtapi_group_hndl_t group) {
  mtapi_status_t status;
  mtapi_task_attributes_t attr;
  ExecutionPolicy policy = action.GetExecutionPolicy();
  mtapi_taskattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_PRIORITY,
    &policy.priority_, sizeof(policy.priority_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_AFFINITY,
    &policy.affinity_, sizeof(policy.affinity_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_domain_t domain_id = mtapi_domain_id_get(&status);
  assert(MTAPI_SUCCESS == status);
  mtapi_job_hndl_t job = mtapi_job_get(TASKS_CPP_JOB, domain_id, &status);
  assert(MTAPI_SUCCESS == status);
  Action* holder = embb::base::Allocation::New<Action>(action);
  void * idptr = MTAPI_NULL;
  memcpy(&idptr, &id, sizeof(id));
  handle_ = mtapi_task_start(id, job,
    holder, sizeof(Action), idptr, 0, &attr, group, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Task could not be started");
  }
}

Task::Task(
  Action action,
  mtapi_queue_hndl_t queue) {
  mtapi_status_t status;
  mtapi_task_attributes_t attr;
  ExecutionPolicy policy = action.GetExecutionPolicy();
  mtapi_taskattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_PRIORITY,
    &policy.priority_, sizeof(policy.priority_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_AFFINITY,
    &policy.affinity_, sizeof(policy.affinity_), &status);
  assert(MTAPI_SUCCESS == status);
  Action* holder = embb::base::Allocation::New<Action>(action);
  handle_ = mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue,
    holder, sizeof(Action), MTAPI_NULL, 0, &attr, MTAPI_GROUP_NONE, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Task could not be started");
  }
}

Task::Task(
  Action action,
  mtapi_queue_hndl_t queue,
  mtapi_group_hndl_t group) {
  mtapi_status_t status;
  mtapi_task_attributes_t attr;
  ExecutionPolicy policy = action.GetExecutionPolicy();
  mtapi_taskattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_PRIORITY,
    &policy.priority_, sizeof(policy.priority_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_AFFINITY,
    &policy.affinity_, sizeof(policy.affinity_), &status);
  assert(MTAPI_SUCCESS == status);
  Action* holder = embb::base::Allocation::New<Action>(action);
  handle_ = mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue,
    holder, sizeof(Action), MTAPI_NULL, 0, &attr, group, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Task could not be started");
  }
}

Task::Task(
  mtapi_task_id_t id,
  Action action,
  mtapi_queue_hndl_t queue,
  mtapi_group_hndl_t group) {
  mtapi_status_t status;
  mtapi_task_attributes_t attr;
  ExecutionPolicy policy = action.GetExecutionPolicy();
  mtapi_taskattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_PRIORITY,
    &policy.priority_, sizeof(policy.priority_), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_taskattr_set(&attr, MTAPI_TASK_AFFINITY,
    &policy.affinity_, sizeof(policy.affinity_), &status);
  assert(MTAPI_SUCCESS == status);
  Action* holder = embb::base::Allocation::New<Action>(action);
  void * idptr = MTAPI_NULL;
  memcpy(&idptr, &id, sizeof(id));
  handle_ = mtapi_task_enqueue(id, queue,
    holder, sizeof(Action), idptr, 0, &attr, group, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Task could not be started");
  }
}

Task::~Task() {
}

mtapi_status_t Task::Wait(mtapi_timeout_t timeout) {
  mtapi_status_t status;
  mtapi_task_wait(handle_, timeout, &status);
  return status;
}

void Task::Cancel() {
  mtapi_status_t status;
  mtapi_task_cancel(handle_, &status);
  assert(MTAPI_SUCCESS == status);
}

} // namespace tasks
} // namespace embb
