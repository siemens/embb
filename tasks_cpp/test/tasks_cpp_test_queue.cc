/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#include <cstdlib>

#include <tasks_cpp_test_config.h>
#include <tasks_cpp_test_queue.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23
#define QUEUE_TEST_ID 17

static void testQueueAction(embb::tasks::TaskContext & /*context*/) {
  // empty
}

static void testDoSomethingElse() {
}

QueueTest::QueueTest() {
  CreateUnit("tasks_cpp queue test").Add(&QueueTest::TestBasic, this);
}

void QueueTest::TestBasic() {
  embb::tasks::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);

  embb::tasks::Node & node = embb::tasks::Node::GetInstance();
  embb::tasks::Queue & queue = node.CreateQueue(0, false);

  embb::tasks::Task task = queue.Spawn(testQueueAction);

  testDoSomethingElse();

  task.Wait(MTAPI_INFINITE);

  node.DestroyQueue(queue);

  embb::tasks::Node::Finalize();

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}
