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

#ifdef _WIN32
#include <WinSock2.h>
#endif

#include <embb/mtapi/c/mtapi_network.h>
#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/atomic.h>
#include <embb/base/c/mutex.h>
#include <embb/base/c/internal/unused.h>
#include <embb_mtapi_network_socket.h>
#include <embb_mtapi_network.h>

#include <embb_mtapi_task_t.h>
#include <embb_mtapi_action_t.h>
#include <embb_mtapi_node_t.h>
#include <embb_mtapi_group_t.h>
#include <mtapi_status_t.h>

#include <assert.h>
#include <string.h>

int embb_mtapi_network_initialize() {
#ifdef _WIN32
  WORD ver_request;
  WSADATA wsa_data;
  int err;

  ver_request = MAKEWORD(2, 2);

  err = WSAStartup(ver_request, &wsa_data);
  if (err != 0) {
    return 0;
  } else {
    return 1;
  }
#else
  return 1;
#endif
}

void embb_mtapi_network_finalize() {
#ifdef _WIN32
  WSACleanup();
#endif
}

enum embb_mtapi_network_operation_enum {
  EMBB_MTAPI_NETWORK_START_TASK = 0x01AFFE01,
  EMBB_MTAPI_NETWORK_RETURN_RESULT = 0x02AFFE02,
  EMBB_MTAPI_NETWORK_RETURN_FAILURE = 0x03AFFE03,
  EMBB_MTAPI_NETWORK_CANCEL_TASK = 0x04AFFE04
};

struct embb_mtapi_network_plugin_struct {
  embb_thread_t thread;
  embb_mtapi_network_socket_t *sockets; // sockets[0] is the listening socket
  int socket_count;
  embb_atomic_int run;
  mtapi_size_t buffer_size;

  embb_mutex_t send_mutex;
  embb_mtapi_network_buffer_t send_buffer;

  embb_mtapi_network_buffer_t recv_buffer;
};

typedef struct embb_mtapi_network_plugin_struct embb_mtapi_network_plugin_t;

static embb_mtapi_network_plugin_t embb_mtapi_network_plugin;

struct embb_mtapi_network_action_struct {
  mtapi_domain_t domain_id;
  mtapi_job_id_t job_id;

  char const * host;
  mtapi_uint16_t port;
  embb_mtapi_network_socket_t socket;

  embb_mutex_t send_mutex;
  embb_mtapi_network_buffer_t send_buffer;
};

typedef struct embb_mtapi_network_action_struct embb_mtapi_network_action_t;

struct embb_mtapi_network_task_struct {
  embb_mtapi_network_socket_t socket;
  int32_t remote_task_id;
  int32_t remote_task_tag;
};

typedef struct embb_mtapi_network_task_struct embb_mtapi_network_task_t;

static void embb_mtapi_network_return_failure(
  int32_t remote_task_id,
  int32_t remote_task_tag,
  mtapi_status_t status,
  embb_mtapi_network_socket_t * socket,
  embb_mtapi_network_buffer_t * buffer) {
  embb_mtapi_network_buffer_clear(buffer);

  // packet size
  embb_mtapi_network_buffer_push_back_int32(
    buffer, 16);

  // operation
  embb_mtapi_network_buffer_push_back_int32(
    buffer, EMBB_MTAPI_NETWORK_RETURN_FAILURE);

  // task handle
  embb_mtapi_network_buffer_push_back_int32(
    buffer, remote_task_id);
  embb_mtapi_network_buffer_push_back_int32(
    buffer, remote_task_tag);

  // status
  embb_mtapi_network_buffer_push_back_int32(
    buffer, (int32_t)status);

  embb_mtapi_network_socket_sendbuffer(
    socket, buffer);
}

static void embb_mtapi_network_task_complete(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        /* not needed right now
        embb_mtapi_action_t * local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, local_task->action);*/

        embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
        embb_mtapi_network_task_t * network_task =
          (embb_mtapi_network_task_t*)local_task->attributes.user_data;
        embb_mtapi_network_buffer_t * send_buf = &plugin->send_buffer;

        embb_atomic_memory_barrier();
        local_task->attributes.complete_func = NULL;
        embb_atomic_memory_barrier();

        // serialize sending of results
        embb_mutex_lock(&plugin->send_mutex);
        embb_mtapi_network_buffer_clear(send_buf);

        if (local_task->error_code == MTAPI_SUCCESS) {
          // actual counts bytes actually put into the buffer
          int actual = 0;
          // expected counts bytes we intended to put into the buffer
          int expected =
            4 +                               // operation
            4 + 4 +                           // remote task handle
            4 +                               // status
            4 + (int)local_task->result_size; // result buffer

                                              // packet size
          actual += embb_mtapi_network_buffer_push_back_int32(
            send_buf, expected);
          expected += 4;

          // operation is "return result"
          actual += embb_mtapi_network_buffer_push_back_int32(
            send_buf, EMBB_MTAPI_NETWORK_RETURN_RESULT);

          // remote task id
          actual += embb_mtapi_network_buffer_push_back_int32(
            send_buf, network_task->remote_task_id);
          actual += embb_mtapi_network_buffer_push_back_int32(
            send_buf, network_task->remote_task_tag);

          // status
          actual += embb_mtapi_network_buffer_push_back_int32(
            send_buf, local_task->error_code);

          // result size
          actual += embb_mtapi_network_buffer_push_back_int32(
            send_buf, (int32_t)local_task->result_size);
          actual += embb_mtapi_network_buffer_push_back_rawdata(
            send_buf, (int32_t)local_task->result_size,
            local_task->result_buffer);

          if (expected == actual) {
            int sent = embb_mtapi_network_socket_sendbuffer(
              &network_task->socket, send_buf);
            assert(sent == send_buf->size);
            EMBB_UNUSED_IN_RELEASE(sent);
          } else {
            embb_mtapi_network_return_failure(
              network_task->remote_task_id,
              network_task->remote_task_tag,
              MTAPI_ERR_UNKNOWN,
              &network_task->socket, send_buf);
          }
        } else {
          embb_mtapi_network_return_failure(
            network_task->remote_task_id,
            network_task->remote_task_tag,
            local_task->error_code,
            &network_task->socket, send_buf);
        }

        // sending done
        embb_mutex_unlock(&plugin->send_mutex);

        // we allocated arguments and results on receive, so free them here
        embb_free((void*)local_task->arguments);
        embb_free(local_task->result_buffer);

        void * data = local_task->attributes.user_data;

        embb_atomic_memory_barrier();
        local_task->attributes.user_data = NULL;
        embb_atomic_memory_barrier();

        embb_free(data);

        local_status = MTAPI_SUCCESS;
      }
    }
  }

  mtapi_status_set(status, local_status);
}

static mtapi_status_t embb_mtapi_network_handle_start_task(
  embb_mtapi_network_socket_t * socket,
  embb_mtapi_network_buffer_t * buffer,
  int packet_size) {
  int32_t domain_id;
  int32_t job_id;
  int32_t results_size;
  void * results;
  int err;
  EMBB_UNUSED_IN_RELEASE(err);

  int32_t arguments_size;
  int32_t remote_task_id;
  int32_t remote_task_tag;
  mtapi_uint_t priority = 0;
  mtapi_job_hndl_t job_hndl;
  mtapi_task_attributes_t task_attr;
  void * arguments;
  mtapi_task_complete_function_t func = embb_mtapi_network_task_complete;
  void * func_void;
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  // check if we have at least 28 bytes
  if (packet_size >= 28) {
    // domain id
    err = embb_mtapi_network_buffer_pop_front_int32(buffer, &domain_id);
    assert(err == 4);
    // job id
    err = embb_mtapi_network_buffer_pop_front_int32(buffer, &job_id);
    assert(err == 4);
    // priority
    err = embb_mtapi_network_buffer_pop_front_int32(
      buffer, (int32_t*)&priority);
    assert(err == 4);
    // remote task handle
    err = embb_mtapi_network_buffer_pop_front_int32(
      buffer, &remote_task_id);
    assert(err == 4);
    err = embb_mtapi_network_buffer_pop_front_int32(
      buffer, &remote_task_tag);
    assert(err == 4);
    // result size
    err = embb_mtapi_network_buffer_pop_front_int32(buffer,
      &results_size);
    assert(err == 4);
    // arguments size
    err = embb_mtapi_network_buffer_pop_front_int32(buffer, &arguments_size);
    assert(err == 4);

    embb_mtapi_network_task_t * network_task =
      (embb_mtapi_network_task_t*)embb_alloc(
        sizeof(embb_mtapi_network_task_t));

    if (network_task == NULL) {
      embb_mtapi_network_return_failure(
        remote_task_id, remote_task_tag, MTAPI_ERR_UNKNOWN,
        socket, buffer);
      return MTAPI_ERR_UNKNOWN;
    }
    network_task->remote_task_id = remote_task_id;
    network_task->remote_task_tag = remote_task_tag;

    // check packet_size again
    if (packet_size == 28 + arguments_size) {
      // allocate buffers
      results = embb_alloc((size_t)results_size);
      if (results == NULL) {
        embb_free(network_task);
        embb_mtapi_network_return_failure(
          remote_task_id, remote_task_tag, MTAPI_ERR_UNKNOWN,
          socket, buffer);
        return MTAPI_ERR_UNKNOWN;
      }
      arguments = embb_alloc((size_t)arguments_size);
      if (arguments == NULL) {
        embb_free(network_task);
        embb_free(results);
        embb_mtapi_network_return_failure(
          remote_task_id, remote_task_tag, MTAPI_ERR_UNKNOWN,
          socket, buffer);
        return MTAPI_ERR_UNKNOWN;
      }

      // arguments
      err = embb_mtapi_network_buffer_pop_front_rawdata(
        buffer, arguments_size, arguments);
      assert(err == arguments_size);

      network_task->socket = *socket;
      mtapi_taskattr_init(&task_attr, &local_status);
      assert(local_status == MTAPI_SUCCESS);
      mtapi_taskattr_set(&task_attr, MTAPI_TASK_USER_DATA,
        (void*)network_task, 0, &local_status);
      assert(local_status == MTAPI_SUCCESS);
      mtapi_boolean_t task_detached = MTAPI_TRUE;
      mtapi_taskattr_set(&task_attr, MTAPI_TASK_DETACHED,
        (void*)&task_detached, sizeof(mtapi_boolean_t), &local_status);
      assert(local_status == MTAPI_SUCCESS);
      mtapi_taskattr_set(&task_attr, MTAPI_TASK_PRIORITY,
        (void*)&priority, sizeof(mtapi_uint_t), &local_status);
      assert(local_status == MTAPI_SUCCESS);
      memcpy(&func_void, &func, sizeof(void*));
      mtapi_taskattr_set(&task_attr, MTAPI_TASK_COMPLETE_FUNCTION,
        func_void, 0, &local_status);
      assert(local_status == MTAPI_SUCCESS);
      job_hndl = mtapi_job_get((mtapi_job_id_t)job_id,
        (mtapi_domain_t)domain_id, &local_status);
      if (local_status == MTAPI_SUCCESS) {
        mtapi_task_start(
          MTAPI_TASK_ID_NONE, job_hndl,
          arguments, (mtapi_size_t)arguments_size,
          results, (mtapi_size_t)results_size,
          &task_attr, MTAPI_GROUP_NONE,
          &local_status);
      }
      if (local_status != MTAPI_SUCCESS) {
        embb_free(network_task);
        embb_free(arguments);
        embb_free(results);
        embb_mtapi_network_return_failure(
          remote_task_id, remote_task_tag, local_status, socket, buffer);
      }
    } else {
      embb_free(network_task);
      embb_mtapi_network_return_failure(
        remote_task_id, remote_task_tag, local_status, socket, buffer);
    }
  }

  return local_status;
}

static mtapi_status_t embb_mtapi_network_handle_return_result(
  embb_mtapi_network_buffer_t * buffer,
  int packet_size) {
  int32_t task_status;
  int32_t task_id;
  int32_t task_tag;

  int32_t results_size;
  int err;
  EMBB_UNUSED_IN_RELEASE(err);
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    mtapi_task_hndl_t task;

    // do we have at least 16 bytes?
    if (packet_size >= 16) {
      // local task id
      err = embb_mtapi_network_buffer_pop_front_int32(buffer, &task_id);
      assert(err == 4);
      err = embb_mtapi_network_buffer_pop_front_int32(buffer, &task_tag);
      assert(err == 4);
      // task status
      err = embb_mtapi_network_buffer_pop_front_int32(
        buffer, &task_status);
      assert(err == 4);
      // result size
      err = embb_mtapi_network_buffer_pop_front_int32(
        buffer, &results_size);
      assert(err == 4);

      // check packet_size again
      if (packet_size == 16 + results_size) {
        task.id = (mtapi_task_id_t)task_id;
        task.tag = (mtapi_uint_t)task_tag;

        if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
          embb_mtapi_task_t * local_task =
            embb_mtapi_task_pool_get_storage_for_handle(
              node->task_pool, task);

          if (embb_mtapi_action_pool_is_handle_valid(
            node->action_pool, local_task->action)) {
            embb_mtapi_action_t * local_action =
              embb_mtapi_action_pool_get_storage_for_handle(
                node->action_pool, local_task->action);

            /* not needed right now
            embb_mtapi_network_action_t * network_action =
            (embb_mtapi_network_action_t*)local_action->plugin_data;*/

            err = embb_mtapi_network_buffer_pop_front_rawdata(
              buffer, results_size, local_task->result_buffer);
            assert(err == results_size);

            local_task->error_code = (mtapi_status_t)task_status;
            embb_atomic_store_int(&local_task->state, MTAPI_TASK_COMPLETED);
            embb_atomic_fetch_and_add_int(&local_action->num_tasks,
              -(int)local_task->attributes.num_instances);

            /* is task associated with a group? */
            if (embb_mtapi_group_pool_is_handle_valid(
              node->group_pool, local_task->group)) {
              embb_mtapi_group_t* local_group =
                embb_mtapi_group_pool_get_storage_for_handle(
                  node->group_pool, local_task->group);
              embb_mtapi_task_queue_push_back(
                &local_group->queue, local_task);
            }

            local_status = MTAPI_SUCCESS;
          }
        }
      }
    }
  }

  return local_status;
}

static mtapi_status_t embb_mtapi_network_handle_return_failure(
  embb_mtapi_network_buffer_t * buffer,
  int packet_size) {
  int32_t task_status;
  int32_t task_id;
  int32_t task_tag;

  int err;
  EMBB_UNUSED_IN_RELEASE(err);
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    mtapi_task_hndl_t task;

    // do we have 12 bytes?
    if (packet_size == 12) {
      // local task id
      err = embb_mtapi_network_buffer_pop_front_int32(buffer, &task_id);
      assert(err == 4);
      err = embb_mtapi_network_buffer_pop_front_int32(buffer, &task_tag);
      assert(err == 4);
      // task status
      err = embb_mtapi_network_buffer_pop_front_int32(
        buffer, &task_status);
      assert(err == 4);

      task.id = (mtapi_task_id_t)task_id;
      task.tag = (mtapi_uint_t)task_tag;

      if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
        embb_mtapi_task_t * local_task =
          embb_mtapi_task_pool_get_storage_for_handle(
            node->task_pool, task);

        if (embb_mtapi_action_pool_is_handle_valid(
          node->action_pool, local_task->action)) {
          embb_mtapi_action_t * local_action =
            embb_mtapi_action_pool_get_storage_for_handle(
              node->action_pool, local_task->action);

          embb_atomic_fetch_and_add_int(&local_action->num_tasks,
            -(int)local_task->attributes.num_instances);
          local_task->error_code = (mtapi_status_t)task_status;
          if (MTAPI_ERR_ACTION_CANCELLED == task_status) {
            embb_atomic_store_int(&local_task->state, MTAPI_TASK_CANCELLED);
          } else {
            embb_atomic_store_int(&local_task->state, MTAPI_TASK_ERROR);
          }

          /* is task associated with a group? */
          if (embb_mtapi_group_pool_is_handle_valid(
            node->group_pool, local_task->group)) {
            embb_mtapi_group_t* local_group =
              embb_mtapi_group_pool_get_storage_for_handle(
                node->group_pool, local_task->group);
            embb_mtapi_task_queue_push_back(
              &local_group->queue, local_task);
          }

          local_status = MTAPI_SUCCESS;
        }
      }
    }
  }

  return local_status;
}

static mtapi_status_t embb_mtapi_network_handle_cancel_task(
  embb_mtapi_network_buffer_t * buffer,
  int packet_size) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int32_t remote_task_id;
  int32_t remote_task_tag;
  int err;
  EMBB_UNUSED_IN_RELEASE(err);

  // do we have 8 bytes?
  if (packet_size == 8) {
    // get task handle
    err = embb_mtapi_network_buffer_pop_front_int32(buffer, &remote_task_id);
    assert(err == 4);
    err = embb_mtapi_network_buffer_pop_front_int32(buffer, &remote_task_tag);
    assert(err == 4);

    if (embb_mtapi_node_is_initialized()) {
      embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

      // search for task to cancel
      for (mtapi_uint_t ii = 1; ii <= node->attributes.max_tasks; ii++) {
        embb_mtapi_task_t * task = &node->task_pool->storage[ii];
        // is this our task?
        if (embb_mtapi_network_task_complete ==
            task->attributes.complete_func) {
          embb_mtapi_network_task_t * network_task =
            (embb_mtapi_network_task_t*)task->attributes.user_data;
          // is this task the one matching the given remote task?
          if (remote_task_id == network_task->remote_task_id &&
            remote_task_tag == network_task->remote_task_tag) {
            mtapi_task_cancel(task->handle, &local_status);
            break;
          }
        }
      }
    }
  }

  return local_status;
}

static int embb_mtapi_network_thread(void * args) {
  embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
  embb_mtapi_network_buffer_t * buffer = &plugin->recv_buffer;
  int err;

  EMBB_UNUSED(args);

  while (embb_atomic_load_int(&plugin->run)) {
    err = embb_mtapi_network_socket_select(
      plugin->sockets, plugin->socket_count, 100);
    if (0 == err) {
      // listening socket, accept connection
      embb_mtapi_network_socket_t accept_socket;
      err = embb_mtapi_network_socket_accept(
        &plugin->sockets[0], &accept_socket);
      if (0 < err) {
        // add socket to socket list
        plugin->sockets[plugin->socket_count] = accept_socket;
        plugin->socket_count++;
      }
    } else if (0 < err) {
      int32_t operation;
      int32_t packet_size;
      embb_mtapi_network_socket_t * socket = &plugin->sockets[err];

      embb_mtapi_network_buffer_clear(buffer);

      err = embb_mtapi_network_socket_recvbuffer_sized(
        socket, buffer, 4);
      if (err == 4) {
        err = embb_mtapi_network_buffer_pop_front_int32(
          buffer, &packet_size);
        assert(err == 4);

        embb_mtapi_network_buffer_clear(buffer);
        err = embb_mtapi_network_socket_recvbuffer_sized(
          socket, buffer, packet_size);
        if (err == packet_size) {
          err = embb_mtapi_network_buffer_pop_front_int32(
            buffer, &operation);
          assert(err == 4);
          packet_size -= 4;

          switch (operation) {
          case EMBB_MTAPI_NETWORK_START_TASK:
            embb_mtapi_network_handle_start_task(socket, buffer, packet_size);
            break;
          case EMBB_MTAPI_NETWORK_RETURN_RESULT:
            embb_mtapi_network_handle_return_result(buffer, packet_size);
            break;
          case EMBB_MTAPI_NETWORK_RETURN_FAILURE:
            embb_mtapi_network_handle_return_failure(buffer, packet_size);
            break;
          case EMBB_MTAPI_NETWORK_CANCEL_TASK:
            embb_mtapi_network_handle_cancel_task(buffer, packet_size);
            break;
          default:
            // invalid, ignore
            break;
          }
        }
      }

      embb_mtapi_network_buffer_clear(buffer);
    }
  }

  return EMBB_SUCCESS;
}

void mtapi_network_plugin_initialize(
  MTAPI_IN char * host,
  MTAPI_IN mtapi_uint16_t port,
  MTAPI_IN mtapi_uint16_t max_connections,
  MTAPI_IN mtapi_size_t buffer_size,
  MTAPI_OUT mtapi_status_t* status) {
  embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
  int err;

  mtapi_status_set(status, MTAPI_ERR_UNKNOWN);

  plugin->socket_count = 0;
  plugin->buffer_size = 0;
  plugin->sockets = NULL;

  err = embb_mtapi_network_initialize();
  if (0 == err) return;

  embb_atomic_init_int(&plugin->run, 0);

  err = embb_mtapi_network_buffer_initialize(
    &plugin->recv_buffer, (int)buffer_size);
  if (0 == err) {
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }

  err = embb_mtapi_network_buffer_initialize(
    &plugin->send_buffer, (int)buffer_size);
  if (0 == err) {
    embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }

  plugin->buffer_size = buffer_size;

  // 1 listening socket and max_connections connections
  // (2 sockets each if local)
  plugin->sockets = (embb_mtapi_network_socket_t*)embb_alloc(
    sizeof(embb_mtapi_network_socket_t) * (1 + max_connections * 2));
  if (NULL == plugin->sockets) {
    embb_mtapi_network_buffer_finalize(&plugin->send_buffer);
    embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);
    plugin->buffer_size = 0;
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }

  err = embb_mutex_init(&plugin->send_mutex, 0);
  if (EMBB_SUCCESS != err) {
    embb_free(plugin->sockets);
    plugin->sockets = NULL;
    embb_mtapi_network_buffer_finalize(&plugin->send_buffer);
    embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);
    plugin->buffer_size = 0;
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }

  err = embb_mtapi_network_socket_initialize(&plugin->sockets[0]);
  if (0 == err) {
    embb_mutex_destroy(&plugin->send_mutex);
    embb_free(plugin->sockets);
    plugin->sockets = NULL;
    embb_mtapi_network_buffer_finalize(&plugin->send_buffer);
    embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);
    plugin->buffer_size = 0;
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }
  plugin->socket_count = 1;

  err = embb_mtapi_network_socket_bind_and_listen(
    &plugin->sockets[0], host, port, max_connections);
  if (0 == err) {
    embb_mtapi_network_socket_finalize(&plugin->sockets[0]);
    plugin->socket_count = 0;
    embb_mutex_destroy(&plugin->send_mutex);
    embb_free(plugin->sockets);
    plugin->sockets = NULL;
    embb_mtapi_network_buffer_finalize(&plugin->send_buffer);
    embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);
    plugin->buffer_size = 0;
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }

  embb_atomic_store_int(&plugin->run, 1);

  err = embb_thread_create(
    &plugin->thread, NULL, embb_mtapi_network_thread, NULL);
  if (EMBB_SUCCESS != err) {
    embb_atomic_store_int(&plugin->run, 0);
    embb_mtapi_network_socket_finalize(&plugin->sockets[0]);
    plugin->socket_count = 0;
    embb_mutex_destroy(&plugin->send_mutex);
    embb_free(plugin->sockets);
    plugin->sockets = NULL;
    embb_mtapi_network_buffer_finalize(&plugin->send_buffer);
    embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);
    plugin->buffer_size = 0;
    embb_atomic_destroy_int(&plugin->run);
    embb_mtapi_network_finalize();
    return;
  }

  mtapi_status_set(status, MTAPI_SUCCESS);
}

void mtapi_network_plugin_finalize(
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_SUCCESS;
  embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
  int err;

  embb_atomic_store_int(&plugin->run, 0);
  embb_thread_join(&plugin->thread, &err);

  embb_mutex_destroy(&plugin->send_mutex);
  embb_mtapi_network_buffer_finalize(&plugin->send_buffer);

  embb_mtapi_network_buffer_finalize(&plugin->recv_buffer);

  embb_mtapi_network_socket_finalize(&plugin->sockets[0]);
  embb_free(plugin->sockets);

  embb_atomic_destroy_int(&plugin->run);

  embb_mtapi_network_finalize();

  mtapi_status_set(status, local_status);
}

static void network_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  // assume failure
  mtapi_status_set(status, MTAPI_ERR_UNKNOWN);

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        embb_mtapi_action_t * local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, local_task->action);

        embb_mtapi_network_action_t * network_action =
          (embb_mtapi_network_action_t*)local_action->plugin_data;
        embb_mtapi_network_buffer_t * send_buf = &network_action->send_buffer;

        // serialize sending
        embb_mutex_lock(&network_action->send_mutex);
        embb_mtapi_network_buffer_clear(send_buf);

        // actual counts bytes actually put into the buffer
        int actual = 0;
        // expected counts bytes we intended to put into the buffer
        int expected =
          4 +                                  // operation
          4 +                                  // domain_id
          4 +                                  // job_id
          4 +                                  // priority
          4 + 4 +                              // task handle
          4 +                                  // result_size
          4 + (int)local_task->arguments_size; // arguments buffer

        // packet size
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)expected);
        expected += 4;

        // operation is "start task"
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, EMBB_MTAPI_NETWORK_START_TASK);

        // domain_id
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)network_action->domain_id);

        // job_id
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)network_action->job_id);

        // priority
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->attributes.priority);

        // task handle
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->handle.id);
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->handle.tag);

        // result size
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->result_size);

        // arguments buffer
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->arguments_size);
        actual += embb_mtapi_network_buffer_push_back_rawdata(
          send_buf, (int32_t)local_task->arguments_size, local_task->arguments);

        // check if everything fit into the buffer
        if (actual == expected) {
          embb_atomic_store_int(&local_task->state, MTAPI_TASK_RUNNING);
          int sent = embb_mtapi_network_socket_sendbuffer(
            &network_action->socket, send_buf);
          // was everything sent?
          if (sent == send_buf->size) {
            // we've done it, success!
            mtapi_status_set(status, MTAPI_SUCCESS);
          } else {
            // could not send the whole task, this will fail on the remote side,
            // so we can safely assume that the task is in error
            embb_atomic_store_int(&local_task->state, MTAPI_TASK_ERROR);
          }
        }

        embb_mtapi_network_buffer_clear(send_buf);
        embb_mutex_unlock(&network_action->send_mutex);
      }
    }
  }
}

static void network_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  // assume failure
  mtapi_status_set(status, MTAPI_ERR_UNKNOWN);

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      if (embb_mtapi_action_pool_is_handle_valid(
        node->action_pool, local_task->action)) {
        embb_mtapi_action_t * local_action =
          embb_mtapi_action_pool_get_storage_for_handle(
            node->action_pool, local_task->action);

        embb_mtapi_network_action_t * network_action =
          (embb_mtapi_network_action_t*)local_action->plugin_data;
        embb_mtapi_network_buffer_t * send_buf = &network_action->send_buffer;

        // serialize sending
        embb_mutex_lock(&network_action->send_mutex);
        embb_mtapi_network_buffer_clear(send_buf);

        // actual counts bytes actually put into the buffer
        int actual = 0;
        // expected counts bytes we intended to put into the buffer
        int expected =
          4 +    // operation
          4 + 4; // task handle

        // packet size
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)expected);
        expected += 4;

        // operation is "cancel task"
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, EMBB_MTAPI_NETWORK_CANCEL_TASK);

        // task handle
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->handle.id);
        actual += embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->handle.tag);

        // check if everything fit into the buffer
        if (actual == expected) {
          int sent = embb_mtapi_network_socket_sendbuffer(
            &network_action->socket, send_buf);
          // was everything sent?
          if (sent == send_buf->size) {
            // we've done it, success!
            mtapi_status_set(status, MTAPI_SUCCESS);
          } else {
            embb_atomic_store_int(&local_task->state, MTAPI_TASK_ERROR);
          }
        } else {
          embb_atomic_store_int(&local_task->state, MTAPI_TASK_ERROR);
        }

        embb_mtapi_network_buffer_clear(send_buf);
        embb_mutex_unlock(&network_action->send_mutex);
      }
    }
  }
}

static void network_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  if (embb_mtapi_node_is_initialized()) {
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
    if (embb_mtapi_action_pool_is_handle_valid(node->action_pool, action)) {
      embb_mtapi_action_t * local_action =
        embb_mtapi_action_pool_get_storage_for_handle(
          node->action_pool, action);
      embb_mtapi_network_action_t * network_action =
        (embb_mtapi_network_action_t *)local_action->plugin_data;

      embb_mutex_destroy(&network_action->send_mutex);
      embb_mtapi_network_buffer_finalize(&network_action->send_buffer);
      embb_mtapi_network_socket_finalize(&network_action->socket);

      embb_free(network_action);
      local_status = MTAPI_SUCCESS;
    }
  }

  mtapi_status_set(status, local_status);
}

mtapi_action_hndl_t mtapi_network_action_create(
  MTAPI_IN mtapi_domain_t domain_id,
  MTAPI_IN mtapi_job_id_t local_job_id,
  MTAPI_IN mtapi_job_id_t remote_job_id,
  MTAPI_IN char * host,
  MTAPI_IN mtapi_uint16_t port,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
  embb_mtapi_network_action_t * action =
    (embb_mtapi_network_action_t*)embb_alloc(
      sizeof(embb_mtapi_network_action_t));
  mtapi_action_hndl_t action_hndl = { 0, 0 };
  int err;

  if (NULL != action) {
    action->domain_id = domain_id;
    action->job_id = remote_job_id;

    err = embb_mtapi_network_buffer_initialize(
      &action->send_buffer, (int)plugin->buffer_size);
    if (0 != err) {
      err = embb_mutex_init(&action->send_mutex, 0);
      if (EMBB_SUCCESS == err) {
        action->host = host;
        action->port = port;
        embb_mtapi_network_socket_initialize(&action->socket);
        err = embb_mtapi_network_socket_connect(&action->socket, host, port);
        if (0 != err) {
          // store socket for select
          plugin->sockets[plugin->socket_count] = action->socket;
          plugin->socket_count++;

          action_hndl = mtapi_ext_plugin_action_create(
            local_job_id,
            network_task_start,
            network_task_cancel,
            network_action_finalize,
            action,
            NULL, 0, // no node local data obviously
            MTAPI_NULL,
            &local_status);
        } else {
          embb_mutex_destroy(&action->send_mutex);
          embb_mtapi_network_socket_finalize(&action->socket);
          embb_mtapi_network_buffer_finalize(&action->send_buffer);
          embb_free(action);
        }
      } else {
        embb_mtapi_network_buffer_finalize(&action->send_buffer);
        embb_free(action);
      }
    } else {
      embb_free(action);
    }
  }

  mtapi_status_set(status, local_status);
  return action_hndl;
}
