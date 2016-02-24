void plugin_task_cancel(
  MTAPI_IN mtapi_task_hndl_t task,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(task);
  // nothing to cancel in this simple example
  mtapi_status_set(status, MTAPI_SUCCESS);
}
