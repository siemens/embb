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

#include <embb_mtapi_test_config.h>
#include <embb_mtapi_test_error.h>

#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/internal/unused.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/atomic.h>

#include <embb_mtapi_task_context_t.h>
#include <embb_mtapi_thread_context_t.h>


#define JOB_TEST_TASK 42
#define TASK_TEST_ID 23
#define INVALID_ATTRIBUTE 255

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
  CreateUnit("mtapi error test").Add(&ErrorTest::TestBasic, this);
}

static void TestNodeNotInit() {
  mtapi_status_t status;
  mtapi_affinity_t affinity;
  mtapi_job_hndl_t job_hndl;
  mtapi_queue_hndl_t queue_hndl;
  mtapi_group_hndl_t group_hndl;
  mtapi_action_hndl_t action_hndl;

  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_node_id_get(&status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_node_get_attribute(THIS_NODE_ID,
    MTAPI_NODE_MAX_ACTIONS, MTAPI_NULL, 0,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_domain_id_get(&status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);


  status = MTAPI_ERR_UNKNOWN;
  action_hndl = mtapi_action_create(1, testErrorAction, MTAPI_NULL, 0,
    MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_set_attribute(action_hndl,
    MTAPI_ACTION_GLOBAL,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_TRUE), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_get_attribute(action_hndl,
    MTAPI_ACTION_GLOBAL, MTAPI_NULL, 0,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_enable(action_hndl, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_disable(action_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(action_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);


  status = MTAPI_ERR_UNKNOWN;
  job_hndl = mtapi_job_get(1, THIS_DOMAIN_ID, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);


  status = MTAPI_ERR_UNKNOWN;
  queue_hndl = mtapi_queue_create(1, job_hndl,
    MTAPI_DEFAULT_QUEUE_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_enable(queue_hndl, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_disable(queue_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_delete(queue_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_get_attribute(queue_hndl,
    MTAPI_QUEUE_DOMAIN_SHARED, MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_set_attribute(queue_hndl,
    MTAPI_QUEUE_DOMAIN_SHARED, MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);


  status = MTAPI_ERR_UNKNOWN;
  group_hndl = mtapi_group_create(MTAPI_GROUP_ID_NONE,
    MTAPI_DEFAULT_GROUP_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_delete(group_hndl, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_get_attribute(group_hndl,
    0, MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_set_attribute(group_hndl,
    0, MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_affinity_init(&affinity, MTAPI_TRUE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_affinity_set(&affinity, 0, MTAPI_TRUE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_affinity_get(&affinity, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_NOTINIT);


  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}

static void TestLimits() {
  mtapi_status_t status;
  mtapi_node_attributes_t node_attr;
  mtapi_action_hndl_t action_hndl, action_hndl_invalid;
  mtapi_job_hndl_t job_hndl, job_hndl_invalid;
  mtapi_task_hndl_t task_hndl, task_hndl_invalid;
  mtapi_group_hndl_t group_hndl, group_hndl_invalid;
  mtapi_queue_hndl_t queue_hndl, queue_hndl_invalid;
  char buffer[128];

  /* initialize a node with all limits set to 1 */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_nodeattr_init(&node_attr, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_ACTIONS,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_ACTIONS_PER_JOB,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_GROUPS,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_QUEUES,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_PRIORITIES,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_JOBS,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_MAX_TASKS,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_nodeattr_set(&node_attr,
    MTAPI_NODE_QUEUE_LIMIT,
    MTAPI_ATTRIBUTE_VALUE(1), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    &node_attr, MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);


  /* try to get an attribute with invalid attribute pointer */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_node_get_attribute(THIS_NODE_ID,
    MTAPI_NODE_MAX_ACTIONS, MTAPI_NULL, 0,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);


  /* create our test action */
  status = MTAPI_ERR_UNKNOWN;
  action_hndl = mtapi_action_create(1, testErrorAction, MTAPI_NULL, 0,
    MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_disable(action_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_enable(action_hndl, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_set_attribute(action_hndl, MTAPI_ACTION_GLOBAL,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_TRUE), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_get_attribute(action_hndl,
    MTAPI_ACTION_GLOBAL, &buffer, MTAPI_ACTION_GLOBAL_SIZE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try to get an attribute of that action with invalid attribute pointer */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_get_attribute(action_hndl,
    MTAPI_ACTION_GLOBAL, MTAPI_NULL, 0,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  /* try to get an attribute of that action with invalid attribute number */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_get_attribute(action_hndl,
    INVALID_ATTRIBUTE, &buffer, 0,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);

  /* try to create another action, since the limit is one this will fail */
  status = MTAPI_ERR_UNKNOWN;
  action_hndl_invalid = mtapi_action_create(1, testErrorAction, MTAPI_NULL, 0,
    MTAPI_DEFAULT_ACTION_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_LIMIT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_enable(action_hndl_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_disable(action_hndl_invalid, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_delete(action_hndl_invalid, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_set_attribute(action_hndl_invalid, MTAPI_ACTION_DOMAIN_SHARED,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_TRUE), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_action_get_attribute(action_hndl_invalid, MTAPI_ACTION_DOMAIN_SHARED,
    MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ACTION_INVALID);


  /* get handle for job number 1, associated with our action */
  status = MTAPI_ERR_UNKNOWN;
  job_hndl = mtapi_job_get(1, THIS_DOMAIN_ID, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try to get the invalid job number 2, limit is 1 */
  status = MTAPI_ERR_UNKNOWN;
  job_hndl_invalid = mtapi_job_get(2, THIS_DOMAIN_ID, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_JOB_INVALID);


  /* start a task with the invalid job, this will fail */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_start(MTAPI_TASK_ID_NONE, job_hndl_invalid,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_JOB_INVALID);


  /* test if executing a task works, so do not wait */
  embb_atomic_store_int(&wait, 0);

  status = MTAPI_ERR_UNKNOWN;
  task_hndl = mtapi_task_start(MTAPI_TASK_ID_NONE, job_hndl,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_wait(task_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* task should have executed, wait contains 2 */
  PT_EXPECT_EQ(embb_atomic_load_int(&wait), 2);


  /* this time wait, so we can test the task limit */
  embb_atomic_store_int(&wait, 1);

  /* this task will wait until wait is set to 0 */
  status = MTAPI_ERR_UNKNOWN;
  task_hndl = mtapi_task_start(MTAPI_TASK_ID_NONE, job_hndl,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* we cannot start another task since the limit is 1 */
  status = MTAPI_ERR_UNKNOWN;
  task_hndl_invalid = mtapi_task_start(MTAPI_TASK_ID_NONE, job_hndl,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_TASK_LIMIT);

  /* let the waiting task do its work */
  embb_atomic_store_int(&wait, 0);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_wait(task_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* task should have executed, wait contains 2 */
  PT_EXPECT_EQ(embb_atomic_load_int(&wait), 2);


  /* create a group */
  status = MTAPI_ERR_UNKNOWN;
  group_hndl = mtapi_group_create(MTAPI_GROUP_ID_NONE,
    MTAPI_DEFAULT_GROUP_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try to create another group, this will fail since the limit is 1 */
  status = MTAPI_ERR_UNKNOWN;
  group_hndl_invalid = mtapi_group_create(MTAPI_GROUP_ID_NONE,
    MTAPI_DEFAULT_GROUP_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_GROUP_LIMIT);

  /* try to delete the invalid group, this will fail */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_delete(group_hndl_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_GROUP_INVALID);

  /* delete the valid group */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_delete(group_hndl, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try to get an attribute of the deleted group */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_get_attribute(group_hndl, 0, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_GROUP_INVALID);

  /* try to set an attribute of the deleted group */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_group_set_attribute(group_hndl, 0, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_GROUP_INVALID);


  /* create a queue with invalid job */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_create(MTAPI_QUEUE_ID_NONE, job_hndl_invalid,
    MTAPI_DEFAULT_QUEUE_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_JOB_INVALID);

  /* create a queue */
  status = MTAPI_ERR_UNKNOWN;
  queue_hndl = mtapi_queue_create(MTAPI_QUEUE_ID_NONE, job_hndl,
    MTAPI_DEFAULT_QUEUE_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_disable(queue_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_enable(queue_hndl, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try to create another queue, this will fail since the limit is 1 */
  status = MTAPI_ERR_UNKNOWN;
  queue_hndl_invalid = mtapi_queue_create(MTAPI_QUEUE_ID_NONE, job_hndl,
    MTAPI_DEFAULT_QUEUE_ATTRIBUTES, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_LIMIT);

  /* try to enable the invalid queue */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_enable(queue_hndl_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_INVALID);

  /* try to disable the invalid queue */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_disable(queue_hndl_invalid, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_INVALID);

  /* try to delete the invalid queue */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_delete(queue_hndl_invalid, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_set_attribute(queue_hndl_invalid, MTAPI_QUEUE_DOMAIN_SHARED,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_TRUE), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_get_attribute(queue_hndl_invalid, MTAPI_QUEUE_DOMAIN_SHARED,
    MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_INVALID);


  /* now test the task limit via queues, let the first one wait */
  embb_atomic_store_int(&wait, 1);

  /* enqueue the task */
  status = MTAPI_ERR_UNKNOWN;
  task_hndl = mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue_hndl,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* enqueue another one, this will fail since the limit is one */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue_hndl,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_TASK_LIMIT);

  /* let the valid task do its work */
  embb_atomic_store_int(&wait, 0);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_wait(task_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* the task has completed, wait should contain 2 */
  PT_EXPECT_EQ(embb_atomic_load_int(&wait), 2);


  /* disable our queue */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_disable(queue_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_set_attribute(queue_hndl, MTAPI_QUEUE_DOMAIN_SHARED,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_TRUE), MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_get_attribute(queue_hndl, MTAPI_QUEUE_DOMAIN_SHARED,
    MTAPI_NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_get_attribute(queue_hndl, INVALID_ATTRIBUTE,
    &buffer, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_get_attribute(queue_hndl, MTAPI_QUEUE_DOMAIN_SHARED,
    &buffer, MTAPI_QUEUE_DOMAIN_SHARED_SIZE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try to enqueue another task, this will fail since the queue is disabled */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_task_enqueue(MTAPI_TASK_ID_NONE, queue_hndl,
    MTAPI_NULL, 0, MTAPI_NULL, 0,
    MTAPI_DEFAULT_TASK_ATTRIBUTES, MTAPI_GROUP_NONE, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_QUEUE_DISABLED);

  /* disable our queue */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_queue_delete(queue_hndl, MTAPI_INFINITE, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);


  /* and we're done */
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}

static void TestParameter() {
  mtapi_status_t status;
  mtapi_node_attributes_t node_attr;
  mtapi_action_attributes_t action_attr;
  mtapi_task_attributes_t task_attr;
  mtapi_queue_attributes_t queue_attr;
  mtapi_group_attributes_t group_attr;
  mtapi_info_t info;


  status = MTAPI_ERR_UNKNOWN;
  mtapi_nodeattr_init(MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_nodeattr_init(&node_attr, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_nodeattr_set(&node_attr, INVALID_ATTRIBUTE, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES, MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  /* try second init */
  status = MTAPI_ERR_UNKNOWN;
  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES, &info, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_NODE_INITIALIZED);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_actionattr_init(MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_actionattr_init(&action_attr, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_actionattr_set(&action_attr, INVALID_ATTRIBUTE, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_taskattr_init(MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_taskattr_init(&task_attr, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_taskattr_set(&task_attr, INVALID_ATTRIBUTE, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_queueattr_init(MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queueattr_init(&queue_attr, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_queueattr_set(&queue_attr, INVALID_ATTRIBUTE, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_groupattr_init(MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_PARAMETER);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_groupattr_init(&group_attr, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_groupattr_set(&group_attr, INVALID_ATTRIBUTE, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_ATTR_NUM);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}

void TestContext() {
  mtapi_status_t status;
  mtapi_task_context_t* task_ctx_invalid = MTAPI_NULL;

  /* the following context is considered valid although it is not a real one,
     but is checked against the stored pointers and will lead to
     MTAPI_ERR_CONTEXT_OUTOFCONTEXT */
  embb_mtapi_thread_context_t thread_ctx_storage;
  embb_tss_create(&thread_ctx_storage.tss_id);
  embb_mtapi_task_context_t task_ctx_storage;
  task_ctx_storage.thread_context = &thread_ctx_storage;
  mtapi_task_context_t* task_ctx = &task_ctx_storage;

  status = MTAPI_ERR_UNKNOWN;
  mtapi_initialize(THIS_DOMAIN_ID, THIS_NODE_ID,
    MTAPI_DEFAULT_NODE_ATTRIBUTES, MTAPI_NULL, &status);
  PT_EXPECT_EQ(status, MTAPI_SUCCESS);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_corenum_get(task_ctx_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_instnum_get(task_ctx_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_numinst_get(task_ctx_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_status_set(task_ctx_invalid, MTAPI_SUCCESS, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_runtime_notify(task_ctx_invalid,
    MTAPI_NOTIF_EXECUTE_NEXT, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_INVALID);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_taskstate_get(task_ctx_invalid, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_INVALID);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_corenum_get(task_ctx, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_OUTOFCONTEXT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_instnum_get(task_ctx, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_OUTOFCONTEXT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_numinst_get(task_ctx, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_OUTOFCONTEXT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_status_set(task_ctx, MTAPI_SUCCESS, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_OUTOFCONTEXT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_runtime_notify(task_ctx,
    MTAPI_NOTIF_EXECUTE_NEXT, NULL, 0, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_OUTOFCONTEXT);

  status = MTAPI_ERR_UNKNOWN;
  mtapi_context_taskstate_get(task_ctx, &status);
  PT_EXPECT_EQ(status, MTAPI_ERR_CONTEXT_OUTOFCONTEXT);


  status = MTAPI_ERR_UNKNOWN;
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

  embb_tss_delete(&thread_ctx_storage.tss_id);

  PT_EXPECT_EQ(embb_get_bytes_allocated(), 0u);
}

void ErrorTest::TestBasic() {
  embb_atomic_init_int(&wait, 0);

  TestNodeNotInit();
  TestParameter();
  TestLimits();
  TestContext();

  embb_atomic_destroy_int(&wait);
}
