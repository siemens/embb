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

#include <iostream>
#include <string>
#include <cassert>

#include <tasks_cpp_test_config.h>
#include <tasks_cpp_test_task.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23

static void testTaskAction(
  char const * msg,
  std::string * output,
  embb::tasks::TaskContext & /*context*/) {
  *output = msg;
}

static void testRecursiveTaskAction(
  int * value,
  embb::tasks::TaskContext & /*context*/) {
  embb::tasks::Node & node = embb::tasks::Node::GetInstance();
  *value = *value + 1;
  if (*value < 1000) {
    embb::tasks::Task task = node.Spawn(
      embb::base::Bind(
        testRecursiveTaskAction, value, embb::base::Placeholder::_1));
    task.Wait(MTAPI_INFINITE);
  }
  PT_EXPECT(*value == 1000);
}

static void testErrorTaskAction(embb::tasks::TaskContext & context) {
  context.SetStatus(MTAPI_ERR_ACTION_FAILED);
}

static void testDoSomethingElse() {
}

TaskTest::TaskTest() {
  CreateUnit("tasks_cpp task test").Add(&TaskTest::TestBasic, this);
}

void TaskTest::TestBasic() {
  embb::tasks::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);

  embb::tasks::Node & node = embb::tasks::Node::GetInstance();

  embb::tasks::ExecutionPolicy policy(false);
  PT_EXPECT_EQ(policy.GetAffinity(), 0u);
  PT_EXPECT_EQ(policy.GetPriority(), 0u);
  policy.AddWorker(0u);
  PT_EXPECT_EQ(policy.GetAffinity(), 1u);
  policy.AddWorker(1u);
  PT_EXPECT_EQ(policy.GetAffinity(), 3u);
  policy.RemoveWorker(0u);
  PT_EXPECT_EQ(policy.GetAffinity(), 2u);
  PT_EXPECT_EQ(policy.IsSetWorker(0), false);
  PT_EXPECT_EQ(policy.IsSetWorker(1), true);

  std::string test;
  embb::tasks::Task task = node.Spawn(
    embb::base::Bind(
      testTaskAction, "simple", &test, embb::base::Placeholder::_1));
  testDoSomethingElse();
  task.Wait(MTAPI_INFINITE);
  PT_EXPECT(test == "simple");

  std::string test1, test2, test3;
  task = node.First(
    embb::base::Bind(
      testTaskAction, "first", &test1, embb::base::Placeholder::_1)).
    Then(embb::base::Bind(
      testTaskAction, "second", &test2, embb::base::Placeholder::_1)).
    Then(embb::base::Bind(
      testTaskAction, "third", &test3, embb::base::Placeholder::_1)).
    Spawn();
  testDoSomethingElse();
  task.Wait(MTAPI_INFINITE);
  PT_EXPECT(test1 == "first");
  PT_EXPECT(test2 == "second");
  PT_EXPECT(test3 == "third");

  int value = 0;
  task = node.Spawn(
    embb::base::Bind(
      testRecursiveTaskAction, &value, embb::base::Placeholder::_1));
  task.Wait(MTAPI_INFINITE);
  PT_EXPECT(value == 1000);

  mtapi_status_t status;
  task = node.Spawn(testErrorTaskAction);
  testDoSomethingElse();
  status = task.Wait(MTAPI_INFINITE);
  PT_EXPECT(MTAPI_ERR_ACTION_FAILED == status);

  embb::tasks::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}
