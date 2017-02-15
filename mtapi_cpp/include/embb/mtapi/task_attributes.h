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

#ifndef EMBB_MTAPI_TASK_ATTRIBUTES_H_
#define EMBB_MTAPI_TASK_ATTRIBUTES_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/internal/check_status.h>
#include <embb/mtapi/execution_policy.h>

namespace embb {
namespace mtapi {

/**
 * Contains attributes of a Task.
 *
 * \ingroup CPP_MTAPI
 */
class TaskAttributes {
 public:
  /**
   * Constructs a TaskAttributes object.
   * \notthreadsafe
   */
  TaskAttributes() {
    mtapi_status_t status;
    mtapi_taskattr_init(&attributes_, &status);
    internal::CheckStatus(status);
  }

  /**
   * Sets the detached property of a Task.
   * If set to \c true, the started Task will have no handle and cannot be
   * waited for.
   *
   * \returns Reference to this object.
   * \notthreadsafe
   */
  TaskAttributes & SetDetached(
    bool state                         /**< The state to set. */
    ) {
    mtapi_status_t status;
    mtapi_boolean_t st = state ? MTAPI_TRUE : MTAPI_FALSE;
    mtapi_taskattr_set(&attributes_, MTAPI_TASK_DETACHED,
      &st, sizeof(st), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Sets the priority of a Task.
   * The priority influences the order in which tasks are chosen for execution.
   *
   * \returns Reference to this object.
   * \notthreadsafe
   */
  TaskAttributes & SetPriority(
    mtapi_uint_t priority              /**< The priority to set. */
    ) {
    mtapi_status_t status;
    mtapi_taskattr_set(&attributes_, MTAPI_TASK_PRIORITY,
      &priority, sizeof(priority), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Sets the affinity of a Task.
   * The affinity influences on which worker the Task will be executed.
   *
   * \returns Reference to this object.
   * \notthreadsafe
   */
  TaskAttributes & SetAffinity(
    mtapi_affinity_t affinity          /**< The affinity to set. */
    ) {
    mtapi_status_t status;
    mtapi_taskattr_set(&attributes_, MTAPI_TASK_AFFINITY,
      &affinity, sizeof(affinity), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Sets the ExecutionPolicy of a Task.
   * The ExecutionPolicy determines the affinity and priority of a Task.
   *
   * \returns Reference to this object.
   * \notthreadsafe
   */
  TaskAttributes & SetPolicy(
    ExecutionPolicy const & policy     /**< The ExecutionPolicy to set. */
    ) {
    SetPriority(policy.GetPriority());
    SetAffinity(policy.GetAffinity());
    return *this;
  }

  /**
   * Sets the number of instances in a Task.
   * The Task will be launched \c instances times. In the action function,
   * the number of instances and the current instance can be queried from
   * the TaskContext.
   *
   * \returns Reference to this object.
   * \notthreadsafe
   */
  TaskAttributes & SetInstances(
    mtapi_uint_t instances             /**< Number of instances to set. */
    ) {
    mtapi_status_t status;
    mtapi_taskattr_set(&attributes_, MTAPI_TASK_INSTANCES,
      &instances, sizeof(instances), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns A reference to the internal mtapi_task_attributes_t structure.
   * \waitfree
   */
  mtapi_task_attributes_t const & GetInternal() const {
    return attributes_;
  }

 private:
  mtapi_task_attributes_t attributes_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_TASK_ATTRIBUTES_H_
