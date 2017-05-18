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

#ifndef FRAME_FORMAT_CONVERTER_H_
#define FRAME_FORMAT_CONVERTER_H_

#include <embb/base/base.h>

#include "./ffmpeg.h"

enum ConversionType {
  TO_RGB,
  TO_ORIGINAL
};

/**
 * This class is used for converting frames from YUV format
 * to RGB format and vice versa. It is not strictly necessary to
 * change the format of the frames before processing. However,
 * the effect of the filters is clearer when they work with
 * pictures in RGB format.
 */
class FrameFormatConverter {
 public:
  FrameFormatConverter();
  ~FrameFormatConverter();

  /**
   * Extract format information from an AVCodecContext.
   *
   * @param codecCtx context to extract the format information from
   */
  void getFormatInfo(AVCodecContext* codecCtx);

  /**
   * Convert format of input frame to output format.
   *
   * @param input input frame
   * @param outout output frame
   * @param ct conversion type: either TO_RGB or TO_ORIGINAL
   */
  void convertFormat(AVFrame** input, AVFrame** output, ConversionType ct);

 private:
  SwsContext* toRGBCtx;
  SwsContext* toOriginalFormatCtx;
  AVPixelFormat originalFormat;
  embb::base::Mutex mutex;
};

#endif // FRAME_FORMAT_CONVERTER_H_
