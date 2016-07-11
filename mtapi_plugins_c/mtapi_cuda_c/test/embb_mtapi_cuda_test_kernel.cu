extern "C" __global__ void test(
  void* arguments,
  int arguments_size,
  void* result_buffer,
  int result_buffer_size,
  void* node_local_data,
  int node_local_data_size) {
  int ii = blockDim.x * blockIdx.x + threadIdx.x;
  int elements = arguments_size / sizeof(float) / 2;
  if (ii >= elements)
    return;
  float* a = (float*)arguments;
  float* b = ((float*)arguments) + elements;
  float* c = (float*)result_buffer;
  float* d = (float*)node_local_data;
  c[ii] = a[ii] + b[ii] + d[0];
}
