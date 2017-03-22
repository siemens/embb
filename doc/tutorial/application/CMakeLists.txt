cmake_minimum_required(VERSION 2.8)
project (video_app)

set(EMBB_DIR "d:/EMBB/_install/CMake")
find_package(EMBB REQUIRED NO_MODULE)
include(${EMBB_USE_FILE})

file(GLOB_RECURSE SOURCES "*.cc" "*.h")

include_directories("${CMAKE_SOURCE_DIR}/include")

if (MSVC)
    set(FFMPEG_DIR "d:/EMBB/ffmpeg")
    include_directories("${FFMPEG_DIR}/include")
    link_directories("${FFMPEG_DIR}/lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
else()
    find_package(ffmpeg REQUIRED)
endif()

set(LINK_EMBB
    embb_dataflow_cpp
    embb_mtapi_cpp
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
