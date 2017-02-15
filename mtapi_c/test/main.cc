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

#include <partest/partest.h>
#include <embb/base/c/thread.h>

#include <stdio.h>

#include <embb_mtapi_log.h>

#include <embb_mtapi_test_plugin.h>
#include <embb_mtapi_test_init_finalize.h>
#include <embb_mtapi_test_task.h>
#include <embb_mtapi_test_group.h>
#include <embb_mtapi_test_queue.h>
#include <embb_mtapi_test_error.h>
#include <embb_mtapi_test_id_pool.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/atomic.h>

PT_MAIN("MTAPI C") {
  embb_log_set_log_level(EMBB_LOG_LEVEL_NONE);
  embb_thread_set_max_count(1024);

  PT_RUN(TaskTest);
  PT_RUN(PluginTest);
  PT_RUN(ErrorTest);
  PT_RUN(InitFinalizeTest);
  PT_RUN(GroupTest);
  PT_RUN(QueueTest);
  PT_RUN(IdPoolTest);

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}
