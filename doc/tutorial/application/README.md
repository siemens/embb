## Tutorial Application

To build the application a few preparation steps are required:

- Install EMB² and set EMBB_DIR to the installation directory (if not default).
- Provide the ffmpeg libraries like detailed below.
- Create a build directory and run cmake.

Now the video_app executable can be run and used with a video file of your choice. It expects at least two command line arguments, the first is the input video file and the second is the name of the result video file. An optional third argument specifies the processing mode. There are 5 processing modes:

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

Go to ffmeg.org and download a developer package and the shared libraries. Extract them to a directory of your choosing. Edit CMakeLists.txt and set FFMPEG_DIR to the directory containing the lib and include folders from the developer package.
