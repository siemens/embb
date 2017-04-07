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

#include <embb/mtapi/execution_policy.h>
#include <embb/mtapi/node.h>
#include <embb/base/exceptions.h>
#include <embb/base/c/internal/bitset.h>
#include <cassert>

namespace embb {
namespace mtapi {

ExecutionPolicy::ExecutionPolicy()
  : affinity_(0)
  , priority_(DefaultPriority) {
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
  Node::GetInstance(); // MTAPI has to be initialized
#endif
  mtapi_status_t status;
  mtapi_affinity_init(&affinity_, MTAPI_TRUE, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "Could not default construct Affinity.");
  }
}

ExecutionPolicy::ExecutionPolicy(bool initial_affinity, mtapi_uint_t priority)
  : affinity_(0)
  , priority_(priority) {
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
  Node::GetInstance(); // MTAPI has to be initialized
#endif
  mtapi_status_t status;
  mtapi_affinity_init(&affinity_, initial_affinity ? MTAPI_TRUE : MTAPI_FALSE,
    &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "Could not default construct Affinity.");
  }
}

ExecutionPolicy::ExecutionPolicy(mtapi_uint_t priority)
  : affinity_(0)
  , priority_(priority) {
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
  Node::GetInstance(); // MTAPI has to be initialized
#endif
  mtapi_status_t status;
  mtapi_affinity_init(&affinity_, MTAPI_TRUE, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "Could not default construct Affinity.");
  }
}

ExecutionPolicy::ExecutionPolicy(bool initial_affinity)
  : affinity_(0)
  , priority_(DefaultPriority) {
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
  Node::GetInstance(); // MTAPI has to be initialized
#endif
  mtapi_status_t status;
  mtapi_affinity_init(&affinity_, initial_affinity ? MTAPI_TRUE : MTAPI_FALSE,
    &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "Could not default construct Affinity.");
  }
}

void ExecutionPolicy::AddWorker(mtapi_uint_t worker) {
  mtapi_status_t status;
  mtapi_affinity_set(&affinity_, worker, MTAPI_TRUE, &status);
  assert(MTAPI_SUCCESS == status);
}

void ExecutionPolicy::RemoveWorker(mtapi_uint_t worker) {
  mtapi_status_t status;
  mtapi_affinity_set(&affinity_, worker, MTAPI_FALSE, &status);
  assert(MTAPI_SUCCESS == status);
}

bool ExecutionPolicy::IsSetWorker(mtapi_uint_t worker) {
  mtapi_status_t status;
  mtapi_boolean_t aff = mtapi_affinity_get(&affinity_, worker, &status);
  assert(MTAPI_SUCCESS == status);
  return MTAPI_TRUE == aff;
}

unsigned int ExecutionPolicy::GetCoreCount() const {
  return embb_bitset_count(&affinity_);
}

mtapi_affinity_t ExecutionPolicy::GetAffinity() const {
  return affinity_;
}

mtapi_uint_t ExecutionPolicy::GetPriority() const {
  return priority_;
}

const mtapi_uint_t ExecutionPolicy::DefaultPriority = 0;

}  // namespace mtapi
}  // namespace embb
