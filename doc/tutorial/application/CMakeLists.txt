cmake_minimum_required(VERSION 2.8)
project (video_app)

# uncomment the following line and provide the correct EMBB install directory
# if EMBB is not found in the default locations

#set(EMBB_DIR "MY-EMBB-DIRECTORY")

find_package(EMBB REQUIRED NO_MODULE)
include(${EMBB_USE_FILE})

file(GLOB_RECURSE SOURCES "*.cc" "*.h")

include_directories("${CMAKE_SOURCE_DIR}/include")

if (MSVC)

# provide the correct ffmpeg install directory in the line below
    set(FFMPEG_DIR "MY-FFMPEG-DIRECTORY")

    include_directories("${FFMPEG_DIR}/include")
    link_directories("${FFMPEG_DIR}/lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
  set (EXTRA_LIBS pthread dl rt)
endif()

set(LINK_EMBB
    embb_dataflow_cpp
    embb_mtapi_cpp
    embb_mtapi_opencl_c
    embb_mtapi_c
    embb_base_cpp
    embb_base_c
)

set(LINK_FFMPEG
    avcodec
    avutil
    avdevice
    avfilter
    avformat
    postproc
    swresample
    swscale
)

add_executable(video_app ${SOURCES})
target_link_libraries(video_app ${LINK_EMBB} ${LINK_FFMPEG})
