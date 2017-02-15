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

#ifndef EMBB_MTAPI_JOB_H_
#define EMBB_MTAPI_JOB_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/internal/check_status.h>

namespace embb {
namespace mtapi {

/**
 * Represents a collection of Actions.
 *
 * \ingroup CPP_MTAPI
 */
class Job {
 public:
  /**
   * Constructs a Job.
   * The Job object will be invalid.
   * \waitfree
   */
  Job() {
    handle_.id = 0;
    handle_.tag = 0;
  }

  /**
   * Copies a Job object.
   * \waitfree
   */
  Job(
    Job const & other                  /**< The Job to copy from */
    ) : handle_(other.handle_) {
    // empty
  }

  /**
   * Copies a Job object.
   * \waitfree
   */
  void operator=(
    Job const & other                  /**< The Job to copy from */
    ) {
    handle_ = other.handle_;
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns The internal mtapi_job_hndl_t.
   * \waitfree
   */
  mtapi_job_hndl_t GetInternal() const {
    return handle_;
  }

  friend class Node;

 private:
   /**
    * Constructs a Job with the given \c job_id and \c domain_id.
    * Requires an initialized Node.
    */
  Job(
    mtapi_job_id_t job_id,             /**< Job ID to use. */
    mtapi_domain_t domain_id           /**< Domain ID to use. */
    ) {
    mtapi_status_t status;
    handle_ = mtapi_job_get(job_id, domain_id, &status);
    internal::CheckStatus(status);
  }

  mtapi_job_hndl_t handle_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_JOB_H_
