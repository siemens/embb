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

#include <embb/mtapi/task_context.h>

#include <embb/mtapi/internal/check_status.h>
#include <embb/mtapi/status_exception.h>
#include <embb/base/internal/config.h>

namespace embb {
namespace mtapi {
namespace internal {

void CheckStatus(mtapi_status_t status) {
  switch (status) {
  case MTAPI_SUCCESS:
  case MTAPI_TIMEOUT:
  case MTAPI_GROUP_COMPLETED:
    // these are no errors
    break;

  case MTAPI_ERR_NODE_INITIALIZED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: node already initialized.");
    break;
  case MTAPI_ERR_NODE_NOTINIT:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: node not initialized.");
    break;
  case MTAPI_ERR_PARAMETER:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: invalid parameter.");
    break;
  case MTAPI_ERR_CORE_NUM:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: invalid worker number.");
    break;
  case MTAPI_ERR_RUNTIME_LOADBALANCING_NOTSUPPORTED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: load balancing not supported.");
    break;
  case MTAPI_ERR_RUNTIME_REMOTETASKS_NOTSUPPORTED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: remote tasks not supported.");
    break;
  case MTAPI_ERR_ARG_NOT_IMPLEMENTED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: argument not implemented.");
    break;
  case MTAPI_ERR_FUNC_NOT_IMPLEMENTED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: function not implemented.");
    break;
  case MTAPI_ERR_WAIT_PENDING:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: wait pending.");
    break;
  case MTAPI_ERR_ARG_SIZE:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: argument size mismatch.");
    break;
  case MTAPI_ERR_RESULT_SIZE:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: result buffer size mismatch.");
    break;
  case MTAPI_ERR_BUFFER_SIZE:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: buffer size mismatch.");
    break;
  case MTAPI_ERR_GROUP_LIMIT:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: group limit exceeded.");
    break;
  case MTAPI_ERR_GROUP_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: group invalid.");
    break;
  case MTAPI_ERR_QUEUE_LIMIT:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: queue limit exceeded.");
    break;
  case MTAPI_ERR_QUEUE_DISABLED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: queue disabled.");
    break;
  case MTAPI_ERR_QUEUE_DELETED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: queue deleted.");
    break;
  case MTAPI_ERR_QUEUE_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: queue invalid.");
    break;
  case MTAPI_ERR_JOB_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: job invalid.");
    break;
  case MTAPI_ERR_TASK_LIMIT:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: task limit exceeded.");
    break;
  case MTAPI_ERR_TASK_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: task invalid.");
    break;
  case MTAPI_ERR_CONTEXT_OUTOFCONTEXT:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: task context used outside of worker thread.");
    break;
  case MTAPI_ERR_CONTEXT_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: task context invalid.");
    break;
  case MTAPI_ERR_ACTION_DISABLED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action disabled.");
    break;
  case MTAPI_ERR_ACTION_DELETED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action deleted.");
    break;
  case MTAPI_ERR_ACTION_CANCELLED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action canceled.");
    break;
  case MTAPI_ERR_ACTION_FAILED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action failed.");
    break;
  case MTAPI_ERR_ACTION_LIMIT:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action limit exceeded.");
    break;
  case MTAPI_ERR_ACTION_EXISTS:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action already exists.");
    break;
  case MTAPI_ERR_ACTION_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: action invalid.");
    break;
  case MTAPI_ERR_DOMAIN_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: domain invalid.");
    break;
  case MTAPI_ERR_NODE_INVALID:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: node invalid.");
    break;
  case MTAPI_ERR_NODE_INITFAILED:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: node initialization failed.");
    break;
  case MTAPI_ERR_ATTR_SIZE:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: attribute size mismatch.");
    break;
  case MTAPI_ERR_ATTR_NUM:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: invalid attribute.");
    break;
  case MTAPI_ERR_ATTR_READONLY:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: attribute is read only.");
    break;

  case MTAPI_ERR_ACTION_NUM_INVALID:
  case MTAPI_ERR_UNKNOWN:
  default:
    EMBB_THROW(embb::mtapi::StatusException,
      "MTAPI: unknown error.");
    break;
  }
}

} // namespace internal
} // namespace mtapi
} // namespace embb
