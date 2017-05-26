# Copyright (c) 2014-2017, Siemens AG. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8)
project (video_app)

# Uncomment the following line and provide the correct installation directory
# if EMBB cannot be found in the default location.

#set(EMBB_DIR "MY-EMBB-DIRECTORY")

find_package(EMBB REQUIRED NO_MODULE)
include(${EMBB_USE_FILE})

file(GLOB_RECURSE SOURCES "*.cc" "*.h")

include_directories("${CMAKE_SOURCE_DIR}/include")

if(MSVC)
    # Provide the correct FFmpeg installation directory in the line below.
	# Make sure to use slashes instead of backslashes in paths.
    set(FFMPEG_DIR "MY-FFMPEG-DIRECTORY")
    include_directories("${FFMPEG_DIR}/include")
    link_directories("${FFMPEG_DIR}/lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -Wno-deprecated-declarations")
  set(EXTRA_LIBS pthread dl rt)
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
target_link_libraries(video_app ${LINK_EMBB} ${LINK_FFMPEG} ${EXTRA_LIBS})
