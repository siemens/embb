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
#include <mtapi_cpp_test_group.h>

#include <embb/base/c/memory_allocation.h>

#define JOB_TEST_GROUP 5
#define TASK_COUNT 4

struct result_example_struct {
  int value1;
  int value2;
};

typedef struct result_example_struct result_example_t;

static void testGroupAction(
  const void* args,
  mtapi_size_t /*args_size*/,
  void* results,
  mtapi_size_t /*results_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/) {
  result_example_t const * in = static_cast<result_example_t const *>(args);
  result_example_t * out = static_cast<result_example_t *>(results);
  out->value2 = in->value1;
}

static void testDoSomethingElse() {
}

GroupTest::GroupTest() {
  CreateUnit("mtapi_cpp group test").Add(&GroupTest::TestBasic, this);
}

void GroupTest::TestBasic() {
  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID);
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

  embb::mtapi::Job job = node.GetJob(JOB_TEST_GROUP);
  embb::mtapi::Action action =
    node.CreateAction(JOB_TEST_GROUP, testGroupAction);

  embb::mtapi::Group group;

  {
    group = node.CreateGroup();

    result_example_t buffer[TASK_COUNT];
    for (int ii = 0; ii < TASK_COUNT; ii++) {
      buffer[ii].value1 = ii;
      buffer[ii].value2 = -1;
      group.Start(job, &buffer[ii], &buffer[ii]);
    }

    testDoSomethingElse();

    group.WaitAll();

    for (int ii = 0; ii < TASK_COUNT; ii++) {
      PT_EXPECT_EQ(buffer[ii].value1, ii);
      PT_EXPECT_EQ(buffer[ii].value2, ii);
    }
  }

  {
    group = node.CreateGroup();

    result_example_t buffer[TASK_COUNT];
    for (int ii = 0; ii < 4; ii++) {
      buffer[ii].value1 = ii;
      buffer[ii].value2 = -1;
      group.Start(job, &buffer[ii], &buffer[ii]);
    }

    testDoSomethingElse();

    mtapi_status_t status;
    result_example_t* result;
    while (MTAPI_SUCCESS ==
      (status = group.WaitAny(reinterpret_cast<void**>(&result)))) {
      PT_EXPECT(result != MTAPI_NULL);
      PT_EXPECT_EQ(result->value1, result->value2);
    }
    PT_EXPECT_EQ(status, MTAPI_GROUP_COMPLETED);
  }

  action.Delete();
  embb::mtapi::Node::Finalize();

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}
