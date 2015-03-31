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

#include <cassert>

#include <embb/base/exceptions.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace tasks {

Queue::Queue(mtapi_uint_t priority, bool ordered) {
  mtapi_status_t status;
  mtapi_queue_attributes_t attr;
  mtapi_boolean_t bb;
  mtapi_queueattr_init(&attr, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_queueattr_set(&attr, MTAPI_QUEUE_PRIORITY,
    &priority, sizeof(priority), &status);
  assert(MTAPI_SUCCESS == status);
  bb = ordered ? MTAPI_TRUE : MTAPI_FALSE;
  mtapi_queueattr_set(&attr, MTAPI_QUEUE_ORDERED,
    &bb, sizeof(bb), &status);
  assert(MTAPI_SUCCESS == status);
  bb = MTAPI_TRUE;
  mtapi_queueattr_set(&attr, MTAPI_QUEUE_RETAIN,
    &bb, sizeof(bb), &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_domain_t domain_id = mtapi_domain_id_get(&status);
  assert(MTAPI_SUCCESS == status);
  mtapi_job_hndl_t job = mtapi_job_get(TASKS_CPP_JOB, domain_id, &status);
  assert(MTAPI_SUCCESS == status);
  handle_ = mtapi_queue_create(MTAPI_QUEUE_ID_NONE, job, &attr, &status);
  // Handle MTAPI error status in appropriate exceptions
  if (status == MTAPI_SUCCESS) {
    return;
  } else if (status == MTAPI_ERR_QUEUE_LIMIT) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Queue could not be constructed, "
      "maximum number of queues exceeded");
  } else if (status == MTAPI_ERR_JOB_INVALID) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Queue could not be constructed, "
      "invalid job");
  } else {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Queue could not be constructed");
  }
}

Queue::~Queue() {
  mtapi_status_t status;
  mtapi_queue_delete(handle_, MTAPI_INFINITE, &status);
  assert(MTAPI_SUCCESS == status);
}

void Queue::Enable() {
  mtapi_status_t status;
  mtapi_queue_enable(handle_, &status);
  assert(MTAPI_SUCCESS == status);
}

void Queue::Disable() {
  mtapi_status_t status;
  mtapi_queue_disable(handle_, MTAPI_INFINITE, &status);
  assert(MTAPI_SUCCESS == status);
}

Task Queue::Spawn(Action action) {
  return Task(action, handle_);
}

Task Queue::Spawn(Group const * group, Action action) {
  return Task(action, handle_, group->handle_);
}

} // namespace tasks
} // namespace embb
