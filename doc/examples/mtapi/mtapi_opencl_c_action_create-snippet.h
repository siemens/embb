  float node_local = 1.0f;
  action = mtapi_opencl_action_create(
    OPENCL_JOB,
    kernel, "AddVector", 32, 4,
    &node_local, sizeof(float),
    &status);
