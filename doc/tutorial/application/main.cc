#include <iostream>
#include <exception>
#include <time.h>

#include "input_video_handler.h"
#include "output_video_builder.h"
#include "frame_format_converter.h"
#include "ffmpeg.h"
#include "filters.h"

#include <embb/dataflow/dataflow.h>
#include <embb/base/base.h>

typedef embb::dataflow::Network Network;

// maybe not optimal way of declaring inputHandler and outputBuilder
static InputVideoHandler* inputHandler = nullptr;
static OutputVideoBuilder* outputBuilder = nullptr;
static FrameFormatConverter converter;

#define TERMINATE() std::cout << e.what() << std::endl; exit(1)

void filter(AVFrame* frame) {
  // apply filters to the frame. Here are some examples:
  // filters::applyBlackAndWhite(frame);
  // filters::applyVariableMeanFilter(frame);
  // filters::changeSaturation(frame, 0.2);
  // filters::applyShuffleColors(frame);
  // filters::applyNegative(frame);
  filters::applyMeanFilter(frame, 3);
  filters::applyCartoonify(frame, 90, 40);
}

bool readFromFile(AVFrame* &frame) {
  int success = 0;
  int ret = 1;
  frame = av_frame_alloc();
  while (!success && ret)
    // ret != 1 if there are no more frames to process
    ret = inputHandler->readFrame(frame, &success);
  // if frame is not ready just send a nullptr frame
  if (!success)
    frame = nullptr;
  return ret;
}

void writeToFile(AVFrame* const &frame) {
  if (frame != nullptr) {
    try {
      outputBuilder->writeFrame(frame);
    } catch (std::exception& e) {
      TERMINATE();
    }
    AVFrame* copy = frame;
    av_frame_unref(copy);
    av_frame_free(&copy);
  }
}

void applyFilter(AVFrame* const &input_frame, AVFrame* &output_frame) {
  if (input_frame == nullptr) {
    output_frame = nullptr;
    return;
  }
  output_frame = input_frame;
  filter(output_frame);

}

void convertToRGB(AVFrame* const &input_frame, AVFrame* &output_frame) {
  if (input_frame == nullptr) {
    output_frame = nullptr;
    return;
  }
  AVFrame* input = input_frame;
  output_frame = av_frame_alloc();
  converter.convertFormat(&input, &output_frame, TO_RGB);
  av_frame_free(&input);
}

void convertToOriginal(AVFrame* const &input_frame, AVFrame* &output_frame) {
  if (input_frame == nullptr) {
    output_frame = nullptr;
    return;
  }
  AVFrame* input = input_frame;
  output_frame = av_frame_alloc();
  converter.convertFormat(&input, &output_frame, TO_ORIGINAL);
  av_frame_free(&input);
}

int main(int argc, char *argv[]) {
  // silent warnings from ffmpeg
  av_log_set_level(AV_LOG_QUIET);

  AVFrame* frame;
  AVFrame* convertedFrame;
  int gotFrame = 0;
  int parallel = 1;

  if (argc < 3) {
    std::cout << "Video processing test application" << std::endl;
    std::cout << "Given an input video file, the program applies some simple filter to each frame ";
    std::cout << "and creates a new output video file." << std::endl << std::endl;
    std::cout << "Please provide an input video file as first argument and the name of the output file ";
    std::cout << "as second argument" << std::endl << std::endl;
    std::cout << "Use the third optional parameter to decide if processing in parallel (1)" << std::endl;
    std::cout << "or not (!1) (by default, parallel is enabled)" << std::endl;
    return -1;
  }

  if (argc == 4) {
    parallel = atoi(argv[3]);
  }

  // initialize ffmpeg libraries
  av_register_all();

  // open input video file
  try {
    inputHandler = new InputVideoHandler(argv[1]);
  }
  catch (std::exception& e) {
    TERMINATE();
  }

  // open output video file
  try {
    outputBuilder = new OutputVideoBuilder(argv[2], inputHandler->getCodecContext());
  }
  catch (std::exception& e) {
    TERMINATE();
  }

  converter.getFormatInfo(inputHandler->getCodecContext());

  // change this value to determine output quality
  outputBuilder->setMaxQB(7);

  std::string mode = parallel ? "enabled" : "disabled";
  std::cout << "Start reading and processing video: parallel mode " << mode <<std::endl;
  clock_t start = clock();

  if (parallel){
    Network nw(8);

    Network::Source<AVFrame*> read(nw, embb::base::MakeFunction(&readFromFile));

    Network::ParallelProcess<Network::Inputs<AVFrame*>, Network::Outputs<AVFrame*>>
      convertToRGB(nw, embb::base::MakeFunction(&convertToRGB));

    Network::ParallelProcess<Network::Inputs<AVFrame*>, Network::Outputs<AVFrame*>>
      convertToOriginal(nw, embb::base::MakeFunction(&convertToOriginal));

    Network::ParallelProcess<Network::Inputs<AVFrame*>, Network::Outputs<AVFrame*>>
      applyFilter(nw, embb::base::MakeFunction(&applyFilter));

    Network::Sink<AVFrame*> write(nw, embb::base::MakeFunction(&writeToFile));

    read.GetOutput<0>() >> convertToRGB.GetInput<0>();
    convertToRGB.GetOutput<0>() >> applyFilter.GetInput<0>();
    applyFilter.GetOutput<0>() >> convertToOriginal.GetInput<0>();
    convertToOriginal.GetOutput<0>() >> write.GetInput<0>();

    nw();
  } else {
    frame = av_frame_alloc();
    convertedFrame = av_frame_alloc();
    while (inputHandler->readFrame(frame, &gotFrame)) {

      if (gotFrame) {
        converter.convertFormat(&frame, &convertedFrame, TO_RGB);
        filter(convertedFrame);
        av_frame_unref(frame);
        av_frame_free(&frame);
        frame = av_frame_alloc();
        converter.convertFormat(&convertedFrame, &frame, TO_ORIGINAL);
        try {
          outputBuilder->writeFrame(frame);
        }
        catch (std::exception& e) {
          TERMINATE();
        }
      }
    }
    av_frame_free(&frame);
    av_frame_free(&convertedFrame);
  }

  outputBuilder->writeVideo();
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  std::cout << "Elapsed time = " << seconds << " s" << std::endl;

  delete inputHandler;
  delete outputBuilder;
  return 0;
}
