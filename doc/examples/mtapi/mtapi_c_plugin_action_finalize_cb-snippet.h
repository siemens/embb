void plugin_action_finalize(
  MTAPI_IN mtapi_action_hndl_t action,
  MTAPI_OUT mtapi_status_t* status
  ) {
  EMBB_UNUSED(action);
  // nothing to do for tearing down the plugin action
  mtapi_status_set(status, MTAPI_SUCCESS);
}
