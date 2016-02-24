  float node_remote = 1.0f;
  local_action = mtapi_action_create(
    NETWORK_REMOTE_JOB,
    AddVectorAction,
    &node_remote, sizeof(float),
    MTAPI_DEFAULT_ACTION_ATTRIBUTES,
    &status);
