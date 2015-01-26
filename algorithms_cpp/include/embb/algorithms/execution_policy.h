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

#ifndef EMBB_ALGORITHMS_EXECUTION_POLICY_H_
#define EMBB_ALGORITHMS_EXECUTION_POLICY_H_

#include <embb/mtapi/mtapi.h>
#include <embb/mtapi/affinity.h>

namespace embb {
namespace algorithms {
/**
 * Describes the execution policy of a parallel algorithm.
 * The execution policy comprises
 *  - the affinity of tasks to MTAPI worker threads (not CPU cores) and
 *  - the priority of the spawned tasks.
 *
 * \ingroup CPP_ALGORITHMS
 */
class ExecutionPolicy{
 public:
  /**
   * Constructs the default execution policy.
   * Sets the affinity to all worker threads and the priority to the default
   * value.
   */
  ExecutionPolicy();

  /**
   * Constructs an execution policy with the specified affinity and priority.
   */
  ExecutionPolicy(
      bool initial_affinity, /**<
        [IN] \c true sets the affinity to all worker threads, \c false to no
        worker threads. */
      mtapi_uint_t priority /**<
        [IN] Priority for the execution policy. */
  );

  /**
   * Constructs an execution policy with the specified priority.
   * Sets the affinity to all worker threads.
   */
  explicit ExecutionPolicy(
      mtapi_uint_t priority /**<
        [IN] Priority for the execution policy. */
  );

  /**
   * Constructs an execution policy with the specified affinity.
   * Sets the priority to the default value.
   */
  explicit ExecutionPolicy(
      bool initial_affinity /**<
        [IN] \c true sets the affinity to all worker threads, \c false to no
        worker threads. */
  );

  /**
   *  Sets affinity to a specific worker thread.
   */
  void AddWorker(
    mtapi_uint_t worker
    /**< [IN] Worker thread index */
    );

  /**
   * Removes affinity to a specific worker thread.
   */
  void RemoveWorker(
    mtapi_uint_t worker
    /**< [IN] Worker thread index */
    );

  /**
   * Checks if affinity to a specific worker thread is set.
   *
   * \return \c true if affinity is set, otherwise \c false
   */
  bool IsSetWorker(
    mtapi_uint_t worker
    /**< [IN] Worker thread index */
    );

  /**
   * Returns the affinity
   *
   * \return the affinity
   */
  const mtapi::Affinity &GetAffinity() const;

  /** Returns the priority
   *
   * \return the priority
   */
  mtapi_uint_t GetPriority() const;

 private:
  /**
   * Default priority.
   * Currently set to 0 = MAX priority.
   */
  static const mtapi_uint_t DefaultPriority;

  /**
   * Task Affinity.
   * Maps the affinity of tasks to MTAPI worker threads (not CPU cores).
   */
  mtapi::Affinity affinity_;

  /**
   * Task Priority.
   */
  mtapi_uint_t priority_;
};
}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_EXECUTION_POLICY_H_
