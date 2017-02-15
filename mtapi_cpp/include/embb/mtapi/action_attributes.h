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

#ifndef EMBB_MTAPI_ACTION_ATTRIBUTES_H_
#define EMBB_MTAPI_ACTION_ATTRIBUTES_H_

#include <embb/mtapi/c/mtapi.h>
#include <embb/mtapi/internal/check_status.h>
#include <embb/mtapi/affinity.h>

namespace embb {
namespace mtapi {

/**
 * Contains attributes of an Action.
 *
 * \ingroup CPP_MTAPI
 */
class ActionAttributes {
 public:
  /**
   * Constructs an ActionAttributes object.
   * \waitfree
   */
  ActionAttributes() {
    mtapi_status_t status;
    mtapi_actionattr_init(&attributes_, &status);
    internal::CheckStatus(status);
  }

  /**
   * Sets the global property of an Action.
   * This determines whether the object will be visible across nodes.
   *
   * \returns Reference to this object.
   * \waitfree
   */
  ActionAttributes & SetGlobal(
    bool state                         /**< The state to set */
    ) {
    mtapi_status_t status;
    mtapi_boolean_t st = state ? MTAPI_TRUE : MTAPI_FALSE;
    mtapi_actionattr_set(&attributes_, MTAPI_ACTION_GLOBAL,
      &st, sizeof(st), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Sets the affinity of an Action.
   *
   * \returns Reference to this object.
   * \waitfree
   */
  ActionAttributes & SetAffinity(
    Affinity const & affinity          /**< The Affinity to set. */
    ) {
    mtapi_status_t status;
    mtapi_affinity_t af = affinity.GetInternal();
    mtapi_actionattr_set(&attributes_, MTAPI_ACTION_AFFINITY,
      &af, sizeof(af), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Sets the domain shared property of an Action.
   * This determines whether the object will be visible across domains.
   *
   * \returns Reference to this object.
   * \waitfree
   */
  ActionAttributes & SetDomainShared(
    bool state                         /**< The state to set */
    ) {
    mtapi_status_t status;
    mtapi_boolean_t st = state ? MTAPI_TRUE : MTAPI_FALSE;
    mtapi_actionattr_set(&attributes_, MTAPI_ACTION_DOMAIN_SHARED,
      &st, sizeof(st), &status);
    internal::CheckStatus(status);
    return *this;
  }

  /**
   * Returns the internal representation of this object.
   * Allows for interoperability with the C interface.
   *
   * \returns A reference to the internal mtapi_action_attributes_t structure.
   * \waitfree
   */
  mtapi_action_attributes_t const & GetInternal() const {
    return attributes_;
  }

 private:
  mtapi_action_attributes_t attributes_;
};

} // namespace mtapi
} // namespace embb

#endif // EMBB_MTAPI_ACTION_ATTRIBUTES_H_
