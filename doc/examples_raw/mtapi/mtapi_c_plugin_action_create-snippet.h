action = mtapi_ext_plugin_action_create(
  PLUGIN_JOB_ID,
  plugin_task_start,
  plugin_task_cancel,
  plugin_action_finalize,
  MTAPI_NULL,
  MTAPI_NULL,
  0,
  MTAPI_DEFAULT_ACTION_ATTRIBUTES,
  &status);
