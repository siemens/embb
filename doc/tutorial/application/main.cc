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

#include <iostream>
#include <exception>
#include <chrono>

#include "include/input_video_handler.h"
#include "include/output_video_builder.h"
#include "include/frame_format_converter.h"
#include "include/filters.h"
#include "include/ffmpeg.h"

#include <embb/base/base.h>
#include <embb/mtapi/c/mtapi_opencl.h>
#include <embb/dataflow/dataflow.h>

typedef embb::dataflow::Network Network;

static InputVideoHandler* inputHandler = nullptr;
static OutputVideoBuilder* outputBuilder = nullptr;
static FrameFormatConverter converter;

void terminate(char const * message, int code) {
  std::cout << message << std::endl;
  exit(code);
}

void filter(AVFrame* frame) {
  // Apply filters to the frame. Here are some examples:
  // filters::applyBlackAndWhite(frame);
  // filters::edgeDetection(frame);
  // filters::changeSaturation(frame, 0.2);
  filters::applyMeanFilter(frame, 3);
  filters::applyCartoonify(frame, 90, 40);
}

void filter_parallel(AVFrame* frame) {
  // Apply filters to the frame. Here are some examples:
  // filters::applyBlackAndWhiteParallel(frame);
  // filters::edgeDetectionParallel(frame);
  // filters::changeSaturationParallel(frame, 0.2);
  filters::applyMeanFilterParallel(frame, 3);
  filters::applyCartoonifyParallel(frame, 90, 40);
}

#define JOB_MEAN 10
#define JOB_CARTOONIFY 11

void filter_opencl(AVFrame* frame) {
  int const width = frame->width;
  int const height = frame->height;

  int n_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);

  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Job job;
  embb::mtapi::Task task;
  int size;
  unsigned char * args;
  int * param;
  unsigned char * data;
  unsigned char * res;

// snippet_begin:call_opencl_action
  args = new unsigned char[n_bytes + sizeof(int) * 4];
  res = new unsigned char[n_bytes + sizeof(int) * 4];

  size = n_bytes + sizeof(int) * 3;
  param = reinterpret_cast<int*>(args);
  data = args + sizeof(int) * 3;
  param[0] = width;
  param[1] = height;
  param[2] = 3;
  memcpy(data, frame->data[0], n_bytes);
  job = node.GetJob(JOB_MEAN);
  task = node.Start(MTAPI_TASK_ID_NONE, job.GetInternal(), args, size,
    res + sizeof(int)*4, n_bytes, MTAPI_DEFAULT_TASK_ATTRIBUTES);
  task.Wait();
// snippet_end

  delete[] args;
  args = res;
  res = frame->data[0];

  size = n_bytes + sizeof(int) * 4;
  param = reinterpret_cast<int*>(args);
  data = args + sizeof(int) * 4;
  param[0] = width;
  param[1] = height;
  param[2] = 90;
  param[3] = 40;
  job = node.GetJob(JOB_CARTOONIFY);
  task = node.Start(MTAPI_TASK_ID_NONE, job.GetInternal(), args, size,
    res, n_bytes, MTAPI_DEFAULT_TASK_ATTRIBUTES);
  task.Wait();

  delete[] args;
}

bool readFromFile(AVFrame* &frame) {
  int success = 0;
  int ret = 1;
  frame = av_frame_alloc();
  while (!success && ret) {
    // ret != 1 if there are no more frames to process
    ret = inputHandler->readFrame(frame, &success);
  }
  // If frame is not ready, just send a nullptr frame
  if (!success) {
    av_frame_free(&frame);
    frame = nullptr;
  }
  return ret != 0;
}

void writeToFile(AVFrame* const &frame) {
  static int framecnt = 0;
  if (frame != nullptr) {
    try {
      outputBuilder->writeFrame(frame);
    } catch (std::exception& e) {
      terminate(e.what(), 10);
    }
    AVFrame* copy = frame;
    av_free(copy->data[0]);
    av_frame_free(&copy);
    framecnt++;
    std::cout << "frame " << framecnt << "\r";
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

void applyFilterParallel(AVFrame* const &input_frame, AVFrame* &output_frame) {
  if (input_frame == nullptr) {
    output_frame = nullptr;
    return;
  }
  output_frame = input_frame;
  filter_parallel(output_frame);
}

void applyFilterOpenCL(AVFrame* const &input_frame, AVFrame* &output_frame) {
  if (input_frame == nullptr) {
    output_frame = nullptr;
    return;
  }
  output_frame = input_frame;
  filter_opencl(output_frame);
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
  av_free(input->data[0]);
  av_frame_free(&input);
}

void process_parallel_dataflow_and_opencl() {
  mtapi_status_t status;
  embb::mtapi::Node::Initialize(1, 1);
  mtapi_opencl_plugin_initialize(&status);
  if (status != MTAPI_SUCCESS) {
    std::cout << "OpenCL unavailable..." << std::endl;
    embb::mtapi::Node::Finalize();
    return;
  }

  int node_local = 1;
// snippet_begin:make_opencl_action
  mtapi_opencl_action_create(JOB_MEAN, filters::mean_kernel,
    "mean", 32, 3, &node_local, sizeof(int), &status);
  if (status != MTAPI_SUCCESS) {
    std::cout << "Could not create OpenCL action..." << std::endl;
    mtapi_opencl_plugin_finalize(MTAPI_NULL);
    embb::mtapi::Node::Finalize();
    return;
  }
// snippet_end

  mtapi_opencl_action_create(JOB_CARTOONIFY, filters::cartoonify_kernel,
    "cartoonify", 32, 3, &node_local, sizeof(int), &status);
  if (status != MTAPI_SUCCESS) {
    std::cout << "Could not create OpenCL action..." << std::endl;
    mtapi_opencl_plugin_finalize(MTAPI_NULL);
    embb::mtapi::Node::Finalize();
    return;
  }

  Network nw(8);

  Network::Source<AVFrame*> read(nw, embb::base::MakeFunction(readFromFile));

  Network::ParallelProcess<Network::Inputs<AVFrame*>,
    Network::Outputs<AVFrame*> >
    rgb(nw, embb::base::MakeFunction(convertToRGB));

  Network::ParallelProcess<Network::Inputs<AVFrame*>,
    Network::Outputs<AVFrame*> >
    original(nw, embb::base::MakeFunction(convertToOriginal));

// snippet_begin:opencl_pipeline
  Network::ParallelProcess<Network::Inputs<AVFrame*>,
    Network::Outputs<AVFrame*> >
    filter(nw, embb::base::MakeFunction(applyFilterOpenCL));
// snippet_end

  Network::Sink<AVFrame*> write(nw, embb::base::MakeFunction(writeToFile));

  read >> rgb >> filter >> original >> write;

  nw();

  std::cout << std::endl;
  mtapi_opencl_plugin_finalize(MTAPI_NULL);
  embb::mtapi::Node::Finalize();
}

void process_parallel_dataflow_and_algorithms() {
  embb::mtapi::Node::Initialize(1, 1);

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
    filter(nw, embb::base::MakeFunction(applyFilterParallel));

  Network::Sink<AVFrame*> write(nw, embb::base::MakeFunction(writeToFile));

  read >> rgb >> filter >> original >> write;

  nw();

  std::cout << std::endl;
  embb::mtapi::Node::Finalize();
}

void process_parallel_dataflow() {
  embb::mtapi::Node::Initialize(1, 1);

// snippet_begin:pipelined_execution
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

  read >> rgb >> filter >> original >> write;

  nw();
// snippet_end

  std::cout << std::endl;
  embb::mtapi::Node::Finalize();
}

void process_parallel_algorithms() {
  embb::mtapi::Node::Initialize(1, 1);

  AVFrame* frame = nullptr;
  AVFrame* convertedFrame = nullptr;
  AVFrame* originalFrame = nullptr;
  int gotFrame = 0;

  while (readFromFile(frame)) {
    convertToRGB(frame, convertedFrame);
    filter_parallel(convertedFrame);
    convertToOriginal(convertedFrame, originalFrame);
    writeToFile(originalFrame);
  }

  std::cout << std::endl;
  embb::mtapi::Node::Finalize();
}

void process_serial() {
  AVFrame* frame = nullptr;
  AVFrame* convertedFrame = nullptr;
  AVFrame* originalFrame = nullptr;

// snippet_begin:serial_execution
  while (readFromFile(frame)) {
    convertToRGB(frame, convertedFrame);
    filter(convertedFrame);
    convertToOriginal(convertedFrame, originalFrame);
    writeToFile(originalFrame);
  }
// snippet_end

  std::cout << std::endl;
}

int parallel = 0;

bool check_arguments(int argc, char * argv[]) {
  bool result = true;

  std::cout << std::endl << "Video processing application" <<
    std::endl << std::endl;

  if (argc >= 3 && argc <= 4) {
    if (argc == 4) {
      try {
        parallel = std::stoi(argv[3]);
        if (parallel < 0 || parallel > 4) {
          result = false;
        }
      } catch (std::exception &) {
        result = false;
      }
    }
  } else {
    result = false;
  }

  if (!result) {
    std::cout << "usage: video_app <input> <output> [mode]" << std::endl;
    std::cout << "  <input>     source video file name" << std::endl;
    std::cout << "  <output>    output video file name" << std::endl;
    std::cout << "  [mode]      0 = serial (default)" << std::endl;
    std::cout << "              1 = parallel algorithms" << std::endl;
    std::cout << "              2 = parallel dataflow" << std::endl;
    std::cout << "              3 = parallel dataflow and algorithms" <<
      std::endl;
    std::cout << "              4 = parallel dataflow and OpenCL" <<
      std::endl;
    std::cout << std::endl;
  }

  return result;
}

int main(int argc, char *argv[]) {
  // Silence warnings from ffmpeg
  av_log_set_level(AV_LOG_QUIET);

  if (!check_arguments(argc, argv)) {
    return 30;
  }

  // Initialize ffmpeg libraries
  av_register_all();

  // Open input video file
  try {
    inputHandler = new InputVideoHandler(argv[1]);
  } catch (std::exception& e) {
    terminate(e.what(), 31);
  }

  // Open output video file
  try {
    outputBuilder = new OutputVideoBuilder(argv[2],
      inputHandler->getCodecContext());
  } catch (std::exception& e) {
    terminate(e.what(), 32);
  }

  converter.getFormatInfo(inputHandler->getCodecContext());

  // Modify this value to change output quality
  outputBuilder->setMaxQB(7);

  std::string mode = "serial";
  switch (parallel) {
  default:
  case 0:
    mode = "serial";
    break;
  case 1:
    mode = "parallel algorithms";
    break;
  case 2:
    mode = "parallel dataflow";
    break;
  case 3:
    mode = "parallel dataflow and algorithms";
    break;
  case 4:
    mode = "parallel dataflow and OpenCL";
    break;
  }
  std::cout << "Reading and processing video: " << mode <<
    " mode" << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  switch (parallel) {
  case 0:
    process_serial();
    break;
  case 1:
    process_parallel_algorithms();
    break;
  case 2:
    process_parallel_dataflow();
    break;
  case 3:
    process_parallel_dataflow_and_algorithms();
    break;
  case 4:
    process_parallel_dataflow_and_opencl();
    break;
  }

  outputBuilder->writeVideo();
  auto end = std::chrono::high_resolution_clock::now();
  float seconds =
    std::chrono::duration_cast<std::chrono::duration<float> >
      (end - start).count();
  std::cout << "Elapsed time = " << seconds << " s" << std::endl;

  delete inputHandler;
  delete outputBuilder;

  return 0;
}
