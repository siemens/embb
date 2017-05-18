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

#ifndef INPUT_VIDEO_HANDLER_H_
#define INPUT_VIDEO_HANDLER_H_

struct AVCodecContext;
struct AVFormatContext;
struct AVPacket;
struct AVCodec;
struct AVFrame;

/**
 * This class takes care of the input file (opening and closing
 * the file, extracting frames, ...). The handler cannot be created
 * without an input file (whose name is provided at construction
 * time). If during construction problems occur (file not found,
 * file cannot be opened, ...) an exception is thrown.
 *
 */
class InputVideoHandler {
 public:
  explicit InputVideoHandler(char* name);
  ~InputVideoHandler();

  /**
   * Returns the pointer to AVCodecContext that is set at construction.
   */
  AVCodecContext* getCodecContext();

  /**
   * Reads next (or first) frame from the input file. Note that the
   * frame must be allocated before being passed to the function
   * using av_frame_alloc().
   *
   * @param frame pointer to frame that is extracted from input file
   * @param success integer code that tells us if a complete 
            frame was extracted. success == 1 if the frame is valid
   * @return true there are still frames to process, false otherwise
   */
  bool readFrame(AVFrame* frame, int* success);

 private:
  InputVideoHandler();
  void init(char* name);

  AVFormatContext* formatCtx_;
  AVCodec* codec_;
  AVCodecContext* codecCtx_;
  AVPacket* packet_;
  int streamIndex_;
};

#endif // INPUT_VIDEO_HANDLER_H_

