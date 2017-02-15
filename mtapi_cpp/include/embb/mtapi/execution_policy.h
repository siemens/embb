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

#ifndef EMBB_MTAPI_EXECUTION_POLICY_H_
#define EMBB_MTAPI_EXECUTION_POLICY_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/internal/cmake_config.h>

namespace embb {
namespace mtapi {

/**
 * Describes the execution policy of a parallel algorithm.
 * The execution policy comprises
 *  - the affinity of tasks to MTAPI worker threads (not CPU cores) and
 *  - the priority of the spawned tasks.
 *
 * The priority is a number between 0 (denoting the highest priority) to
 * max_priorities - 1 as given during initialization using Node::Initialize().
 * The default value of max_priorities is 4.
 *
 * \ingroup CPP_MTAPI
 */
class ExecutionPolicy{
 public:
  /**
   * Constructs the default execution policy.
   * Sets the affinity to all worker threads and the priority to the default
   * value.
   * \notthreadsafe
   */
  ExecutionPolicy();

  /**
   * Constructs an execution policy with the specified affinity and priority.
   * \notthreadsafe
   */
  ExecutionPolicy(
      bool initial_affinity,           /**< [in] \c true sets the affinity to
                                            all worker threads, \c false to no
                                            worker threads. */
      mtapi_uint_t priority            /**< [in] Priority for the execution
                                            policy. */
  );

  /**
   * Constructs an execution policy with the specified priority.
   * Sets the affinity to all worker threads.
   * \notthreadsafe
   */
  explicit ExecutionPolicy(
      mtapi_uint_t priority            /**< [in] Priority for the execution
                                            policy. */
  );

  /**
   * Constructs an execution policy with the specified affinity.
   * Sets the priority to the default value.
   * \notthreadsafe
   */
  explicit ExecutionPolicy(
      bool initial_affinity            /**< [in] \c true sets the affinity to
                                            all worker threads, \c false to no
                                            worker threads. */
  );

  /**
   * Sets affinity to a specific worker thread.
   * \notthreadsafe
   */
  void AddWorker(
    mtapi_uint_t worker                /**< [in] Worker thread index */
    );

  /**
   * Removes affinity to a specific worker thread.
   * \notthreadsafe
   */
  void RemoveWorker(
    mtapi_uint_t worker                /**< [in] Worker thread index */
    );

  /**
   * Checks if affinity to a specific worker thread is set.
   *
   * \return \c true if affinity is set, otherwise \c false
   * \waitfree
   */
  bool IsSetWorker(
    mtapi_uint_t worker                /**< [in] Worker thread index */
    );

  /**
   * Returns the number of cores the policy is affine to.
   *
   * \return the number of cores
   * \waitfree
   */
  unsigned int GetCoreCount() const;

  /**
   * Returns the affinity
   *
   * \return the affinity
   * \waitfree
   */
  mtapi_affinity_t GetAffinity() const;

  /** Returns the priority
   *
   * \return the priority
   * \waitfree
   */
  mtapi_uint_t GetPriority() const;

  friend class Task;

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
  mtapi_affinity_t affinity_;

  /**
   * Task Priority.
   */
  mtapi_uint_t priority_;
};

}  // namespace mtapi
}  // namespace embb

#endif  // EMBB_MTAPI_EXECUTION_POLICY_H_
