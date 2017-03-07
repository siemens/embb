project (project_embb_tutorials)

file(GLOB_RECURSE EXAMPLES_SOURCES "*.cc" "*.h")


include_directories(  
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${CMAKE_CURRENT_BINARY_DIR}/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../base_c/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../base_c/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../base_cpp/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../base_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_c/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_c/src
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_plugins_c/mtapi_network_c/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_cpp/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../mtapi_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../containers_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../algorithms_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../dataflow_cpp/include
  )

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11")
  set (EXTRA_LIBS dl)
endif()

IF(MSVC)
  set (EXTRA_LIBS ws2_32)
ENDIF()

if(BUILD_OPENCL_PLUGIN STREQUAL ON)
  # used in source code, to include opencl code
  add_definitions(-DEMBB_WITH_OPENCL)
  # add opencl includes
  include_directories( 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_plugins_c/mtapi_opencl_c/include
  )
  # later used, to link opencl to target...
  set (EMBB_MTAPI_OPENCL_C_CONDITIONAL "embb_mtapi_opencl_c")
else()
  # remove opencl examples from sources (should not be build)
  file(GLOB_RECURSE EXAMPLES_SOURCES_OPENCL_TO_REMOVE "*opencl*" )
  list(REMOVE_ITEM EXAMPLES_SOURCES ${EXAMPLES_SOURCES_OPENCL_TO_REMOVE})
endif()

if(CUDA_FOUND)
  # used in source code, to include cuda code
  add_definitions(-DEMBB_WITH_CUDA)
  # build cuda kernel
  add_custom_command(
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/mtapi/mtapi_c_cuda_kernel.cu"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.ptx"
    COMMAND ${CUDA_NVCC_EXECUTABLE} -ptx -m 32
      "${CMAKE_CURRENT_SOURCE_DIR}/mtapi/mtapi_c_cuda_kernel.cu"
      -o "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.ptx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  add_custom_command(
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.ptx"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.h"
    COMMAND ${CUDA_TOOLKIT_ROOT_DIR}/bin/bin2c -p 0
      "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.ptx" >
      "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  list(APPEND EXAMPLES_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/mtapi_c_cuda_kernel.h")
  list(APPEND EXAMPLES_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mtapi/mtapi_c_cuda_kernel.cu")
  # add cuda includes
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_plugins_c/mtapi_cuda_c/include
  )
  # later used, to link cuda to target...
  set (EMBB_MTAPI_CUDA_C_CONDITIONAL "embb_mtapi_cuda_c")

  list(APPEND EXTRA_LIBS ${CUDA_CUDA_LIBRARY})
else()
  # remove opencl examples from sources (should not be build)
  file(GLOB_RECURSE EXAMPLES_SOURCES_CUDA_TO_REMOVE "*cuda*" )
  list(REMOVE_ITEM EXAMPLES_SOURCES ${EXAMPLES_SOURCES_CUDA_TO_REMOVE})
endif()

include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(algorithms)
GroupSourcesMSVC(containers)
GroupSourcesMSVC(dataflow)
GroupSourcesMSVC(mtapi)
GroupSourcesMSVC(stl_for_each)

add_executable(examples ${EXAMPLES_SOURCES})
target_link_libraries(examples embb_dataflow_cpp embb_algorithms_cpp embb_mtapi_cpp
                      embb_mtapi_network_c
                      ${EMBB_MTAPI_OPENCL_C_CONDITIONAL}
                      ${EMBB_MTAPI_CUDA_C_CONDITIONAL}
                      embb_mtapi_c embb_base_cpp embb_base_c embb_containers_cpp
                      ${EXTRA_LIBS} ${compiler_libs})
CopyBin(BIN examples DEST ${local_install_dir})
