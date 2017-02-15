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

#include <mtapi_cpp_test_config.h>
#include <mtapi_cpp_test_queue.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_QUEUE 42

static void testQueueAction(
  const void* /*args*/,
  mtapi_size_t /*args_size*/,
  void* results,
  mtapi_size_t /*results_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * context) {
  embb::mtapi::TaskContext ctx(context);
  int * out = reinterpret_cast<int*>(results);
  *out = 1;
  ctx.SetStatus(MTAPI_ERR_ACTION_CANCELLED);
}

static void testDoSomethingElse() {
}

QueueTest::QueueTest() {
  CreateUnit("mtapi_cpp queue test").Add(&QueueTest::TestBasic, this);
}

void QueueTest::TestBasic() {
  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

  embb::mtapi::Job job = node.GetJob(JOB_TEST_QUEUE);
  embb::mtapi::Action action = node.CreateAction(
    JOB_TEST_QUEUE, testQueueAction, MTAPI_NULL, 0);

  embb::mtapi::Queue queue;

  {
    queue = node.CreateQueue(job);

    int result = 0;
    embb::mtapi::Task task = queue.Enqueue<void, int>(MTAPI_NULL, &result);

    testDoSomethingElse();

    mtapi_status_t status = task.Wait();
    PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_CANCELLED);
    PT_EXPECT_EQ(result, 1);

    queue.Delete();
  }

  action.Delete();
  embb::mtapi::Node::Finalize();

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}
