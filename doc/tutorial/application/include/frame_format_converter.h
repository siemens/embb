#ifndef _FRAME_FORMAT_CONVERTER_H
#define _FRAME_FORMAT_CONVERTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

#include "embb/base/base.h"

typedef enum ConvertionType {TO_RGB, TO_ORIGINAL};

/**
 * This class is used for converting Frames from YUV format
 * to RGB format at viceversa. It is not strictly necessary to 
 * change the format of frames before processing. However, 
 * the effect of filters is probably clearer when they work with 
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
   * @param ct convertion type: either TO_RGB or TO_ORIGINAL
   */
  void convertFormat(AVFrame** input, AVFrame** output, ConvertionType ct);

private:
  struct SwsContext* toRGBCtx;
  struct SwsContext* toOriginalFormatCtx;
  enum AVPixelFormat originalFormat;
  embb::base::Mutex mutex;

};

#endif  // _FRAME_FORMAT_CONVERTER_H

