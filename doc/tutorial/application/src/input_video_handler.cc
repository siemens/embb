#include "input_video_handler.h"

InputVideoHandler::InputVideoHandler(char* name) :
  formatCtx(nullptr),
  codec(nullptr),
  codecCtx(nullptr){
  init(name);
}

InputVideoHandler::~InputVideoHandler(){
  avcodec_close(codecCtx);
  avformat_close_input(&formatCtx);
}

void InputVideoHandler::init(char* name) {
  streamIndex = -1;
  int numStreams;

  if (avformat_open_input(&formatCtx, name, NULL, NULL) != 0)
    throw std::exception("Invalid input file name.");

  if (avformat_find_stream_info(formatCtx, NULL)<0)
    throw std::exception("Could not find stream information.");

  numStreams = formatCtx->nb_streams;

  if (numStreams < 0)
    throw std::exception("No streams in input video file.");

  for (int i = 0; i < numStreams; i++) {
    if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      streamIndex = i;
      break;
    }
  }

  if (streamIndex < 0)
    throw std::exception("No video stream in input video file.");

  // find decoder using id
  codec = avcodec_find_decoder(formatCtx->streams[streamIndex]->codec->codec_id);
  if (codec == nullptr)
    throw std::exception("Could not find suitable decoder for input file.");

  // copy context from input stream
  codecCtx = avcodec_alloc_context3(codec);
  if (avcodec_copy_context(codecCtx, formatCtx->streams[streamIndex]->codec) != 0)
    throw std::exception("Could not copy codec context from input stream.");

  if (avcodec_open2(codecCtx, codec, NULL) < 0)
    throw std::exception("Could not open decoder.");

  // by setting this option we ensure that the reference to each frame
  // is kept across multiple calls to read_frame function. This is 
  // needed if reading and writing are done by different processes.
  codecCtx->refcounted_frames = 1;
}

AVCodecContext* InputVideoHandler::getCodecContext() {
  return codecCtx;
}

bool InputVideoHandler::readFrame(AVFrame* frame, int* success) {
  *success = 0;
  if (av_read_frame(formatCtx, &packet) < 0)
    return false;
  if (packet.stream_index == streamIndex) {
    avcodec_decode_video2(codecCtx, frame, success, &packet);
  }
  av_free_packet(&packet);
  return true;
}
