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

#ifndef EMBB_MTAPI_ACTION_H_
#define EMBB_MTAPI_ACTION_H_

#include <embb/mtapi/internal/check_status.h>
#include <embb/mtapi/action_attributes.h>
#include <embb/mtapi/job.h>

namespace embb {
namespace mtapi {

/**
 * Holds the actual worker function used to execute a Task.
 *
 * \ingroup CPP_MTAPI
 */
class Action {
 public:
  /**
   * Constructs an Action.
   */
  Action(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func,      /**< The action function */
    const void * node_local_data,      /**< Node local data available to all
                                            Tasks using this Action */
    mtapi_size_t node_local_data_size, /**< Size of node local data */
    ActionAttributes const & attributes
                                       /**< Attributes of the Action */
    ) {
    Create(job_id, func, node_local_data, node_local_data_size,
      &attributes.GetInternal());
  }

  /**
   * Constructs an Action.
   */
  Action(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func,      /**< The action function */
    const void * node_local_data,      /**< Node local data available to all
                                            Tasks using this Action */
    mtapi_size_t node_local_data_size  /**< Size of node local data */
    ) {
    Create(job_id, func, node_local_data, node_local_data_size,
      MTAPI_DEFAULT_ACTION_ATTRIBUTES);
  }

  /**
   * Constructs an Action.
   */
  Action(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func,      /**< The action function */
    ActionAttributes const & attributes
                                       /**< Attributes of the Action */
    ) {
    Create(job_id, func, MTAPI_NULL, 0, &attributes.GetInternal());
  }

  /**
   * Constructs an Action.
   */
  Action(
    mtapi_job_id_t job_id,             /**< Job ID the Action belongs to */
    mtapi_action_function_t func       /**< The action function */
    ) {
    Create(job_id, func, MTAPI_NULL, 0, MTAPI_DEFAULT_ACTION_ATTRIBUTES);
  }

  /**
   * Destroys an Action.
   */
  ~Action() {
    mtapi_action_delete(handle_, MTAPI_INFINITE, MTAPI_NULL);
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns The internal mtapi_action_hndl_t.
   * \waitfree
   */
  mtapi_action_hndl_t GetInternal() const {
    return handle_;
  }

 private:
  // no default constructor
  Action();

  // not copyable
  Action(Action const & other);
  void operator=(Action const & other);

  void Create(
    mtapi_job_id_t job_id,
    mtapi_action_function_t func,
    const void * node_local_data,
    mtapi_size_t node_local_data_size,
    mtapi_action_attributes_t const * attributes
    ) {
    mtapi_status_t status;
    handle_ = mtapi_action_create(job_id, func,
      node_local_data, node_local_data_size,
      attributes, &status);
    internal::CheckStatus(status);
  }

  mtapi_action_hndl_t handle_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_ACTION_H_
