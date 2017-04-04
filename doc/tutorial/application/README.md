## Tutorial Application

To apply the conceptes detailed above in practice, we create a video processing application in the following paragraphs. The application is supposed to accept a video file on the command line, apply some filter and write an output video to a given file. For handling video files, we are going to use ffmpeg.

### Prerequisites

To build the application a few preparation steps are required.
Install EMB² and set EMBB_DIR to the installation directory (if not default).

#### Linux

Install the following libraries:
  libavcodec-dev
  libavformat-dev
  libswscale-dev
  libavdevice-dev

Create a build directory and run cmake.

#### Windows

Go to ffmeg.org and download a developer package and the shared libraries. Extract them to a directory of your choosing. Edit CMakeLists.txt and set FFMPEG_DIR to the directory containing the lib and include folders from the developer package.

Create a build directory and run cmake.

### Implementation

The application consists of 5 parts, the main application, the filters, the input video handler, the frame format converter and the output video builder.

The input video handler opens a given video file and is used to read consecutive frames from the stream until there are no more frames.

The frame format converter is used to convert from the source color format to RGB and vice versa, since all filter processing is done in RGB color space.

The output video builder encodes the resulting image stream back into a video file.

We provide several example filters to use during processing. The filters come in 3 flavours: sequential, parallel using the algorithms library and as OpenCL kernels.

Finally, the main application binds the other components into a working whole.

#### Main Application

The main application checks the commandline parameters for validity and displays usage information if errors are detected. Then the input video is openened and checked for decodability. The output video builder is created and the availability of the requested coded is checked.

After the ffmpeg setup is done, the filters are applied to the video stream depending on the commandline parameters. There are serial, parallel algortihms, parallel dataflow, parallel dataflow+algorithms and parallel dataflow+OpenCL variants.

#### Serial Execution

Serial execution is decoding of the source frame, conversion to RGB color space, filtering, conversion back to the original color space and encoding the frame into the output video. The filters are ordinary loops that iterate over the pixels of the input frame.

#### Parallel Filters using Algorithms

The filters can obviously be parallelized on the pixel level. Therefore, instead of ordinary loops, `embb::algorithms::ForEach` is used to iterate over the pixels of the frame. The flow of execution is the same as in serial mode though. This version does not scale perfectly, since encoding, conversiond and decoding are sequential steps and only the filtering part runs in parallel.

#### Pipelining using Dataflow

Looking at the flow of the serial code it is clear that it constitutes a linear pipeline. Thus, the dataflow building block can be used to make that pipline run in parallel. The calls from the serial version are wrapped into a `Source`, three `ParallelProcess`es, and a `Sink`. A Network is constructed, the processes are connected and the network is run. The filters are run sequentially. Assuming that all 3 stages of the pipeline are equally fast, that version would only scale to up to 3 cores.

#### Pipelining and Parallel Filters

Now, the two previous techniques are combined to allow for even better scaling. Basic execution is done using a dataflow network and the filters are run in parallel.

#### OpenCL Filters

To demonstrate the support for heterogeneous systems, the OpenCL plugin is used to offload the filtering process onto a GPU. The filters are now single shot kernels that process one pixel each. Processing is pipelined using a dataflow network.
