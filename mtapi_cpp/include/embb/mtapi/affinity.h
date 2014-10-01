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

#ifndef EMBB_MTAPI_AFFINITY_H_
#define EMBB_MTAPI_AFFINITY_H_

#include <embb/mtapi/c/mtapi.h>

namespace embb {
namespace mtapi {

/**
  * Describes the Affinity of a Task to worker threads.
  *
  * \ingroup CPP_MTAPI
  */
class Affinity {
 public:
  /**
    * Constructs an Affinity including all worker threads.
    * \memory Calls embb::mtapi::Node::Initialize() which potentially allocates
    * \throws ErrorException if the Affinity object could not be constructed.
    */
  Affinity();

  /**
    * Constructs an Affinity including all or no worker threads.
    * \memory Calls embb::mtapi::Node::Initialize() which potentially allocates
    * \throws ErrorException if the Affinity object could not be constructed.
    */
  Affinity(
    bool initial_affinity              /**< [in] Initial affinity
                                            (true = all worker threads,
                                             false = no worker threads) */
    );

  /**
    * Sets Affinity to a specific worker thread.
    * \threadsafe
    */
  void Add(
    mtapi_uint_t worker                /**< [in] Worker thread index */
    );

  /**
    * Removes Affinity to a specific worker thread.
    * \threadsafe
    */
  void Remove(
    mtapi_uint_t worker                /**< [in] Worker thread index */
    );

  /**
    * Checks if Affinity to a specific worker thread is set.
    * \return \c true if \c *this is affine to the given worker, otherwise
    * \c false.
    * \threadsafe
    */
  bool IsSet(
    mtapi_uint_t worker                /**< [in] Worker thread index */
    );

  friend class Task;

 private:
  mtapi_affinity_t affinity_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_AFFINITY_H_
