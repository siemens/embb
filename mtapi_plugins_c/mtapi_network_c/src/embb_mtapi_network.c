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
  EMBB_MTAPI_NETWORK_START_TASK,
  EMBB_MTAPI_NETWORK_RETURN_RESULT
};

struct embb_mtapi_network_plugin_struct {
  embb_thread_t thread;
  embb_mtapi_network_socket_t *sockets; // sockets[0] is the listening socket
  int socket_count;
  embb_atomic_int run;
  mtapi_size_t buffer_size;

  embb_mutex_t send_mutex;
  embb_mtapi_network_buffer_t send_buffer;
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

static void embb_mtapi_network_task_complete(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int err;
  EMBB_UNUSED_IN_RELEASE(err);

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

        // serialize sending of results
        embb_mutex_lock(&plugin->send_mutex);
        embb_mtapi_network_buffer_clear(send_buf);

        // operation is "return result"
        err = embb_mtapi_network_buffer_push_back_int8(
          send_buf, EMBB_MTAPI_NETWORK_RETURN_RESULT);
        assert(err == 1);
        // remote task id
        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, network_task->remote_task_id);
        assert(err == 4);
        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, network_task->remote_task_tag);
        assert(err == 4);
        // status
        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, local_task->error_code);
        assert(err == 4);
        // result size
        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->result_size);
        assert(err == 4);
        err = embb_mtapi_network_buffer_push_back_rawdata(
          send_buf, (int32_t)local_task->result_size, local_task->result_buffer);
        assert(err == (int)local_task->result_size);

        err = embb_mtapi_network_socket_sendbuffer(
          &network_task->socket, send_buf);
        assert(err == send_buf->size);

        // sending done
        embb_mutex_unlock(&plugin->send_mutex);

        // we allocated arguments and results on receive, so free them here
        embb_free((void*)local_task->arguments);
        embb_free(local_task->result_buffer);

        local_status = MTAPI_SUCCESS;
      }
    }
  }

  mtapi_status_set(status, local_status);
}

static int embb_mtapi_network_thread(void * args) {
  embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
  embb_mtapi_network_buffer_t buffer;
  int err;

  EMBB_UNUSED(args);

  embb_mtapi_network_buffer_initialize(&buffer, (int)plugin->buffer_size);

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
      int32_t domain_id;
      int32_t job_id;
      int32_t results_size;
      void * results;
      int8_t operation;
      embb_mtapi_network_socket_t * socket = &plugin->sockets[err];

      embb_mtapi_network_buffer_clear(&buffer);

      err = embb_mtapi_network_socket_recvbuffer_sized(
        socket, &buffer, 1);
      if (err == 0) {
        // there was some socket error, ignore
        continue;
      }
      assert(err == 1);
      err = embb_mtapi_network_buffer_pop_front_int8(
        &buffer, &operation);
      assert(err == 1);

      embb_mtapi_network_buffer_clear(&buffer);

      if (operation == EMBB_MTAPI_NETWORK_START_TASK) {
        int32_t arguments_size;
        mtapi_uint_t priority = 0;
        mtapi_job_hndl_t job_hndl;
        mtapi_task_attributes_t task_attr;
        void * arguments;
        mtapi_task_complete_function_t func = embb_mtapi_network_task_complete;
        void * func_void;
        embb_mtapi_network_task_t * network_task =
          (embb_mtapi_network_task_t*)embb_alloc(
          sizeof(embb_mtapi_network_task_t));
        mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

        err = embb_mtapi_network_socket_recvbuffer_sized(
          socket, &buffer, 28);
        assert(err == 28);
        // domain id
        err = embb_mtapi_network_buffer_pop_front_int32(&buffer, &domain_id);
        assert(err == 4);
        // job id
        err = embb_mtapi_network_buffer_pop_front_int32(&buffer, &job_id);
        assert(err == 4);
        // priority
        err = embb_mtapi_network_buffer_pop_front_int32(
          &buffer, (int32_t*)&priority);
        assert(err == 4);
        // remote task handle
        err = embb_mtapi_network_buffer_pop_front_int32(
          &buffer, &network_task->remote_task_id);
        assert(err == 4);
        err = embb_mtapi_network_buffer_pop_front_int32(
          &buffer, &network_task->remote_task_tag);
        assert(err == 4);
        // result size
        err = embb_mtapi_network_buffer_pop_front_int32(&buffer,
          &results_size);
        assert(err == 4);
        results = embb_alloc((size_t)results_size);
        assert(results != NULL);
        // arguments size
        embb_mtapi_network_buffer_pop_front_int32(&buffer, &arguments_size);
        assert(err == 4);
        arguments = embb_alloc((size_t)arguments_size);
        assert(arguments != NULL);

        embb_mtapi_network_buffer_clear(&buffer);

        // arguments
        err = embb_mtapi_network_socket_recvbuffer_sized(
          socket, &buffer, arguments_size);
        assert(err == arguments_size);
        err = embb_mtapi_network_buffer_pop_front_rawdata(
          &buffer, arguments_size, arguments);
        assert(err == arguments_size);

        embb_mtapi_network_buffer_clear(&buffer);

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
        job_hndl = mtapi_job_get((mtapi_job_id_t)job_id, (mtapi_domain_t)domain_id, &local_status);
        assert(local_status == MTAPI_SUCCESS);
        mtapi_task_start(
          MTAPI_TASK_ID_NONE, job_hndl,
          arguments, (mtapi_size_t)arguments_size,
          results, (mtapi_size_t)results_size,
          &task_attr, MTAPI_GROUP_NONE,
          &local_status);
        assert(local_status == MTAPI_SUCCESS);

        // send back result of task creation
        //embb_mtapi_network_buffer_push_back_int32(
        //  &buffer, local_status);
        //embb_mtapi_network_socket_sendbuffer(
        //  socket, &buffer);

        embb_mtapi_network_buffer_clear(&buffer);
      } else if (operation == EMBB_MTAPI_NETWORK_RETURN_RESULT) {
        int task_status;
        int task_id;
        int task_tag;

        embb_mtapi_network_buffer_clear(&buffer);

        if (embb_mtapi_node_is_initialized()) {
          embb_mtapi_node_t * node = embb_mtapi_node_get_instance();
          mtapi_task_hndl_t task;

          err = embb_mtapi_network_socket_recvbuffer_sized(
            socket, &buffer, 16);
          assert(err == 16);
          // local task id
          err = embb_mtapi_network_buffer_pop_front_int32(&buffer, &task_id);
          assert(err == 4);
          err = embb_mtapi_network_buffer_pop_front_int32(&buffer, &task_tag);
          assert(err == 4);
          // task status
          err = embb_mtapi_network_buffer_pop_front_int32(
            &buffer, &task_status);
          assert(err == 4);
          // result size
          err = embb_mtapi_network_buffer_pop_front_int32(
            &buffer, &results_size);
          assert(err == 4);

          embb_mtapi_network_buffer_clear(&buffer);

          err = embb_mtapi_network_socket_recvbuffer_sized(
            socket, &buffer, results_size);
          assert(err == results_size);

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
                &buffer, results_size, local_task->result_buffer);
              assert(err == results_size);

              local_task->error_code = (mtapi_status_t)task_status;
              local_task->state = MTAPI_TASK_COMPLETED;
              embb_atomic_fetch_and_add_int(&local_action->num_tasks, -1);

              /* is task associated with a group? */
              if (embb_mtapi_group_pool_is_handle_valid(
                node->group_pool, local_task->group)) {
                embb_mtapi_group_t* local_group =
                  embb_mtapi_group_pool_get_storage_for_handle(
                  node->group_pool, local_task->group);
                embb_mtapi_task_queue_push(&local_group->queue, local_task);
              }
            }
          }
        }
      }
    }
  }

  embb_mtapi_network_buffer_finalize(&buffer);

  return EMBB_SUCCESS;
}

void mtapi_network_plugin_initialize(
  MTAPI_IN char * host,
  MTAPI_IN mtapi_uint16_t port,
  MTAPI_IN mtapi_uint16_t max_connections,
  MTAPI_IN mtapi_size_t buffer_size,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  embb_mtapi_network_plugin_t * plugin = &embb_mtapi_network_plugin;
  int err;

  err = embb_mtapi_network_initialize();
  if (err) {
    embb_atomic_store_int(&plugin->run, 1);
    plugin->buffer_size = buffer_size;

    plugin->socket_count = 1;
    // 1 listening socket and max_connections connections
    // (2 sockets each if local)
    plugin->sockets = (embb_mtapi_network_socket_t*)embb_alloc(
      sizeof(embb_mtapi_network_socket_t) * (1 + max_connections * 2));

    embb_mtapi_network_buffer_initialize(
      &plugin->send_buffer, (int)plugin->buffer_size);
    embb_mutex_init(&plugin->send_mutex, 0);

    if (NULL != plugin->sockets) {
      err = embb_mtapi_network_socket_initialize(&plugin->sockets[0]);
      if (err) {
        err = embb_mtapi_network_socket_bind_and_listen(
          &plugin->sockets[0], host, port, max_connections);
        if (err) {
          err = embb_thread_create(
            &plugin->thread, NULL, embb_mtapi_network_thread, NULL);
          if (EMBB_SUCCESS == err) {
            local_status = MTAPI_SUCCESS;
          }
        }
      }
    }
  }

  mtapi_status_set(status, local_status);
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

  embb_mtapi_network_socket_finalize(&plugin->sockets[0]);
  embb_free(plugin->sockets);
  embb_mtapi_network_finalize();

  mtapi_status_set(status, local_status);
}

static void network_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;
  int err;
  EMBB_UNUSED_IN_RELEASE(err);

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

        // operation is "start task"
        err = embb_mtapi_network_buffer_push_back_int8(
          send_buf, EMBB_MTAPI_NETWORK_START_TASK);
        assert(err == 1);

        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)network_action->domain_id);
        assert(err == 4);

        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)network_action->job_id);
        assert(err == 4);

        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->attributes.priority);
        assert(err == 4);

        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->handle.id);
        assert(err == 4);
        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->handle.tag);
        assert(err == 4);

        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->result_size);
        assert(err == 4);

        err = embb_mtapi_network_buffer_push_back_int32(
          send_buf, (int32_t)local_task->arguments_size);
        assert(err == 4);
        err = embb_mtapi_network_buffer_push_back_rawdata(
          send_buf, (int32_t)local_task->arguments_size, local_task->arguments);
        assert(err == (int)local_task->arguments_size);

        err = embb_mtapi_network_socket_sendbuffer(
          &network_action->socket, send_buf);
        assert(err == send_buf->size);

        embb_atomic_fetch_and_add_int(&local_action->num_tasks, 1);
        local_task->state = MTAPI_TASK_RUNNING;

        embb_mtapi_network_buffer_clear(send_buf);

        embb_mutex_unlock(&network_action->send_mutex);

        local_status = MTAPI_SUCCESS;
      }
    }
  }

  mtapi_status_set(status, local_status);
}

static void network_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  EMBB_UNUSED(task);

  mtapi_status_set(status, local_status);
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

    embb_mtapi_network_buffer_initialize(
      &action->send_buffer, (int)plugin->buffer_size);
    embb_mutex_init(&action->send_mutex, 0);

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
      embb_mtapi_network_buffer_finalize(&action->send_buffer);
      embb_mtapi_network_socket_finalize(&action->socket);
      embb_free(action);
    }
  }

  mtapi_status_set(status, local_status);
  return action_hndl;
}
