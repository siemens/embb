#include "frame_format_converter.h"



FrameFormatConverter::FrameFormatConverter() :
  toRGBCtx(nullptr),
  toOriginalFormatCtx(nullptr){
}


FrameFormatConverter::~FrameFormatConverter() {

}

void FrameFormatConverter::getFormatInfo(AVCodecContext* codecCtx) {
  originalFormat = codecCtx->pix_fmt;
  toRGBCtx = sws_getContext(codecCtx->width, codecCtx->height, originalFormat,
                            codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB24,
                            0, 0, 0, 0);
  toOriginalFormatCtx = sws_getContext(codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB24,
                                       codecCtx->width, codecCtx->height, originalFormat,
                                       0, 0, 0, 0);
}

void FrameFormatConverter::convertFormat(AVFrame** input, AVFrame** output, ConvertionType ct) {
  embb::base::LockGuard<embb::base::Mutex> guard(mutex);
  AVFrame* convertedFrame = *output;
  struct SwsContext* ctx = (ct == TO_RGB ? toRGBCtx : toOriginalFormatCtx);
  enum AVPixelFormat format = (ct == TO_RGB ? AV_PIX_FMT_RGB24 : originalFormat);

  int height = (*input)->height;
  int width = (*input)->width;
  int numBytes = avpicture_get_size(format, width, height);
  uint8_t *buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  // associate buffer with output frame
  avpicture_fill((AVPicture *)convertedFrame, buffer, format, width, height);

  sws_scale(ctx, (uint8_t const * const *)(*input)->data, (*input)->linesize, 0, height,
    convertedFrame->data, convertedFrame->linesize);

  // These are not set automatically
  convertedFrame->width = width;
  convertedFrame->height = height;
  convertedFrame->format = format;
}
