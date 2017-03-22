#include "output_video_builder.h"

OutputVideoBuilder::OutputVideoBuilder(char* name, AVCodecContext* inputCtx) :
  formatCtx(nullptr),
  codec(nullptr),
  codecCtx(nullptr),
  stream(nullptr){
  init(name,inputCtx);
}

OutputVideoBuilder::~OutputVideoBuilder() {
  avcodec_close(codecCtx);
  avio_closep(&formatCtx->pb);
  avformat_free_context(formatCtx);
}

void OutputVideoBuilder::init(char* name, AVCodecContext* inputCtx) {
  if (avformat_alloc_output_context2(&formatCtx, NULL, NULL, name) < 0)
    throw std::exception("Could not determine file extension from provided name.");

  codec = avcodec_find_encoder(inputCtx->codec_id);
  if (codec == nullptr) {
    throw std::exception("Could not find suitable encoder.");
  }

  codecCtx = avcodec_alloc_context3(codec);
  if (avcodec_copy_context(codecCtx, inputCtx) < 0)
    throw std::exception("Could not copy output codec context from input");

  codecCtx->time_base = inputCtx->time_base;

  if (avcodec_open2(codecCtx, codec, NULL) < 0)
    throw std::exception("Could not open encoder.");

  stream = avformat_new_stream(formatCtx, codec);
  if (stream == nullptr) {
    throw std::exception("Could not allocate stream.");
  }

  // not sure why this is needed
  stream->id = formatCtx->nb_streams - 1;
  stream->codec = codecCtx;
  stream->time_base = codecCtx->time_base;

  // not sure what this does
  av_dump_format(formatCtx, 0, name, 1);
  if (!(formatCtx->oformat->flags & AVFMT_NOFILE)) {
    if (avio_open(&formatCtx->pb, name, AVIO_FLAG_WRITE) < 0) {
      throw std::exception("Could not open output file.");
    }
  }

  if (avformat_write_header(formatCtx, NULL) < 0) {
    throw std::exception("Error occurred when opening output file.");
  }

}

void OutputVideoBuilder::writeFrame(AVFrame* frame) {
  AVPacket packet = { 0 };
  int success;
  av_init_packet(&packet);

  if (avcodec_encode_video2(codecCtx, &packet, frame, &success))
    throw std::exception("Error encoding frames");

  if (success) {
    av_packet_rescale_ts(&packet, codecCtx->time_base, stream->time_base);
    packet.stream_index = stream->index;
#ifdef LOG_MODE
    logMsg(&packet,&stream->time_base);
#endif
    av_interleaved_write_frame(formatCtx, &packet);
  }
  av_free_packet(&packet);
}

void OutputVideoBuilder::writeVideo() {
  av_write_trailer(formatCtx);
}

void OutputVideoBuilder::logMsg(AVPacket* packet, AVRational* tb) {
  std::cout << "Writing frame at time: " << av_q2d(*tb)*packet->pts << std::endl;
}

void OutputVideoBuilder::setMaxQB(int maxqb) {
  codecCtx->qmax = maxqb;
  // Probably this is the best way to fix quality of result, but syntax is wrong
  // av_opt_set(codecCtx->priv_data, "cfr", "18", 0);
}
