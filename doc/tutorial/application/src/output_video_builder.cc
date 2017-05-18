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

#include "../include/output_video_builder.h"

#include <stdexcept>
#include <iostream>

#include "../include/ffmpeg.h"

OutputVideoBuilder::OutputVideoBuilder(char* name, AVCodecContext* inputCtx) :
  formatCtx(nullptr),
  codec(nullptr),
  codecCtx(nullptr),
  stream(nullptr) {
  init(name, inputCtx);
}

OutputVideoBuilder::~OutputVideoBuilder() {
  avcodec_close(codecCtx);
  avio_closep(&formatCtx->pb);
  avformat_free_context(formatCtx);
}

void OutputVideoBuilder::init(char* name, AVCodecContext* inputCtx) {
  if (avformat_alloc_output_context2(&formatCtx, nullptr, nullptr, name) < 0) {
    throw std::runtime_error(
      "Could not determine file extension from provided name.");
  }

  codec = avcodec_find_encoder(inputCtx->codec_id);
  if (codec == nullptr) {
    throw std::runtime_error("Could not find suitable encoder.");
  }

  codecCtx = avcodec_alloc_context3(codec);
  if (avcodec_copy_context(codecCtx, inputCtx) < 0) {
    throw std::runtime_error("Could not copy output codec context from input");
  }

  codecCtx->time_base = inputCtx->time_base;

  if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
    throw std::runtime_error("Could not open encoder.");
  }

  stream = avformat_new_stream(formatCtx, codec);
  if (stream == nullptr) {
    throw std::runtime_error("Could not allocate stream.");
  }

  stream->id = formatCtx->nb_streams - 1;
  stream->codec = codecCtx;
  stream->time_base = codecCtx->time_base;

  av_dump_format(formatCtx, 0, name, 1);
  if (formatCtx->oformat->flags & AVFMT_NOFILE) {
    throw std::runtime_error("Demuxer cannot use avio_open.");
  }
  if (avio_open(&formatCtx->pb, name, AVIO_FLAG_WRITE) < 0) {
    throw std::runtime_error("Could not open output file.");
  }

  if (avformat_write_header(formatCtx, nullptr) < 0) {
    throw std::runtime_error("Error occurred when opening output file.");
  }
}

void OutputVideoBuilder::writeFrame(AVFrame* frame) {
  AVPacket packet = { 0 };
  int success;
  av_init_packet(&packet);

  if (avcodec_encode_video2(codecCtx, &packet, frame, &success)) {
    throw std::runtime_error("Error while encoding frames");
  }

  if (success) {
    av_packet_rescale_ts(&packet, codecCtx->time_base, stream->time_base);
    packet.stream_index = stream->index;
#ifdef LOG_MODE
    logMsg(&packet, &stream->time_base);
#endif
    av_interleaved_write_frame(formatCtx, &packet);
  }
  av_packet_unref(&packet);
}

void OutputVideoBuilder::writeVideo() {
  av_write_trailer(formatCtx);
}

void OutputVideoBuilder::logMsg(AVPacket* packet, AVRational* tb) {
  std::cout << "Writing frame at time: " <<
    av_q2d(*tb)*packet->pts << std::endl;
}

void OutputVideoBuilder::setMaxQB(int maxqb) {
  codecCtx->qmax = maxqb;
}
