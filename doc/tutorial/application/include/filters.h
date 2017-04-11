#ifndef _FILTER_H_
#define _FILTER_H_

struct AVFrame;

namespace filters {

/**
 * Convert the frame to black and white by setting
 * the rgb values to the average of the three.
 *
 * @param frame frame to be processed
 */
void applyBlackAndWhite(AVFrame* frame);

/**
 * Convert the frame to black and white by setting
 * the rgb values to the average of the three.
 *
 * @param frame frame to be processed
 */
void applyBlackAndWhiteParallel(AVFrame* frame);

/**
 * Apply the Sobel operator to a frame to detect edges.
 * see https://en.wikipedia.org/wiki/Sobel_operator
 *
 * @param frame frame to be processed
 */
void edgeDetection(AVFrame* frame);

/**
 * Apply the Sobel operator to a frame to detect edges.
 * see https://en.wikipedia.org/wiki/Sobel_operator
 *
 * @param frame frame to be processed
 */
void edgeDetectionParallel(AVFrame* frame);

/**
 * Apply a cartoon-style filter to a frame. The Sobel operator is used
 * to obtain the edges, which are coloured in black. The threshold parameter
 * in the function prototype determines the minimum value of gradient that
 * classify a specific pixel as a contour pixel (lower values of threshold
 * correspond to thicker but more accurate edges). The number of colours in the
 * frame are then reducted based on the value of the discr parameter (discr = 1
 * outputs an unchanged picture, discr = 255 outputs a picture with only one
 * colour).
 *
 * @param frame frame to be processed
 * @param thershold gradient value used to classify edges
 * @param discr number that determines the intensity of color reduction.
 */
void applyCartoonify(AVFrame* frame, int threshold, int discr);

/**
 * Apply a cartoon-style filter to a frame. The Sobel operator is used
 * to obtain the edges, which are coloured in black. The threshold parameter
 * in the function prototype determines the minimum value of gradient that
 * classify a specific pixel as a contour pixel (lower values of threshold
 * correspond to thicker but more accurate edges). The number of colours in the
 * frame are then reducted based on the value of the discr parameter (discr = 1
 * outputs an unchanged picture, discr = 255 outputs a picture with only one
 * colour).
 *
 * @param frame frame to be processed
 * @param thershold gradient value used to classify edges
 * @param discr number that determines the intensity of color reduction.
 */
void applyCartoonifyParallel(AVFrame* frame, int threshold, int discr);

extern char const * cartoonify_kernel;

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
 * Change value of saturation of a picture based on the 
 * specified amount (the higher the amount, the higher
 * the increase in saturation)
 *
 * @param frame frame to be processed
 * @param amount amount of saturation to add/subtract
 */
void changeSaturationParallel(AVFrame* frame, double amount);

/**
 * Apply mean filter to a frame. The size of the kernel
 * must be specified.
 *
 * @param frame frame to be processed
 * @param size size of the square used for averaging
 */
void applyMeanFilter(AVFrame* frame, int size);

/**
 * Apply mean filter to a frame. The size of the kernel
 * must be specified.
 *
 * @param frame frame to be processed
 * @param size size of the square used for averaging
 */
void applyMeanFilterParallel(AVFrame* frame, int size);

extern char const * mean_kernel;

}  // namespace filters

#endif  // _FILTER_H_
