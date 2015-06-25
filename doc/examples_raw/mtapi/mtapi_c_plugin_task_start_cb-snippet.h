void plugin_task_start(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status) {
  mtapi_status_t local_status = MTAPI_ERR_UNKNOWN;

  // do we have a node?
  if (embb_mtapi_node_is_initialized()) {
    // get the node instance
    embb_mtapi_node_t * node = embb_mtapi_node_get_instance();

    // is this a valid task?
    if (embb_mtapi_task_pool_is_handle_valid(node->task_pool, task)) {
      // get the tasks storage
      embb_mtapi_task_t * local_task =
        embb_mtapi_task_pool_get_storage_for_handle(node->task_pool, task);

      // dispatch the task
      plugin_task_schedule(local_task);

      local_status = MTAPI_SUCCESS;
    }
    else {
      local_status = MTAPI_ERR_TASK_INVALID;
    }
  }
  else {
    local_status = MTAPI_ERR_NODE_NOTINIT;
  }

  mtapi_status_set(status, local_status);
}
