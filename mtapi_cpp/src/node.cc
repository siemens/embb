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

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cassert>

#include <embb/base/memory_allocation.h>
#include <embb/base/exceptions.h>
#include <embb/mtapi/mtapi.h>
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
#include <embb/base/mutex.h>
#endif

namespace {

static embb::mtapi::Node * node_instance = NULL;
static embb::base::Mutex init_mutex;

}

namespace embb {
namespace mtapi {

void Node::action_func(
  const void* args,
  mtapi_size_t /*args_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * context) {
  mtapi::Action * action =
    reinterpret_cast<mtapi::Action*>(const_cast<void*>(args));
  mtapi::TaskContext task_context(context);
  (*action)(task_context);
  embb::base::Allocation::Delete(action);
}

Node::Node(
  mtapi_domain_t domain_id,
  mtapi_node_t node_id,
  mtapi_node_attributes_t * attr) {
  mtapi_status_t status;
  mtapi_info_t info;
  mtapi_initialize(domain_id, node_id, attr, &info, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Node could not initialize mtapi");
  }
  core_count_ = info.hardware_concurrency;
  action_handle_ = mtapi_action_create(MTAPI_CPP_TASK_JOB, action_func,
    MTAPI_NULL, 0, MTAPI_NULL, &status);
  if (MTAPI_SUCCESS != status) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Node could not create an action");
  }
}

Node::~Node() {
  for (std::list<Queue*>::iterator ii = queues_.begin();
       ii != queues_.end();
       ++ii) {
    embb::base::Allocation::Delete(*ii);
  }
  queues_.clear();

  for (std::list<Group*>::iterator ii = groups_.begin();
       ii != groups_.end();
       ++ii) {
    embb::base::Allocation::Delete(*ii);
  }
  groups_.clear();

  mtapi_status_t status;
  mtapi_action_delete(action_handle_, MTAPI_INFINITE, &status);
  assert(MTAPI_SUCCESS == status);
  mtapi_finalize(&status);
  assert(MTAPI_SUCCESS == status);
}

void Node::Initialize(
  mtapi_domain_t domain_id,
  mtapi_node_t node_id) {
  if (IsInitialized()) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Node was already initialized");
  } else {
    mtapi_status_t status;
    mtapi_node_attributes_t attr;
    mtapi_uint_t tmp;
    mtapi_nodeattr_init(&attr, &status);
    assert(MTAPI_SUCCESS == status);
    tmp = 4;
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_ACTIONS,
      &tmp, sizeof(tmp), &status);
    assert(MTAPI_SUCCESS == status);
    tmp = 4;
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_JOBS,
      &tmp, sizeof(tmp), &status);
    assert(MTAPI_SUCCESS == status);
    tmp = 1;
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_ACTIONS_PER_JOB,
      &tmp, sizeof(tmp), &status);
    assert(MTAPI_SUCCESS == status);
    node_instance = embb::base::Allocation::New<Node>(
      domain_id, node_id, &attr);
  }
}

void Node::Initialize(
  mtapi_domain_t domain_id,
  mtapi_node_t node_id,
  embb::base::CoreSet const & core_set,
  mtapi_uint_t max_tasks,
  mtapi_uint_t max_groups,
  mtapi_uint_t max_queues,
  mtapi_uint_t queue_limit,
  mtapi_uint_t max_priorities) {
  if (IsInitialized()) {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Node was already initialized");
  } else {
    mtapi_status_t status;
    mtapi_node_attributes_t attr;
    mtapi_uint_t tmp;
    mtapi_nodeattr_init(&attr, &status);
    assert(MTAPI_SUCCESS == status);
    embb_core_set_t cs;
    embb_core_set_init(&cs, 0);
    for (unsigned int ii = 0; ii < core_set.Count(); ii++) {
      if (core_set.IsContained(ii)) {
        embb_core_set_add(&cs, ii);
      }
    }
    mtapi_nodeattr_set(&attr, MTAPI_NODE_CORE_AFFINITY,
      &cs, sizeof(cs), &status);
    assert(MTAPI_SUCCESS == status);
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_TASKS,
      &max_tasks, sizeof(max_tasks), &status);
    assert(MTAPI_SUCCESS == status);
    tmp = 4;
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_ACTIONS,
      &tmp, sizeof(tmp), &status);
    assert(MTAPI_SUCCESS == status);
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_GROUPS,
      &max_groups, sizeof(max_groups), &status);
    assert(MTAPI_SUCCESS == status);
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_QUEUES,
      &max_queues, sizeof(max_queues), &status);
    assert(MTAPI_SUCCESS == status);
    mtapi_nodeattr_set(&attr, MTAPI_NODE_QUEUE_LIMIT,
      &queue_limit, sizeof(queue_limit), &status);
    assert(MTAPI_SUCCESS == status);
    tmp = 4;
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_JOBS,
      &tmp, sizeof(tmp), &status);
    assert(MTAPI_SUCCESS == status);
    tmp = 1;
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_ACTIONS_PER_JOB,
      &tmp, sizeof(tmp), &status);
    assert(MTAPI_SUCCESS == status);
    mtapi_nodeattr_set(&attr, MTAPI_NODE_MAX_PRIORITIES,
      &max_priorities, sizeof(max_priorities), &status);
    assert(MTAPI_SUCCESS == status);
    node_instance = embb::base::Allocation::New<Node>(
      domain_id, node_id, &attr);
  }
}

bool Node::IsInitialized() {
  return NULL != node_instance;
}

Node & Node::GetInstance() {
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
  if (!IsInitialized()) {
    init_mutex.Lock();
    if (!IsInitialized()) {
      Node::Initialize(
        MTAPI_CPP_AUTOMATIC_DOMAIN_ID, MTAPI_CPP_AUTOMATIC_NODE_ID);
      atexit(Node::Finalize);
    }
    init_mutex.Unlock();
  }
  return *node_instance;
#else
  if (IsInitialized()) {
    return *node_instance;
  } else {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Node is not initialized");
  }
#endif
}

void Node::Finalize() {
  if (IsInitialized()) {
    embb::base::Allocation::Delete(node_instance);
    node_instance = NULL;
  } else {
    EMBB_THROW(embb::base::ErrorException,
      "mtapi::Node is not initialized");
  }
}

Group & Node::CreateGroup() {
  Group * group = embb::base::Allocation::New<Group>();
  groups_.push_back(group);
  return *group;
}

void Node::DestroyGroup(Group & group) {
  std::list<Group*>::iterator ii =
    std::find(groups_.begin(), groups_.end(), &group);
  if (ii != groups_.end()) {
    embb::base::Allocation::Delete(*ii);
    groups_.erase(ii);
  }
}

Queue & Node::CreateQueue(mtapi_uint_t priority, bool ordered) {
  Queue * queue = embb::base::Allocation::New<Queue>(priority, ordered);
  queues_.push_back(queue);
  return *queue;
}

void Node::DestroyQueue(Queue & queue) {
  std::list<Queue*>::iterator ii =
    std::find(queues_.begin(), queues_.end(), &queue);
  if (ii != queues_.end()) {
    embb::base::Allocation::Delete(*ii);
    queues_.erase(ii);
  }
}

Task Node::Spawn(Action action) {
  return Spawn(action, 0);
}

Task Node::Spawn(Action action, mtapi_uint_t priority) {
  return Task(action, priority);
}

Continuation Node::First(Action action) {
  return Continuation(action);
}

} // namespace mtapi
} // namespace embb
