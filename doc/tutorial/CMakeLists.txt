#CMakeLists for using EMBB as an external library in a project.

project (EMBB_USER_PROJECT)
set(EXECUTABLE_NAME exec)
set(BUILD_STATIC 1)

cmake_minimum_required (VERSION 2.8.9)

# Version number
set (EMBB_USER_PROJECT_BASE_VERSION_MAJOR 0)
set (EMBB_USER_PROJECT_BASE_VERSION_MINOR 0)
set (EMBB_USER_PROJECT_BASE_VERSION_PATCH 1)

#############################################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    if(BUILD_STATIC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    endif()
    set(INT_DIR "/${CMAKE_CFG_INTDIR}/")
    set(ADDITIONAL_LIBS "")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    set(INT_DIR "/")
    set(ADDITIONAL_LIBS "pthread" "rt")
else()
 message(FATAL_ERROR "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

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

set(EMBB_DIRS containers_cpp algorithms_cpp dataflow_cpp tasks_cpp mtapi_cpp base_cpp mtapi_c base_c)
set(EMBB_GENERATED_INCLUDE base_c base_cpp tasks_cpp)
### end external project

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)
#Find other modules if any

####################################################

file(GLOB_RECURSE EMBB_USER_PROJECT_SOURCES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE EMBB_USER_PROJECT_HEADERS "src/*.h")

add_executable(${EXECUTABLE_NAME} ${EMBB_USER_PROJECT_SOURCES} ${EMBB_USER_PROJECT_HEADERS})
add_dependencies(${EXECUTABLE_NAME} EMBB)

#############################################

# Make a browsable file hierarchy when opening in IDE
function(GroupSources directory)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${directory} ${PROJECT_SOURCE_DIR}/${directory}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${directory}/${child})
                  GroupSources(${directory}/${child})
          else()
                   if(WIN32)
                        string(REPLACE "/" "\\" groupname ${directory})
                   endif()
                  string(REPLACE "src" "Sources" groupname ${groupname})
                  string(REPLACE "include" "Includes" groupname ${groupname})
                  source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${directory}/${child})
          endif()
   endforeach()
endfunction()

GroupSources(src)

####################################################

 if(BUILD_STATIC)
   set(pref ${CMAKE_STATIC_LIBRARY_PREFIX})
   set(suf ${CMAKE_STATIC_LIBRARY_SUFFIX})
 else()
   set(pref ${CMAKE_SHARED_LIBRARY_PREFIX})
   set(suf ${CMAKE_SHARED_LIBRARY_SUFFIX})
 endif()

###########################################

foreach(dir ${EMBB_DIRS})
  include_directories ("${EMBB_SOURCE_DIR}/${dir}/include")
  list(APPEND LIBS "${EMBB_BINARY_DIR}/${dir}${INT_DIR}${pref}${dir}${suf}")
endforeach()

foreach(dir ${EMBB_GENERATED_INCLUDE})
    include_directories("${EMBB_BINARY_DIR}/${dir}/include")
endforeach()
target_link_libraries( ${EXECUTABLE_NAME} ${LIBS} ${ADDITIONAL_LIBS}   )
