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

#include <embb/base/c/memory_allocation.h>

#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_init_finalize.h>

InitFinalizeTest::InitFinalizeTest() {
  CreateUnit("mtapi init/finalize test").
    Add(&InitFinalizeTest::TestBasic, this);
}

void InitFinalizeTest::TestBasic() {
  embb_mtapi_log_info("running testInitFinalize...\n");

  mtapi_node_attributes_t node_attr;
  mtapi_info_t info;
  mtapi_status_t status;

#ifdef EMBB_THREADING_ANALYSIS_MODE
  const int iterations(10);
#else
  const int iterations(100);
#endif
  for (int ii = 0; ii < iterations; ii++) {
    status = MTAPI_ERR_UNKNOWN;
    mtapi_nodeattr_init(&node_attr, &status);
    MTAPI_CHECK_STATUS(status);

    status = MTAPI_ERR_UNKNOWN;
    mtapi_nodeattr_set(&node_attr,
      MTAPI_NODE_TYPE,
      MTAPI_ATTRIBUTE_VALUE(MTAPI_NODE_TYPE_SMP),
      MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
      &status);
    MTAPI_CHECK_STATUS(status);

    status = MTAPI_ERR_UNKNOWN;
    mtapi_initialize(
      THIS_DOMAIN_ID,
      THIS_NODE_ID,
      &node_attr,
      &info,
      &status);
    MTAPI_CHECK_STATUS(status);

    status = MTAPI_ERR_UNKNOWN;
    mtapi_finalize(&status);
    MTAPI_CHECK_STATUS(status);
  }

  PT_EXPECT(embb_get_bytes_allocated() == 0);

  embb_mtapi_log_info("...done\n\n");
}
