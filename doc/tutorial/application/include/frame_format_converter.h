#ifndef _FRAME_FORMAT_CONVERTER_H
#define _FRAME_FORMAT_CONVERTER_H

#include <embb/base/base.h>

struct AVFrame;
struct AVCodecContext;
struct SwsContext;
enum AVPixelFormat;

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

#endif  // _FRAME_FORMAT_CONVERTER_H
