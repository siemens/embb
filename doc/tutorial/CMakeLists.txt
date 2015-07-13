project (EMBB_USER_PROJECT)
set(EXECUTABLE_NAME exec)
cmake_minimum_required (VERSION 2.8.9)

# Version number
set (EMBB_USER_PROJECT_BASE_VERSION_MAJOR 0)
set (EMBB_USER_PROJECT_BASE_VERSION_MINOR 0)
set (EMBB_USER_PROJECT_BASE_VERSION_PATCH 1)

### begin external project
include(ExternalProject)
file(GLOB EMBB_LIB_FILE ${CMAKE_SOURCE_DIR}/external/embb*.tar.gz)
ExternalProject_Add(EMBB
  URL ${EMBB_LIB_FILE}
  CMAKE_ARGS -DBUILD_TESTS=OFF
  INSTALL_COMMAND ""
)


ExternalProject_Get_Property(EMBB source_dir)
ExternalProject_Get_Property(EMBB binary_dir)

set(EMBB_BINARY_DIR ${binary_dir})
set(EMBB_SOURCE_DIR ${source_dir})

### end external project

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

include_directories ( 
  "${EMBB_SOURCE_DIR}/algorithms_cpp/include"
  "${EMBB_SOURCE_DIR}/base_c/include"
  "${EMBB_SOURCE_DIR}/base_cpp/include"
  "${EMBB_SOURCE_DIR}/containers_cpp/include"
  "${EMBB_SOURCE_DIR}/dataflow_cpp/include"
  "${EMBB_SOURCE_DIR}/mtapi_c/include"
  "${EMBB_SOURCE_DIR}/mtapi_cpp/include"
  "${EMBB_SOURCE_DIR}/mtapi_cpp/include"
  "${EMBB_SOURCE_DIR}/tasks_cpp/include"
  "${EMBB_BINARY_DIR}/base_c/include"
  "${EMBB_BINARY_DIR}/base_cpp/include"
  "${EMBB_BINARY_DIR}/tasks_cpp/include"
)




file(GLOB_RECURSE EMBB_USER_PROJECT_SOURCES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE EMBB_USER_PROJECT_HEADERS "src/*.h")

add_executable(${EXECUTABLE_NAME} ${EMBB_USER_PROJECT_SOURCES} ${EMBB_USER_PROJECT_HEADERS})
add_dependencies(${EXECUTABLE_NAME} EMBB)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

## Visual Studio Project Adaptations
#
# Make a browsable file hierarchy when opening
# the generated visual c++ solution project in visual studio
function(GroupSourcesMSVC directory)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${directory} ${PROJECT_SOURCE_DIR}/${directory}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${directory}/${child})
                  GroupSourcesMSVC(${directory}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${directory})
                  string(REPLACE "src" "Sources" groupname ${groupname})
                  string(REPLACE "include" "Includes" groupname ${groupname})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${directory}/${child})
          endif()
   endforeach()
endfunction()
GroupSourcesMSVC(src)
target_link_libraries(
  ${EXECUTABLE_NAME}
  ${EMBB_BINARY_DIR}/algorithms_cpp/${CMAKE_CFG_INTDIR}/embb_algorithms_cpp.lib
  ${EMBB_BINARY_DIR}/base_c/${CMAKE_CFG_INTDIR}/embb_base_c.lib
  ${EMBB_BINARY_DIR}/base_cpp/${CMAKE_CFG_INTDIR}/embb_base_cpp.lib
  ${EMBB_BINARY_DIR}/containers_cpp/${CMAKE_CFG_INTDIR}/embb_containers_cpp.lib
  ${EMBB_BINARY_DIR}/dataflow_cpp/${CMAKE_CFG_INTDIR}/embb_dataflow_cpp.lib
  ${EMBB_BINARY_DIR}/mtapi_c/${CMAKE_CFG_INTDIR}/embb_mtapi_c.lib
  ${EMBB_BINARY_DIR}/mtapi_cpp/${CMAKE_CFG_INTDIR}/embb_mtapi_cpp.lib
  ${EMBB_BINARY_DIR}/tasks_cpp/${CMAKE_CFG_INTDIR}/embb_tasks_cpp.lib
   )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
target_link_libraries(
  ${EXECUTABLE_NAME}
   ${EMBB_BINARY_DIR}/containers_cpp/libembb_containers_cpp.a
   ${EMBB_BINARY_DIR}/algorithms_cpp/libembb_algorithms_cpp.a
   ${EMBB_BINARY_DIR}/dataflow_cpp/libembb_dataflow_cpp.a
   ${EMBB_BINARY_DIR}/tasks_cpp/libembb_tasks_cpp.a
   ${EMBB_BINARY_DIR}/mtapi_cpp/libembb_mtapi_cpp.a
   ${EMBB_BINARY_DIR}/base_cpp/libembb_base_cpp.a
   ${EMBB_BINARY_DIR}/mtapi_c/libembb_mtapi_c.a
   ${EMBB_BINARY_DIR}/base_c/libembb_base_c.a
   pthread
   rt
   )
else()
 message(FATAL_ERROR "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif()




