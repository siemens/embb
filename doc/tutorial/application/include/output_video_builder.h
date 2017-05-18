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

#ifndef OUTPUT_VIDEO_BUILDER_H_
#define OUTPUT_VIDEO_BUILDER_H_

struct AVCodecContext;
struct AVFormatContext;
struct AVPacket;
struct AVRational;
struct AVCodec;
struct AVStream;
struct AVFrame;

// #define LOG_MODE

/**
 * This class takes care of the output file (opening and closing
 * the file, extracting frames, ...). The builder cannot be created
 * without an input file (whose name is provided at construction
 * time). If during construction problems occur (file not found,
 * file cannot be opened, ...) an exception is thrown.
 *
 */
class OutputVideoBuilder {
 public:
  OutputVideoBuilder(char* name, AVCodecContext* inputCtx);
  ~OutputVideoBuilder();

  /**
   * Write one frame to video file.
   *
   * @param frame to write to output.
   */
  void writeFrame(AVFrame* frame);

  /**
   * Finalize the output file.
   */
  void writeVideo();

  /**
   * Change quantization parameter. Low values tend to increase quality
   * by increasing the size of the output (low compression), high values
   * decrease quality while decreasing file size (high compression).
   *
   * @param maxqb new value of maximum quantization parameter
   */
  void setMaxQB(int maxqb);

 private:
  OutputVideoBuilder();
  void init(char* name, AVCodecContext* inputCtx);
  void logMsg(AVPacket* packet, AVRational* tb);
  AVFormatContext* formatCtx;
  AVCodec* codec;
  AVCodecContext* codecCtx;
  AVStream* stream;
};

#endif // OUTPUT_VIDEO_BUILDER_H_
