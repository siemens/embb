#include "filters.h"

#include "int_iterator.h"

#include <exception>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <embb/algorithms/algorithms.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif

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
 * @param x x coordinate of the pixel
 * @param y y coordinate of the pixel
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
 * Apply mean filter with specified size to a stripe and writes result to a
 * buffer (buffer must have size of the original image).
 *
 * @param y_in lowest value of y coordinate of the stripe
 * @param y_out highest value of y coordinate of the stripe
 * @param size size of the square used for the averaging
 * @param width width in pixels of the frame
 * @param height height in pixels of the frame
 * @param in_buffer input buffer
 * @param out_buffer output buffer
 */
void blurStripe(int y_in, int y_end, int size, int width, int height,
  uint8_t* in_buffer, uint8_t* out_buffer) {
  // number of pixels top left
  int const size_lt = (size % 2 != 0) ? size / 2 : size / 2 - 1;
  // number of pixels bottom right
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
 * Apply mean filter with specified size to a stripe and writes result to a
 * buffer (buffer must have size of the original image).
 *
 * @param y_in lowest value of y coordinate of the stripe
 * @param y_out highest value of y coordinate of the stripe
 * @param size size of the square used for the averaging
 * @param width width in pixels of the frame
 * @param height height in pixels of the frame
 * @param in_buffer input buffer
 * @param out_buffer output buffer
 */
void blurStripeParallel(int y_in, int y_end, int size, int width, int height,
  uint8_t* in_buffer, uint8_t* out_buffer) {
  // number of pixels top left
  int const size_lt = (size % 2 != 0) ? size / 2 : size / 2 - 1;
  // number of pixels bottom right
  int const size_rb = size / 2;

  embb::algorithms::ForEach(
    int_iterator(y_in*width), int_iterator(y_end*width), [&](int idx) {
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

}

namespace filters {

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

void applyBlackAndWhiteParallel(AVFrame* frame) {
  av_frame_make_writable(frame);

  int const width = frame->width;
  int const height = frame->height;

  embb::algorithms::ForEach(
    int_iterator(0), int_iterator(width*height), [&](int idx) {
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
  uint8_t* buffer = (uint8_t*)av_malloc(sizeof(uint8_t)*n_bytes);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      p = mapToData(x, y, width);
      gx = 0;
      gy = 0;
      convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
      f_value = (int)sqrt(gx*gx + gy*gy);
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
  uint8_t* buffer = (uint8_t*)av_malloc(sizeof(uint8_t)*n_bytes);

  embb::algorithms::ForEach(
    int_iterator(0), int_iterator(width*height), [&](int idx){
    int x = idx % width;
    int y = idx / width;
    p = mapToData(x, y, width);
    gx = 0;
    gy = 0;
    convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
    f_value = (int)sqrt(gx*gx + gy*gy);
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
  uint8_t* buffer = (uint8_t*)av_malloc(sizeof(uint8_t)*n_bytes);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int p = mapToData(x, y, width);
      int gx = 0;
      int gy = 0;
      convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
      int f_value = (int)sqrt(gx*gx + gy*gy);
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
  uint8_t* buffer = (uint8_t*)av_malloc(sizeof(uint8_t)*n_bytes);

  embb::algorithms::ForEach(
    int_iterator(0), int_iterator(height*width), [&](int idx) {
    int const x = idx % width;
    int const y = idx / width;
    int p = mapToData(x, y, width);
    int gx = 0;
    int gy = 0;
    convolve(x, y, data, width, height, Gx, Gy, &gx, &gy);
    int f_value = (int)sqrt(gx*gx + gy*gy);
    if (f_value > threshold) {
      buffer[p] = 0;
      buffer[p + 1] = 0;
      buffer[p + 2] = 0;
    }
    else {
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
      int factor = (int)((maximum - minimum)*amount);

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

  embb::algorithms::ForEach(
    int_iterator(0), int_iterator(height*width), [&](int idx) {
    int const x = idx % width;
    int const y = idx / width;
    int p = mapToData(x, y, width);
    int r = frame->data[0][p];
    int g = frame->data[0][p + 1];
    int b = frame->data[0][p + 2];
    int maximum = std::max(r, std::max(g, b));
    int minimum = std::min(r, std::min(g, b));
    int factor = (int)((maximum - minimum)*amount);

    frame->data[0][p] =
      (r == maximum) ? std::min(255, r + factor) : std::max(0, r - factor * 2);
    frame->data[0][p + 1] =
      (g == maximum) ? std::min(255, g + factor) : std::max(0, g - factor * 2);
    frame->data[0][p + 2] =
      (b == maximum) ? std::min(255, b + factor) : std::max(0, b - factor * 2);
  });
}

void applyMeanFilter(AVFrame* frame, int size) {
  int width = frame->width;
  int height = frame->height;

  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = (uint8_t*)av_malloc(sizeof(uint8_t)*n_bytes);
  uint8_t* data = frame->data[0];

  blurStripe(0, height, size, width, height, data, buffer);

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

void applyMeanFilterParallel(AVFrame* frame, int size) {
  int width = frame->width;
  int height = frame->height;

  av_frame_make_writable(frame);

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
  uint8_t* buffer = (uint8_t*)av_malloc(sizeof(uint8_t)*n_bytes);
  uint8_t* data = frame->data[0];

  blurStripeParallel(0, height, size, width, height, data, buffer);

  av_free(frame->data[0]);
  frame->data[0] = buffer;
}

}  // namespace filters
