/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#include <embb/algorithms/execution_policy.h>

namespace embb {
namespace algorithms {

ExecutionPolicy::ExecutionPolicy() :
    affinity_(), priority_(DefaultPriority) {
}

ExecutionPolicy::ExecutionPolicy(bool initial_affinity, mtapi_uint_t priority)
:affinity_(initial_affinity), priority_(priority) {
}

ExecutionPolicy::ExecutionPolicy(mtapi_uint_t priority)
:affinity_(), priority_(priority) {
}

ExecutionPolicy::ExecutionPolicy(bool initial_affinity)
:affinity_(initial_affinity), priority_(DefaultPriority) {
}

void ExecutionPolicy::AddWorker(mtapi_uint_t worker) {
  affinity_.Add(worker);
}

void ExecutionPolicy::RemoveWorker(mtapi_uint_t worker) {
  affinity_.Remove(worker);
}

bool ExecutionPolicy::IsSetWorker(mtapi_uint_t worker) {
  return affinity_.IsSet(worker);
}

const mtapi::Affinity &ExecutionPolicy::GetAffinity() const {
  return affinity_;
}

mtapi_uint_t ExecutionPolicy::GetPriority() const {
  return priority_;
}

const mtapi_uint_t ExecutionPolicy::DefaultPriority = 0;

}  // namespace algorithms
}  // namespace embb
