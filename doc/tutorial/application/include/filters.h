#ifndef _FILTER_H_
#define _FILTER_H_

#include <cstdint>

struct AVFrame;

namespace filters {

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
 * Shuffle the rgb values of the picture: 
 * - red is assigned to green channel
 * - gree is assigned to blue channel
 * - blue is assigned to red channel
 *
 * @param frame frame to be processed
 */
void applyShuffleColors(AVFrame* frame);

/**
 * Convert the frame to black and white by setting
 * the rgb values to the average of the three.
 *
 * @param frame frame to be processed
 */
void applyBlackAndWhite(AVFrame* frame);

/**
 * Convert the frame to its negative by applying
 * the rule 'color = 255 - color' to each channel.
 *
 * @param frame frame to be processed
 */
void applyNegative(AVFrame* frame);

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
  int kernel1[][3], int kernel2[][3], int* v_k1, int* v_k2);

/**
 * Apply the Sobel operator to a frame to detect edges.
 * see https://en.wikipedia.org/wiki/Sobel_operator
 * 
 * @param frame frame to be processed
 */
void edgeDetection(AVFrame* frame);

/**
 * Apply a cartoon-style filter to a frame. The Sobel operator is used
 * to obtain the edges, which are coloured in black. The threshold parameter
 * in the function prototype determines the minimum value of gradient that
 * classify a specific pixel as a contour pixel (lower values of threshold
 * correspond to thicker but more accurate edges). The number of colours in the frame
 * are then reducted based on the value of the discr parameter (discr = 1 outputs
 * an unchanged picture, discr = 255 outputs a picture with only one colour).
 *
 * @param frame frame to be processed
 * @param thershold gradient value used to classify edges
 * @param discr number that determines the intensity of color reduction.
 */
void applyCartoonify(AVFrame* frame, int threshold, int discr);

/**
 * Change value of saturation of a picture based on the 
 * specified amount (the higher the amount, the higher
 * the increase in saturation)
 *
 * @param frame frame to be processed
 * @param amount amount of saturation to add/subtract
 */
void changeSaturation(AVFrame* frame, double amount);

/**
 * Apply mean filter with specified size to a stripe and writes result to a buffer (buffer must have
 * size of the original image).
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
  uint8_t* in_buffer, uint8_t* out_buffer);

/**
 * Apply variable blur to a frame. The image is divided into
 * horizontal stripes and mean filters with different kernels 
 * is applied to each one of those.
 *
 * @param frame frame to be processed
 */
void applyVariableMeanFilter(AVFrame* frame);

/**
* Apply mean filter to a frame. The size of the kernel
* must be specified.

* @param frame frame to be processed
* @param size size of the square used for averaging
*/
void applyMeanFilter(AVFrame* frame, int size);

}  // namespace filters

#endif  // _FILTER_H_
