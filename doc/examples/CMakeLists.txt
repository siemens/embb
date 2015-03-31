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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_network_c/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../tasks_cpp/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../tasks_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../containers_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../algorithms_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../dataflow_cpp/include
  )

if(OpenCL_FOUND)
  # used in source code, to include opencl code
  add_definitions(-DEMBB_WITH_OPENCL)
  # add opencl includes
  include_directories( 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_opencl_c/include
  )
  # later used, to link opencl to target...
  set (EMBB_MTAPI_OPENCL_C_CONDITIONAL "embb_mtapi_opencl_c")
else()
  # remove opencl examples from sources (should not be build)
  file(GLOB_RECURSE EXAMPLES_SOURCES_OPENCL_TO_REMOVE "*opencl*" )
  list(REMOVE_ITEM EXAMPLES_SOURCES ${EXAMPLES_SOURCES_OPENCL_TO_REMOVE})
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11")
  set (EXTRA_LIBS dl)
endif()

IF(MSVC)
  set (EXTRA_LIBS ws2_32)
ENDIF()

add_executable(examples ${EXAMPLES_SOURCES})
target_link_libraries(examples embb_dataflow_cpp embb_algorithms_cpp embb_tasks_cpp embb_mtapi_cpp
                      embb_mtapi_network_c ${EMBB_MTAPI_OPENCL_C_CONDITIONAL} embb_mtapi_c
                      embb_base_cpp embb_base_c embb_containers_cpp
                      ${EXTRA_LIBS} ${compiler_libs})
CopyBin(BIN examples DEST ${local_install_dir})
