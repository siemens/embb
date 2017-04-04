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

#include <iostream>
#include <string>
#include <cassert>

#include <mtapi_cpp_test_config.h>
#include <mtapi_cpp_test_task.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_TASK 42
#define JOB_TEST_ERROR 17

static void testTaskAction(
  const void* args,
  mtapi_size_t /*args_size*/,
  void* results,
  mtapi_size_t /*results_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * context) {
  embb::mtapi::TaskContext ctx(context);
  const char * msg = static_cast<const char *>(args);
  std::string* out = static_cast<std::string*>(results);
  *out = msg;
}

static void testErrorAction(
  const void* /*args*/,
  mtapi_size_t /*args_size*/,
  void* /*results*/,
  mtapi_size_t /*results_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * context) {
  embb::mtapi::TaskContext ctx(context);
  ctx.SetStatus(MTAPI_ERR_ACTION_FAILED);
}

static void testDoSomethingElse() {
}

TaskTest::TaskTest() {
  CreateUnit("mtapi_cpp task test").Add(&TaskTest::TestBasic, this);
}

void TaskTest::TestBasic() {
  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

  {
    embb::mtapi::NodeAttributes attr;
    attr
      .SetMaxActions(1024)
      .SetMaxActionsPerJob(2)
      .SetMaxPriorities(4);
  }

  {
    embb::mtapi::Affinity affinity(false);
    PT_EXPECT_EQ(affinity.GetInternal(), 0u);
    affinity.Set(0u, true);
    PT_EXPECT_EQ(affinity.GetInternal(), 1u);
    if (node.GetCoreCount() > 1) {
      affinity.Set(1u, true);
      PT_EXPECT_EQ(affinity.GetInternal(), 3u);
    }
    affinity.Set(0u, false);
    if (node.GetCoreCount() > 1) {
      PT_EXPECT_EQ(affinity.GetInternal(), 2u);
    } else {
      PT_EXPECT_EQ(affinity.GetInternal(), 0u);
    }
    PT_EXPECT_EQ(affinity.Get(0), false);
    if (node.GetCoreCount() > 1) {
      PT_EXPECT_EQ(affinity.Get(1), true);
    }
  }

  {
    embb::mtapi::Job job_task = node.GetJob(JOB_TEST_TASK);

    embb::mtapi::Action action_task =
      node.CreateAction(JOB_TEST_TASK, testTaskAction);

    std::string test;
    embb::mtapi::Task task = node.Start(job_task, "simple", &test);
    testDoSomethingElse();
    mtapi_status_t status = task.Wait();
    PT_EXPECT_EQ(status, MTAPI_SUCCESS);
    PT_EXPECT(test == "simple");

    action_task.Delete();
  }

  {
    embb::mtapi::Job job_error = node.GetJob(JOB_TEST_ERROR);

    embb::mtapi::Action action_error =
      node.CreateAction(JOB_TEST_ERROR, testErrorAction);

    std::string test;
    embb::mtapi::Task task = node.Start(job_error, "simple", &test);
    testDoSomethingElse();
    mtapi_status_t status = task.Wait();
    PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_FAILED);

    action_error.Delete();
  }

  embb::mtapi::Node::Finalize();

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}
