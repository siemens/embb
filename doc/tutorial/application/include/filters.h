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

#ifndef FILTERS_H_
#define FILTERS_H_

struct AVFrame;

namespace filters {

/**
 * Convert the frame to black and white by setting
 * the RGB values to the average of the three.
 *
 * @param frame frame to be processed
 */
void applyBlackAndWhite(AVFrame* frame);

/**
 * Convert the frame to black and white by setting
 * the RGB values to the average of the three.
 *
 * @param frame frame to be processed
 */
void applyBlackAndWhiteParallel(AVFrame* frame);

/**
 * Apply the Sobel operator to a frame to detect edges.
 * See https://en.wikipedia.org/wiki/Sobel_operator
 *
 * @param frame frame to be processed
 */
void edgeDetection(AVFrame* frame);

/**
 * Apply the Sobel operator to a frame to detect edges.
 * See https://en.wikipedia.org/wiki/Sobel_operator
 *
 * @param frame frame to be processed
 */
void edgeDetectionParallel(AVFrame* frame);

/**
 * Apply a cartoon-style filter to a frame. The Sobel operator is used
 * to obtain the edges, which are colored in black. The threshold parameter
 * in the function prototype determines the minimum value of the gradient that
 * classifies a specific pixel as a contour pixel (lower values of the threshold
 * correspond to thicker but more accurate edges). The number of colors in the
 * frame are then reduced based on the value of the discr parameter (discr = 1
 * outputs an unchanged picture, discr = 255 outputs a picture with only one
 * color).
 *
 * @param frame frame to be processed
 * @param thershold gradient value used to classify edges
 * @param discr number that determines the intensity of color reduction
 */
void applyCartoonify(AVFrame* frame, int threshold, int discr);

/**
 * Apply a cartoon-style filter to a frame. The Sobel operator is used
 * to obtain the edges, which are colored in black. The threshold parameter
 * in the function prototype determines the minimum value of the gradient that
 * classifies a specific pixel as a contour pixel (lower values of the threshold
 * correspond to thicker but more accurate edges). The number of colors in the
 * frame are then reduced based on the value of the discr parameter (discr = 1
 * outputs an unchanged picture, discr = 255 outputs a picture with only one
 * color).
 *
 * @param frame frame to be processed
 * @param thershold gradient value used to classify edges
 * @param discr number that determines the intensity of color reduction
 */
void applyCartoonifyParallel(AVFrame* frame, int threshold, int discr);

extern char const * cartoonify_kernel;

/**
 * Change value of the saturation of a picture based on the 
 * specified amount (the higher the amount, the higher
 * the increase in saturation).
 *
 * @param frame frame to be processed
 * @param amount amount of saturation to add/subtract
 */
void changeSaturation(AVFrame* frame, double amount);

/**
 * Change value of the saturation of a picture based on the 
 * specified amount (the higher the amount, the higher
 * the increase in saturation).
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

#endif // FILTERS_H_
