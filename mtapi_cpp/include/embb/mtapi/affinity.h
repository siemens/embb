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

#ifndef EMBB_MTAPI_AFFINITY_H_
#define EMBB_MTAPI_AFFINITY_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/internal/check_status.h>

namespace embb {
namespace mtapi {

/**
 * Describes the affinity of an Action or Task to a worker thread of a Node.
 *
 * \ingroup CPP_MTAPI
 */
class Affinity {
 public:
  /**
   * Constructs an Affinity object.
   * \notthreadsafe
   */
  Affinity() {
    Init(true);
  }

  /**
   * Copies an Affinity object.
   * \waitfree
   */
  Affinity(
    Affinity const & other             /**< The Affinity to copy from */
    )
    : affinity_(other.affinity_) {
    // empty
  }

  /**
   * Copies an Affinity object.
   * \waitfree
   */
  void operator=(
    Affinity const & other             /**< The Affinity to copy from */
    ) {
    affinity_ = other.affinity_;
  }

  /**
   * Constructs an Affinity object with the given initial affinity.
   * If \c initial_affinity is \c true the Affinity will map to all worker
   * threads, otherwise it will map to no worker threads.
   * \notthreadsafe
   */
  Affinity(
    bool initial_affinity              /**< The initial affinity to set. */
    ) {
    Init(initial_affinity);
  }

  /**
   * Initializes an Affinity object with the given initial affinity.
   * If \c initial_affinity is \c true the Affinity will map to all worker
   * threads, otherwise it will map to no worker threads.
   * \notthreadsafe
   */
  void Init(
    bool initial_affinity              /**< The initial affinity to set. */
    ) {
    mtapi_status_t status;
    mtapi_boolean_t ia = initial_affinity ? MTAPI_TRUE : MTAPI_FALSE;
    mtapi_affinity_init(&affinity_, ia, &status);
    internal::CheckStatus(status);
  }

  /**
   * Sets affinity to the given worker.
   * \notthreadsafe
   */
  void Set(
    mtapi_uint_t worker,               /**< The worker to set affinity to. */
    bool state                         /**< The state of the affinity. */
    ) {
    mtapi_status_t status;
    mtapi_boolean_t st = state ? MTAPI_TRUE : MTAPI_FALSE;
    mtapi_affinity_set(&affinity_, worker, st, &status);
    internal::CheckStatus(status);
  }

  /**
   * Gets affinity to the given worker.
   *
   * \returns \c true, if the Affinity maps to the worker, \c false otherwise.
   * \waitfree
   */
  bool Get(
    mtapi_uint_t worker                /**< The worker to get affinity of. */
    ) {
    mtapi_status_t status;
    mtapi_boolean_t state =
      mtapi_affinity_get(&affinity_, worker, &status);
    internal::CheckStatus(status);
    return (state != MTAPI_FALSE) ? true : false;
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns The internal mtapi_affinity_t.
   * \waitfree
   */
  mtapi_affinity_t GetInternal() const {
    return affinity_;
  }

 private:
  mtapi_affinity_t affinity_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_AFFINITY_H_
