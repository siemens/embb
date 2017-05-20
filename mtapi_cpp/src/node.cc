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

#include <embb/mtapi/node.h>
#include <embb/base/c/mutex.h>

namespace embb {
namespace mtapi {

embb::mtapi::Node * embb::mtapi::Node::node_instance_ = NULL;
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
static EMBB_BASE_BASIC_TYPE_ATOMIC_4 init_mutex(0);
#endif

void Node::Initialize(
  mtapi_domain_t domain_id,
  mtapi_node_t node_id
  ) {
  if (IsInitialized()) {
    EMBB_THROW(StatusException,
      "MTAPI: node was already initialized.");
  } else {
    NodeAttributes attributes; // default attributes
    node_instance_ = embb::base::Allocation::New<Node>(
      domain_id, node_id, attributes);
  }
}

void Node::Initialize(
  mtapi_domain_t domain_id,
  mtapi_node_t node_id,
  NodeAttributes const & attributes
  ) {
  if (IsInitialized()) {
    EMBB_THROW(StatusException,
      "MTAPI: node was already initialized.");
  } else {
    node_instance_ = embb::base::Allocation::New<Node>(
      domain_id, node_id, attributes);
  }
}

Node & Node::GetInstance() {
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
  if (!IsInitialized()) {
    EMBB_BASE_BASIC_TYPE_SIZE_4 compare = 0;
    while (0 == embb_internal__atomic_compare_and_swap_4(
      &init_mutex, &compare, 1)) {
      compare = 0;
      embb_thread_yield();
    }
    if (!IsInitialized()) {
      Node::Initialize(
        MTAPI_CPP_AUTOMATIC_DOMAIN_ID, MTAPI_CPP_AUTOMATIC_NODE_ID);
      atexit(Node::Finalize);
    }
    embb_internal__atomic_store_4(&init_mutex, 0);
  }
  return *node_instance_;
#else
  if (IsInitialized()) {
    return *node_instance_;
  } else {
    EMBB_THROW(StatusException,
      "MTAPI: node is not initialized.");
  }
#endif
}

void Node::Finalize() {
  if (IsInitialized()) {
    embb::base::Allocation::Delete(node_instance_);
    node_instance_ = NULL;
  } else {
    EMBB_THROW(StatusException,
      "MTAPI: node is not initialized.");
  }
}

} // namespace mtapi
} // namespace embb
