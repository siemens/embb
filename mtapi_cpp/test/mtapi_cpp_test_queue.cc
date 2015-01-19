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

#include <cstdlib>

#include <mtapi_cpp_test_config.h>
#include <mtapi_cpp_test_queue.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23
#define QUEUE_TEST_ID 17

static void testQueueAction(embb::mtapi::TaskContext & /*context*/) {
  //std::cout << "testQueueAction on core " <<
  //  context.GetCurrentCoreNumber() << std::endl;
}

static void testDoSomethingElse() {
}

QueueTest::QueueTest() {
  CreateUnit("mtapi queue test").Add(&QueueTest::TestBasic, this);
}

void QueueTest::TestBasic() {
  //std::cout << "running testQueue..." << std::endl;

  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Queue & queue = node.CreateQueue(0, false);

  embb::mtapi::Task task = queue.Spawn(testQueueAction);

  testDoSomethingElse();

  task.Wait(MTAPI_INFINITE);

  node.DestroyQueue(queue);

  embb::mtapi::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
  //std::cout << "...done" << std::endl << std::endl;
}
