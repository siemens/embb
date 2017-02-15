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

#ifndef EMBB_MTAPI_STATUS_EXCEPTION_H_
#define EMBB_MTAPI_STATUS_EXCEPTION_H_

#include <embb/base/exceptions.h>

namespace embb {
namespace mtapi {

/**
 * Represents an MTAPI error state and is thrown by almost all mtapi_cpp
 * methods.
 *
 * \ingroup CPP_MTAPI
 */
class StatusException : public embb::base::Exception {
 public:
  /**
   * Constructs a StatusException.
   * \notthreadsafe
   */
  explicit StatusException(
    const char* message                /**< The message to use. */
    )
    : embb::base::Exception(message) {
    // empty
  }

  /**
   * Code associated with this exception
   *
   * \returns An integer representing the code of the exception
   *
   * \waitfree
   */
  virtual int Code() const { return EMBB_ERROR; }
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_STATUS_EXCEPTION_H_
