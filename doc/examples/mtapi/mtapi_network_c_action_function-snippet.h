/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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
