## Tutorial Application

To build the video processing application, a few preparation steps are required:

- Install EMB² and set EMBB_DIR in CMakeLists.txt to the installation directory (if not the default).
- Provide the ffmpeg libraries as described below.
- Create a build directory and run cmake.

Now, the video_app executable can be run with a video of your choice. It expects at least two command line arguments, the first one is the input video file and the second one the resulting output file. An optional third argument specifies the processing mode. There are five modes:

- 0: serial
- 1: parallel algorithms
- 2: parallel dataflow
- 3: parallel dataflow and algorithms
- 4: parallel dataflow and OpenCL

### Linux

Install the following libraries:

- libavcodec-dev
- libavformat-dev
- libswscale-dev
- libavdevice-dev

### Windows

Go to https://ffmpeg.org/ and download a developer package as well as the shared libraries. Extract them to a directory of your choice. Edit CMakeLists.txt and set FFMPEG_DIR to the directory containing the lib and include folders from the developer package.
