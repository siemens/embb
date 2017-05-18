/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
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

#include "../include/filters.h"

#include <exception>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <embb/algorithms/algorithms.h>

#include "../include/ffmpeg.h"

// Note: Not all of the filters implemented in this file are
// actually used by the tutorial application. However, feel
// free to experiment with them.

namespace {

/**
 * Map the point (x,y) into the corresponding index in the 
 * image buffer.
 *
 * @param x x coordinate of the pixel.
 * @param y y coordinate of the pixel.
 * @param width width in pixels of the image
 */
inline int mapToData(int x, int y, int width) {
  return 3*(x+y*width);
}

/**
 * Compute the convolution of up to two kernels at pixel (x,y).
 *
 * @param x x-coordinate of the pixel
 * @param y y-coordinate of the pixel
 * @param data buffer of the image
 * @param width width in pixels of the image
 * @parama height height in pixels of the image
 * @param kernel1 first kernel to be applied (dimension 3x3)
 * @param kernel2 second kernel to be applied (dimension 3x3)
 * @param v_k1 output value of the first kernel
 * @param v_k2 output value of the second kernel
 */
void convolve(int x, int y, uint8_t* data, int width, int height,
  int kernel1[3][3], int kernel2[3][3], int* v_k1, int* v_k2) {
  int x1, y1;
  int p;
  int value1 = 0;
  int value2 = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      x1 = x + i - 1;
      y1 = y + j - 1;
      x1 = std::max(0, std::min(x1, width - 1));
      y1 = std::max(0, std::min(y1, height - 1));
      p = mapToData(x1, y1, width);
      value1 += kernel1[i][j] * (data[p] + data[p + 1] + data[p + 2]) / 3;
      value2 += kernel2[i][j] * (data[p] + data[p + 1] + data[p + 2]) / 3;
    }
    *v_k1 = value1;
    *v_k2 = value2;
  }
}

/**
 * Apply mean filter with specified size to a stripe and write result to a
 * buffer (buffer must have the size of the original image).
 *
 * @param y_in lowest value of y-coordinate of the stripe
 * @param y_out highest value of y-coordinate of the stripe
 * @param size size of the square used for the averaging
 * @param width width in pixels of the frame
 * @param height height in pixels of the frame
 * @param in_buffer input buffer
 * @param out_buffer output buffer
 */
void blurStripe(int y_in, int y_end, int size, int width, int height,
  uint8_t* in_buffer, uint8_t* out_buffer) {
  // Number of pixels top left
  int const size_lt = (size % 2 != 0) ? size / 2 : size / 2 - 1;
  // Number of pixels bottom right
  int const size_rb = size / 2;

  for (int y = y_in; y < y_end; y++) {
    for (int x = 0; x < width; x++) {
      int close = 0;
      int p = mapToData(x, y, width);
      int r = 0;
      int g = 0;
      int b = 0;

      for (int k1 = y - size_lt; k1 <= y + size_rb; k1++) {
        for (int k2 = x - size_lt; k2 <= x + size_rb; k2++) {
          if (k1 >= 0 && k1 < height && k2 >= 0 && k2 < width) {
            close++;
            int p2 = mapToData(k2, k1, width);
            r += in_buffer[p2];
            g += in_buffer[p2 + 1];
            b += in_buffer[p2 + 2];
          }
        }
      }
      out_buffer[p] = r / close;
      out_buffer[p + 1] = g / close;
      out_buffer[p + 2] = b / close;
    }
  }
}

/**
 * Apply mean filter with specified size to a stripe and write result to a
 * buffer (buffer must have size of the original image).
 *
 * @param y_in lowest value of y-coordinate of the stripe
 * @param y_out highest value of y-coordinate of the stripe
 * @param size size of the square used for the averaging
 * @param width width in pixels of the frame
 * @param height height in pixels of the frame
 * @param in_buffer input buffer
 * @param out_buffer output buffer
 */
void blurStripeParallel(int y_in, int y_end, int size, int width, int height,
  uint8_t* in_buffer, uint8_t* out_buffer) {
  // Number of pixels top left
  int const size_lt = (size % 2 != 0) ? size / 2 : size / 2 - 1;
  // Number of pixels bottom right
  int const size_rb = size / 2;

  embb::algorithms::ForLoop(
    y_in*width, y_end*width, [&](int idx) {
    int x = idx % width;
    int y = idx / width;
    int close = 0;
    int p = mapToData(x, y, width);
    int r = 0;
    int g = 0;
    int b = 0;

    for (int k1 = y - size_lt; k1 <= y + size_rb; k1++) {
      for (int k2 = x - size_lt; k2 <= x + size_rb; k2++) {
        if (k1 >= 0 && k1 < height && k2 >= 0 && k2 < width) {
          close++;
          int p2 = mapToData(k2, k1, width);
          r += in_buffer[p2];
          g += in_buffer[p2 + 1];
          b += in_buffer[p2 + 2];
        }
      }
    }
    out_buffer[p] = r / close;
    out_buffer[p + 1] = g / close;
    out_buffer[p + 2] = b / close;
  });
}

} // namespace

namespace filters {

// snippet_begin:opencl_filter
char const * mean_kernel =
"__kernel void mean(\n"
"  __global void* arguments,\n"
"  int arguments_size,\n"
"  __global void* result_buffer,\n"
"  int result_buffer_size,\n"
"  __global void* node_local_data,\n"
"  int node_local_data_size) {\n"
"  int idx = get_global_id(0);\n"
"  int elements = (arguments_size - sizeof(int) * 3) / 3;\n"
"  if (idx >= elements)\n"
"    return;\n"
"  __global int * param = (__global int*)arguments;\n"
"  __global unsigned char * in_buffer ="
"    ((__global unsigned char*)arguments) + sizeof(int) * 3;\n"
"  __global unsigned char * out_buffer ="
"    (__global unsigned char*)result_buffer;\n"
"  int width = param[0];\n"
"  int height = param[1];\n"
"  int size = param[2];\n"
"  int size_lt = (size % 2 != 0) ? size / 2 : size / 2 - 1;\n"
"  int const size_rb = size / 2;\n"
"  int x = idx % width;\n"
"  int y = idx / width; \n"
"  int close = 0;\n"
"  int p = (x + y * width) * 3;\n"
"  int r = 0;\n"
"  int g = 0;\n"
"  int b = 0;\n"
"  for (int k1 = y - size_lt; k1 <= y + size_rb; k1++) {\n"
"    for (int k2 = x - size_lt; k2 <= x + size_rb; k2++) {\n"
"      if (k1 >= 0 && k1 < height && k2 >= 0 && k2 < width) {\n"
"        close++;\n"
"        int p2 = (k2 + k1 * width) * 3;\n"
"        r += in_buffer[p2];\n"
"        g += in_buffer[p2 + 1];\n"
"        b += in_buffer[p2 + 2];\n"
"      }\n"
"    }\n"
"  }\n"
"  out_buffer[p] = r / close;\n"
"  out_buffer[p + 1] = g / close;\n"
"  out_buffer[p + 2] = b / close;\n"
"}\n";
// snippet_end

char const * cartoonify_kernel =
"__kernel void cartoonify(\n"
"  __global void* arguments,\n"
"  int arguments_size,\n"
"  __global void* result_buffer,\n"
"  int result_buffer_size,\n"
"  __global void* node_local_data,\n"
"  int node_local_data_size) {\n"
"  int idx = get_global_id(0);\n"
"  int elements = (arguments_size - sizeof(int) * 4) / 3;\n"
"  if (idx >= elements)\n"
"    return;\n"
"  __global int * param = (__global int*)arguments;\n"
"  __global unsigned char * data ="
"    ((__global unsigned char*)arguments) + sizeof(int) * 4;\n"
"  __global unsigned char * buffer ="
"    (__global unsigned char*)result_buffer;\n"
"  int width = param[0];\n"
"  int height = param[1];\n"
"  int threshold = param[2];\n"
"  int discr = param[3];\n"
"  int x = idx % width;\n"
"  int y = idx / width;\n"
"  int p = (x + y * width) * 3;\n"
"  int Gx[3][3];\n"
"  int Gy[3][3];\n"
"  Gx[0][0] = -1; Gy[0][0] = -1;\n"
"  Gx[0][1] = 0; Gy[0][1] = -2;\n"
"  Gx[0][2] = 1; Gy[0][2] = -1;\n"
"  Gx[1][0] = -2; Gy[1][0] = 0;\n"
"  Gx[1][1] = 0; Gy[1][1] = 0;\n"
"  Gx[1][2] = 2; Gy[1][2] = 0;\n"
"  Gx[2][0] = -1; Gy[2][0] = 1;\n"
"  Gx[2][1] = 0; Gy[2][1] = 2;\n"
"  Gx[2][2] = 1; Gy[2][2] = 1;\n"
"  int gx = 0;\n"
"  int gy = 0;\n"
"  for (int i = 0; i < 3; i++) {\n"
"    for (int j = 0; j < 3; j++) {\n"
"      int x1 = x + i - 1;\n"
"      int y1 = y + j - 1;\n"
"      if (x1 < 0) x1 = 0; if (x1 > width-1) x1 = width-1;\n"
"      if (y1 < 0) y1 = 0; if (y1 > height-1) y1 = height-1;\n"
"      int p1 = (x1 + y1 * width) * 3;\n"
"      gx += Gx[i][j] * (data[p1] + data[p1 + 1] + data[p1 + 2]) / 3;\n"
"      gy += Gy[i][j] * (data[p1] + data[p1 + 1] + data[p1 + 2]) / 3;\n"
"    }\n"
"  }\n"
"  int f_value = (int)sqrt(gx*gx + gy*gy);\n"
"  buffer[p] = f_value;\n"
"  buffer[p + 1] = f_value;\n"
"  buffer[p + 2] = f_value;\n"
"}\n";

// snippet_begin:serial_filter
void applyBlackAndWhite(AVFrame* frame) {
  av_frame_make_writable(frame);

  int const width = frame->width;
  int const height = frame->height;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int p = mapToData(x, y, width);
      int r = frame->data[0][p];
      int g = frame->data[0][p + 1];
      int b = frame->data[0][p + 2];
      int mean = (r + g + b) / 3;
      frame->data[0][p] = mean;
      frame->data[0][p + 1] = mean;
      frame->data[0][p + 2] = mean;
    }
  }
}
// snippet_end

// snippet_begin:parallel_filter
void applyBlackAndWhiteParallel(AVFrame* frame) {
  av_frame_make_writable(frame);

  int const width = frame->width;
  int const height = frame->height;

  embb::algorithms::ForLoop(0, width*height, [&](int idx) {
    int x = idx % width;
    int y = idx / width;
    int p = mapToData(x, y, width);
    int r = frame->data[0][p];
    int g = frame->data[0][p + 1];
    int b = frame->data[0][p + 2];
    int mean = (r + g + b) / 3;
    frame->data[0][p] = mean;
    frame->data[0][p + 1] = mean;
    frame->data[0][p + 2] = mean;
  });
}
// snippet_end

void edgeDetection(AVFrame* frame) {
  int width = frame->width;
  int height = frame->height;
  int gx, gy;
  int Gx[3][3];
  int Gy[3][3];
  int p;
  int f_value;

  Gx[0][0] = -1; Gy[0][0] = -1;
  Gx[0][1] =  0; Gy[0][1] = -2;
  Gx[0][2] =  1; Gy[0][2] = -1;
  Gx[1][0] = -2; Gy[1][0] =  0;
  Gx[1][1] =  0; Gy[1][1] =  0;
  Gx[1][2] =  2; Gy[1][2] =  0;
  Gx[2][0] = -1; Gy[2][0] =  1;
  Gx[2][1] =  0; Gy[2][1] =  2;
  Gx[2][2] =  1; Gy[2][2] =  1;

  uint8_t* data = frame->data[0];
  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)*n_bytes));

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      p = mapToData(x, y, width);
      gx = 0;
      gy = 0;
      convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
      f_value = static_cast<int>(sqrt(gx*gx + gy*gy));
      buffer[p] = f_value;
      buffer[p+1] = f_value;
      buffer[p+2] = f_value;
    }
  }

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

void edgeDetectionParallel(AVFrame* frame) {
  int width = frame->width;
  int height = frame->height;
  int gx, gy;
  int Gx[3][3];
  int Gy[3][3];
  int p;
  int f_value;

  Gx[0][0] = -1; Gy[0][0] = -1;
  Gx[0][1] =  0; Gy[0][1] = -2;
  Gx[0][2] =  1; Gy[0][2] = -1;
  Gx[1][0] = -2; Gy[1][0] =  0;
  Gx[1][1] =  0; Gy[1][1] =  0;
  Gx[1][2] =  2; Gy[1][2] =  0;
  Gx[2][0] = -1; Gy[2][0] =  1;
  Gx[2][1] =  0; Gy[2][1] =  2;
  Gx[2][2] =  1; Gy[2][2] =  1;

  uint8_t* data = frame->data[0];
  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)*n_bytes));

  embb::algorithms::ForLoop(0, width*height, [&](int idx){
    int x = idx % width;
    int y = idx / width;
    p = mapToData(x, y, width);
    gx = 0;
    gy = 0;
    convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
    f_value = static_cast<int>(sqrt(gx*gx + gy*gy));
    buffer[p] = f_value;
    buffer[p + 1] = f_value;
    buffer[p + 2] = f_value;
  });

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

void applyCartoonify(AVFrame* frame, int threshold, int discr) {
  int const width = frame->width;
  int const height = frame->height;
  int Gx[3][3];
  int Gy[3][3];

  Gx[0][0] = -1; Gy[0][0] = -1;
  Gx[0][1] = 0; Gy[0][1] = -2;
  Gx[0][2] = 1; Gy[0][2] = -1;
  Gx[1][0] = -2; Gy[1][0] = 0;
  Gx[1][1] = 0; Gy[1][1] = 0;
  Gx[1][2] = 2; Gy[1][2] = 0;
  Gx[2][0] = -1; Gy[2][0] = 1;
  Gx[2][1] = 0; Gy[2][1] = 2;
  Gx[2][2] = 1; Gy[2][2] = 1;

  uint8_t* data = frame->data[0];
  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)*n_bytes));

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int p = mapToData(x, y, width);
      int gx = 0;
      int gy = 0;
      convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
      int f_value = static_cast<int>(sqrt(gx*gx + gy*gy));
      if (f_value > threshold) {
        buffer[p] = 0;
        buffer[p + 1] = 0;
        buffer[p + 2] = 0;
      } else {
        int r = (data[p] / discr) * discr;
        int g = (data[p + 1] / discr) * discr;
        int b = (data[p + 2] / discr) * discr;

        buffer[p] = r;
        buffer[p + 1] = g;
        buffer[p + 2] = b;
      }
    }
  }

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

void applyCartoonifyParallel(AVFrame* frame, int threshold, int discr) {
  int const width = frame->width;
  int const height = frame->height;
  int Gx[3][3];
  int Gy[3][3];

  Gx[0][0] = -1; Gy[0][0] = -1;
  Gx[0][1] = 0; Gy[0][1] = -2;
  Gx[0][2] = 1; Gy[0][2] = -1;
  Gx[1][0] = -2; Gy[1][0] = 0;
  Gx[1][1] = 0; Gy[1][1] = 0;
  Gx[1][2] = 2; Gy[1][2] = 0;
  Gx[2][0] = -1; Gy[2][0] = 1;
  Gx[2][1] = 0; Gy[2][1] = 2;
  Gx[2][2] = 1; Gy[2][2] = 1;

  uint8_t* data = frame->data[0];
  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)*n_bytes));

  embb::algorithms::ForLoop(0, height*width, [&](int idx) {
    int const x = idx % width;
    int const y = idx / width;
    int p = mapToData(x, y, width);
    int gx = 0;
    int gy = 0;
    convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
    int f_value = static_cast<int>(sqrt(gx*gx + gy*gy));
    if (f_value > threshold) {
      buffer[p] = 0;
      buffer[p + 1] = 0;
      buffer[p + 2] = 0;
    } else {
      int r = (data[p] / discr) * discr;
      int g = (data[p + 1] / discr) * discr;
      int b = (data[p + 2] / discr) * discr;

      buffer[p] = r;
      buffer[p + 1] = g;
      buffer[p + 2] = b;
    }
  });

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

void changeSaturation(AVFrame* frame, double amount) {
  int const width = frame->width;
  int const height = frame->height;

  av_frame_make_writable(frame);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int p = mapToData(x, y, width);
      int r = frame->data[0][p];
      int g = frame->data[0][p + 1];
      int b = frame->data[0][p + 2];
      int maximum = std::max(r, std::max(g, b));
      int minimum = std::min(r, std::min(g, b));
      int factor = static_cast<int>((maximum - minimum)*amount);

      frame->data[0][p] =
        (r == maximum) ? std::min(255, r + factor) : std::max(0, r - factor*2);
      frame->data[0][p + 1] =
        (g == maximum) ? std::min(255, g + factor) : std::max(0, g - factor*2);
      frame->data[0][p + 2] =
        (b == maximum) ? std::min(255, b + factor) : std::max(0, b - factor*2);
    }
  }
}

void changeSaturationParallel(AVFrame* frame, double amount) {
  int const width = frame->width;
  int const height = frame->height;

  av_frame_make_writable(frame);

  embb::algorithms::ForLoop(0, height*width, [&](int idx) {
    int const x = idx % width;
    int const y = idx / width;
    int p = mapToData(x, y, width);
    int r = frame->data[0][p];
    int g = frame->data[0][p + 1];
    int b = frame->data[0][p + 2];
    int maximum = std::max(r, std::max(g, b));
    int minimum = std::min(r, std::min(g, b));
    int factor = static_cast<int>((maximum - minimum)*amount);

    frame->data[0][p] =
      (r == maximum) ? std::min(255, r + factor) : std::max(0, r - factor * 2);
    frame->data[0][p + 1] =
      (g == maximum) ? std::min(255, g + factor) : std::max(0, g - factor * 2);
    frame->data[0][p + 2] =
      (b == maximum) ? std::min(255, b + factor) : std::max(0, b - factor * 2);
  });
}

void applyMeanFilter(AVFrame* frame, int size) {
  int const width = frame->width;
  int const height = frame->height;

  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)*n_bytes));
  uint8_t* data = frame->data[0];

  blurStripe(0, height, size, width, height, data, buffer);

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

void applyMeanFilterParallel(AVFrame* frame, int size) {
  int const width = frame->width;
  int const height = frame->height;

  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)*n_bytes));
  uint8_t* data = frame->data[0];

  blurStripeParallel(0, height, size, width, height, data, buffer);

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

}  // namespace filters
