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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../tasks_cpp/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../tasks_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../containers_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../algorithms_cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../dataflow_cpp/include
  )

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

add_executable(examples ${EXAMPLES_SOURCES})
target_link_libraries(examples embb_dataflow_cpp embb_algorithms_cpp embb_tasks_cpp embb_mtapi_cpp
                      embb_mtapi_c embb_base_cpp embb_base_c embb_containers_cpp ${compiler_libs})
CopyBin(BIN examples DEST ${local_install_dir})
