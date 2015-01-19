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

#include <iostream>
#include <string>
#include <cassert>

#include <mtapi_cpp_test_config.h>
#include <mtapi_cpp_test_task.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23

static void testTaskAction(
  char const * msg,
  std::string * output,
  embb::mtapi::TaskContext & /*context*/) {
  //std::cout << "testTaskAction " << msg << " on core " <<
  //  context.GetCurrentCoreNumber() << std::endl;
  *output = msg;
}

static void testRecursiveTaskAction(
  int * value,
  embb::mtapi::TaskContext & /*context*/) {
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  *value = *value + 1;
  if (*value < 1000) {
    embb::mtapi::Task task = node.Spawn(
      embb::base::Bind(
        testRecursiveTaskAction, value, embb::base::Placeholder::_1));
    task.Wait(MTAPI_INFINITE);
  }
  PT_EXPECT(*value == 1000);
}

static void testErrorTaskAction(embb::mtapi::TaskContext & context) {
  context.SetStatus(MTAPI_ERR_ACTION_FAILED);
}

static void testDoSomethingElse() {
}

TaskTest::TaskTest() {
  CreateUnit("mtapi_cpp task test").Add(&TaskTest::TestBasic, this);
}

void TaskTest::TestBasic() {
  //std::cout << "running testTask..." << std::endl;

  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

  std::string test;
  embb::mtapi::Task task = node.Spawn(
    embb::base::Bind(
      testTaskAction, "simple", &test, embb::base::Placeholder::_1));
  testDoSomethingElse();
  task.Wait(MTAPI_INFINITE);
  PT_EXPECT(test == "simple");
  //std::cout << "result: " << test.c_str() << std::endl;

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
  //std::cout << "result1: " << test1.c_str() << std::endl;
  //std::cout << "result2: " << test2.c_str() << std::endl;
  //std::cout << "result3: " << test3.c_str() << std::endl;

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

  embb::mtapi::Node::Finalize();

  PT_EXPECT(embb_get_bytes_allocated() == 0);
  //std::cout << "...done" << std::endl << std::endl;
}
