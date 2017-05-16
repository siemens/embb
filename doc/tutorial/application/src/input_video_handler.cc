#include "input_video_handler.h"

#include <exception>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif

InputVideoHandler::InputVideoHandler(char* name)
  : formatCtx_(nullptr)
  , codec_(nullptr)
  , codecCtx_(nullptr)
  , packet_(nullptr)
  , streamIndex_(-1) {
  init(name);
}

InputVideoHandler::~InputVideoHandler() {
  avcodec_close(codecCtx_);
  avformat_close_input(&formatCtx_);
  delete packet_;
}

void InputVideoHandler::init(char* name) {
  packet_ = new AVPacket;

  streamIndex_ = -1;
  int numStreams;

  if (avformat_open_input(&formatCtx_, name, nullptr, nullptr) != 0) {
    throw std::runtime_error("Invalid input file name.");
  }

  if (avformat_find_stream_info(formatCtx_, nullptr) < 0) {
    throw std::runtime_error("Could not find stream information.");
  }

  numStreams = formatCtx_->nb_streams;

  if (numStreams < 0) {
    throw std::runtime_error("No streams in input video file.");
  }

  for (int i = 0; i < numStreams; i++) {
    if (formatCtx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      streamIndex_ = i;
      break;
    }
  }

  if (streamIndex_ < 0) {
    throw std::runtime_error("No video stream in input video file.");
  }

  // Find decoder using id
  codec_ =
    avcodec_find_decoder(formatCtx_->streams[streamIndex_]->codec->codec_id);
  if (codec_ == nullptr) {
    throw std::runtime_error(
      "Could not find suitable decoder for input file.");
  }

  // Copy context from input stream
  codecCtx_ = avcodec_alloc_context3(codec_);
  if (avcodec_copy_context(
    codecCtx_, formatCtx_->streams[streamIndex_]->codec) != 0) {
    throw std::runtime_error(
      "Could not copy codec context from input stream.");
  }

  if (avcodec_open2(codecCtx_, codec_, nullptr) < 0) {
    throw std::runtime_error("Could not open decoder.");
  }

  // By setting this option, we ensure that the reference to each frame
  // is kept across multiple calls to the read_frame function. This is
  // needed if reading and writing are done by different processes.
  codecCtx_->refcounted_frames = 1;
}

AVCodecContext* InputVideoHandler::getCodecContext() {
  return codecCtx_;
}

bool InputVideoHandler::readFrame(AVFrame* frame, int* success) {
  *success = 0;
  if (av_read_frame(formatCtx_, packet_) < 0) {
    return false;
  }
  if (packet_->stream_index == streamIndex_) {
    avcodec_decode_video2(codecCtx_, frame, success, packet_);
  }
  av_free_packet(packet_);
  return true;
}
