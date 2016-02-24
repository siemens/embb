static void AddVectorAction(
  void const * arguments,
  mtapi_size_t arguments_size,
  void * result_buffer,
  mtapi_size_t result_buffer_size,
  void const * node_local_data,
  mtapi_size_t node_local_data_size,
  mtapi_task_context_t * context) {
  EMBB_UNUSED(context);
  EMBB_UNUSED(result_buffer_size);
  EMBB_UNUSED(node_local_data_size);
  int elements = static_cast<int>(arguments_size / sizeof(float) / 2);
  float const * a = reinterpret_cast<float const *>(arguments);
  float const * b = reinterpret_cast<float const *>(arguments)+elements;
  float * c = reinterpret_cast<float*>(result_buffer);
  float const * d = reinterpret_cast<float const *>(node_local_data);
  for (int ii = 0; ii < elements; ii++) {
    c[ii] = a[ii] + b[ii] + d[0];
  }
}
