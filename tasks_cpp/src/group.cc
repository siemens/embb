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

#include <embb/base/exceptions.h>
#include <embb/tasks/tasks.h>

namespace embb {
namespace tasks {

Group::Group() {
  Create();
}

Group::~Group() {
  mtapi_status_t status;
  mtapi_group_delete(handle_, &status);
  assert(MTAPI_SUCCESS == status);
}

void Group::Create() {
  mtapi_status_t status;
  handle_ = mtapi_group_create(MTAPI_GROUP_ID_NONE, MTAPI_NULL, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Group could not be constructed");
  }
}

Task Group::Spawn(Action action) {
  return Task(action, handle_);
}

Task Group::Spawn(mtapi_task_id_t id, Action action) {
  return Task(id, action, handle_);
}

mtapi_status_t Group::WaitAny(mtapi_timeout_t timeout) {
  mtapi_status_t status;
  mtapi_group_wait_any(handle_, MTAPI_NULL, timeout, &status);
  if (MTAPI_GROUP_COMPLETED == status) {
    // group has been deleted, so recreate it for simplicity
    Create();
  }
  return status;
}

mtapi_status_t Group::WaitAny(
  mtapi_timeout_t timeout,
  mtapi_task_id_t & result) {
  mtapi_status_t status;
  void * res;
  mtapi_group_wait_any(handle_, &res, timeout, &status);
  memcpy(&result, &res, sizeof(result));
  if (MTAPI_GROUP_COMPLETED == status) {
    // group has been deleted, so recreate it for simplicity
    Create();
  }
  return status;
}

mtapi_status_t Group::WaitAll(mtapi_timeout_t timeout) {
  mtapi_status_t status;
  mtapi_group_wait_all(handle_, timeout, &status);
  // group has been deleted, so recreate it for simplicity
  Create();
  return status;
}

} // namespace tasks
} // namespace embb
