cmake_minimum_required (VERSION 2.6)
project (Linearizability-checker C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lrt")
find_package(Threads REQUIRED)


if(CMAKE_USE_PTHREADS_INIT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# setting directories
# link_directories(/home/lucapegolotti/embb/build)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include)


# setting names of libraries
set(PROJECT_LINK_LIBS 
${CMAKE_CURRENT_SOURCE_DIR}/../build/base_cpp/libembb_base_cpp.a 
${CMAKE_CURRENT_SOURCE_DIR}/../build/base_c/libembb_base_c.a 
${CMAKE_CURRENT_SOURCE_DIR}/../build/containers_cpp/libembb_containers_cpp.a
)


add_executable(${PROJECT_NAME} lt.cc)
# target_link_libraries(${PROJECT_NAME} ${CMAKE_THREADS_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${PROJECT_LINK_LIBS}) # ${CMAKE_THREADS_LIBS_INIT})

