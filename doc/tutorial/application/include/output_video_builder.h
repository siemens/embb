#ifndef _OUTPUT_VIDEO_BUILDER_H_
#define _OUTPUT_VIDEO_BUILDER_H_

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

#endif  // _OUTPUT_VIDEO_BUILDER_H_
