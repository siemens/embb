#include <iostream>
#include <exception>
#include <ctime>

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

void terminate(char const * message, int code) {
  std::cout << message << std::endl;
  exit(code);
}

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
  while (!success && ret) {
    // ret != 1 if there are no more frames to process
    ret = inputHandler->readFrame(frame, &success);
  }
  // if frame is not ready just send a nullptr frame
  if (!success) {
    frame = nullptr;
  }
  return ret != 0;
}

void writeToFile(AVFrame* const &frame) {
  if (frame != nullptr) {
    try {
      outputBuilder->writeFrame(frame);
    } catch (std::exception& e) {
      terminate(e.what(), 10);
    }
    AVFrame* copy = frame;
    //av_frame_unref(copy);
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

void process_parallel() {
  Network nw(8);

  Network::Source<AVFrame*> read(nw, embb::base::MakeFunction(readFromFile));

  Network::ParallelProcess<Network::Inputs<AVFrame*>,
    Network::Outputs<AVFrame*> >
      rgb(nw, embb::base::MakeFunction(convertToRGB));

  Network::ParallelProcess<Network::Inputs<AVFrame*>,
    Network::Outputs<AVFrame*> >
      original(nw, embb::base::MakeFunction(convertToOriginal));

  Network::ParallelProcess<Network::Inputs<AVFrame*>,
    Network::Outputs<AVFrame*> >
      filter(nw, embb::base::MakeFunction(applyFilter));

  Network::Sink<AVFrame*> write(nw, embb::base::MakeFunction(writeToFile));

  read >> rgb;
  rgb >> filter;
  filter >> original;
  original >> write;

  nw();
}

void process_serial() {
  AVFrame* frame;
  AVFrame* convertedFrame;
  int gotFrame = 0;

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
      } catch (std::exception & e) {
        terminate(e.what(), 20);
      }
    }
  }
  av_frame_free(&frame);
  av_frame_free(&convertedFrame);
}

int parallel = 1;

bool check_arguments(int argc, char * argv[]) {
  bool result = true;

  std::cout << std::endl << "Video processing application" <<
    std::endl << std::endl;

  if (argc >= 3 && argc <= 4) {
    if (argc == 4) {
      try {
        parallel = std::stoi(argv[3]);
      } catch (std::exception &) {
        result = false;
      }
    }
  } else {
    result = false;
  }

  if (!result) {
    std::cout << "usage: video_app <input> <output> [parallel]" << std::endl;
    std::cout << "  <input>     source video file name" << std::endl;
    std::cout << "  <output>    output video file name" << std::endl;
    std::cout << "  [parallel]  process in parallel (!=0, default)"
      " or serially (0), optional" << std::endl << std::endl;
  }

  return result;
}

int main(int argc, char *argv[]) {

  // silence warnings from ffmpeg
  av_log_set_level(AV_LOG_QUIET);

  if (!check_arguments(argc, argv)) {
    return 30;
  }

  // initialize ffmpeg libraries
  av_register_all();

  // open input video file
  try {
    inputHandler = new InputVideoHandler(argv[1]);
  } catch (std::exception& e) {
    terminate(e.what(), 31);
  }

  // open output video file
  try {
    outputBuilder = new OutputVideoBuilder(argv[2],
      inputHandler->getCodecContext());
  } catch (std::exception& e) {
    terminate(e.what(), 32);
  }

  converter.getFormatInfo(inputHandler->getCodecContext());

  // change this value to determine output quality
  outputBuilder->setMaxQB(7);

  std::string mode = parallel ? "enabled" : "disabled";
  std::cout << "Reading and processing video: parallel mode " <<
    mode << std::endl;
  clock_t start = clock();

  if (parallel) {
    process_parallel();
  } else {
    process_serial();
  }

  outputBuilder->writeVideo();
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  std::cout << "Elapsed time = " << seconds << " s" << std::endl;

  delete inputHandler;
  delete outputBuilder;

  return 0;
}
