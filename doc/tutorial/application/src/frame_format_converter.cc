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

#include "../include/frame_format_converter.h"

FrameFormatConverter::FrameFormatConverter()
  : toRGBCtx(nullptr)
  , toOriginalFormatCtx(nullptr) {
  // empty
}

FrameFormatConverter::~FrameFormatConverter() {
  // empty
}

void FrameFormatConverter::getFormatInfo(AVCodecContext* codecCtx) {
  originalFormat = codecCtx->pix_fmt;
  toRGBCtx = sws_getContext(
    codecCtx->width, codecCtx->height, originalFormat,
    codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB24,
    0, 0, 0, 0);
  toOriginalFormatCtx = sws_getContext(
    codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB24,
    codecCtx->width, codecCtx->height, originalFormat,
    0, 0, 0, 0);
}

void FrameFormatConverter::convertFormat(
  AVFrame** input,
  AVFrame** output,
  ConversionType ct) {
  embb::base::LockGuard<embb::base::Mutex> guard(mutex);
  AVFrame* convertedFrame = *output;
  struct SwsContext* ctx =
    (ct == TO_RGB ? toRGBCtx : toOriginalFormatCtx);
  enum AVPixelFormat format =
    (ct == TO_RGB ? AV_PIX_FMT_RGB24 : originalFormat);

  int height = (*input)->height;
  int width = (*input)->width;
  int numBytes = avpicture_get_size(format, width, height);
  uint8_t *buffer = static_cast<uint8_t *>(av_malloc(numBytes*sizeof(uint8_t)));

  // Associate buffer with output frame
  avpicture_fill(reinterpret_cast<AVPicture *>(convertedFrame),
    buffer, format, width, height);

  sws_scale(ctx, static_cast<uint8_t const * const *>((*input)->data),
    (*input)->linesize, 0, height,
    convertedFrame->data, convertedFrame->linesize);

  // These are not set automatically
  convertedFrame->width = width;
  convertedFrame->height = height;
  convertedFrame->format = format;
}
