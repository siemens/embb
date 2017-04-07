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

#include <stdlib.h>

#include <mtapi_cpp_test_config.h>
#include <mtapi_cpp_test_error.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/internal/unused.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/atomic.h>

static embb_atomic_int wait;

static void testErrorAction(
  const void* /*args*/,
  mtapi_size_t /*arg_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t* /*task_context*/) {
  while (1 == embb_atomic_load_int(&wait))
    embb_thread_yield();
  embb_atomic_store_int(&wait, 2);
}

ErrorTest::ErrorTest() {
  CreateUnit("mtapi cpp error test").Add(&ErrorTest::TestBasic, this);
}

static void TestNodeNotInit() {
#ifdef EMBB_USE_EXCEPTIONS
  EMBB_TRY {
    embb::mtapi::Node::Finalize();
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  EMBB_TRY {
    embb::mtapi::Affinity affinity;
    EMBB_UNUSED(affinity);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }
#endif

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}

static void TestLimits() {
  /* initialize a node with all limits set to 1, except for job and
     action as we need one each for the internal smp job */
  embb::mtapi::NodeAttributes node_attr;
  node_attr
    .SetMaxActions(2)
    .SetMaxActionsPerJob(1)
    .SetMaxGroups(1)
    .SetMaxQueues(1)
    .SetMaxPriorities(1)
    .SetMaxJobs(2)
    .SetMaxTasks(1)
    .SetQueueLimit(1);

  embb::mtapi::Node::Initialize(THIS_DOMAIN_ID, THIS_NODE_ID, node_attr);

#ifdef EMBB_USE_EXCEPTIONS
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();

  /* create our test action */
  embb::mtapi::Action action = node.CreateAction(1, testErrorAction);

  /* try to create another action, since the limit is one this will fail */
  EMBB_TRY {
    embb::mtapi::Action action_invalid = node.CreateAction(1, testErrorAction);
    EMBB_UNUSED(action_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* get handle for job number 1, associated with our action */
  embb::mtapi::Job job = node.GetJob(1);

  /* try to get the invalid job number 3, limit is 2 */
  EMBB_TRY {
    embb::mtapi::Job job_invalid = node.GetJob(3);
    EMBB_UNUSED(job_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* test if executing a task works, so do not wait */
  embb_atomic_store_int(&wait, 0);

  embb::mtapi::Task task =
    node.Start(job, reinterpret_cast<void*>(MTAPI_NULL),
      reinterpret_cast<void*>(MTAPI_NULL));

  mtapi_status_t status = task.Wait();
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* task should have executed, wait contains 2 */
  PT_EXPECT_EQ(embb_atomic_load_int(&wait), 2);

  /* this time wait, so we can test the task limit */
  embb_atomic_store_int(&wait, 1);

  /* this task will wait until wait is set to 0 */
  task = node.Start(job, reinterpret_cast<void*>(MTAPI_NULL),
    reinterpret_cast<void*>(MTAPI_NULL));

  /* we cannot start another task since the limit is 1 */
  EMBB_TRY {
    embb::mtapi::Task task_invalid =
      node.Start(job, reinterpret_cast<void*>(MTAPI_NULL),
        reinterpret_cast<void*>(MTAPI_NULL));
    EMBB_UNUSED(task_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* let the waiting task do its work */
  embb_atomic_store_int(&wait, 0);

  task.Wait();

  /* task should have executed, wait contains 2 */
  PT_EXPECT_EQ(embb_atomic_load_int(&wait), 2);


  /* create a group */
  embb::mtapi::Group group = node.CreateGroup();

  /* try to create another group, this will fail since the limit is 1 */
  EMBB_TRY {
    embb::mtapi::Group group_invalid = node.CreateGroup();
    EMBB_UNUSED(group_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* delete the valid group */
  group.Delete();


  /* create a queue */
  embb::mtapi::Queue queue = node.CreateQueue(job);
  queue.Disable();
  queue.Enable();

  /* try to create another queue, this will fail since the limit is 1 */
  EMBB_TRY {
    embb::mtapi::Queue queue_invalid = node.CreateQueue(job);
    EMBB_UNUSED(queue_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* now test the task limit via queues, let the first one wait */
  embb_atomic_store_int(&wait, 1);

  /* enqueue the task */
  task = queue.Enqueue(reinterpret_cast<void*>(MTAPI_NULL),
    reinterpret_cast<void*>(MTAPI_NULL));

  /* enqueue another one, this will fail since the limit is one */
  EMBB_TRY {
    embb::mtapi::Task task_invalid =
      queue.Enqueue(reinterpret_cast<void*>(MTAPI_NULL),
        reinterpret_cast<void*>(MTAPI_NULL));
    EMBB_UNUSED(task_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* let the valid task do its work */
  embb_atomic_store_int(&wait, 0);

  task.Wait();

  /* the task has completed, wait should contain 2 */
  PT_EXPECT_EQ(embb_atomic_load_int(&wait), 2);


  /* disable our queue */
  queue.Disable();

  /* try to enqueue another task, this will fail since the queue is disabled */
  EMBB_TRY {
    embb::mtapi::Task task_invalid =
      queue.Enqueue(reinterpret_cast<void*>(MTAPI_NULL),
        reinterpret_cast<void*>(MTAPI_NULL));
    EMBB_UNUSED(task_invalid);
    PT_EXPECT(false);
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    PT_EXPECT(true);
  }

  /* delete our queue */
  queue.Delete();

  /* delete our action */
  action.Delete();
#endif

  /* and we're done */
  embb::mtapi::Node::Finalize();

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}

void ErrorTest::TestBasic() {
  embb_atomic_init_int(&wait, 0);

  TestNodeNotInit();
  TestLimits();

  embb_atomic_destroy_int(&wait);
}
