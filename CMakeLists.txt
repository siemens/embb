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

project (EMBB)
cmake_minimum_required (VERSION 2.8.9)

# Version number
set (EMBB_BASE_VERSION_MAJOR 1)
set (EMBB_BASE_VERSION_MINOR 0)
set (EMBB_BASE_VERSION_PATCH 0)

# Fix compilation for CMake versions >= 3.1
#
# New Policy 0054:
# CMake 3.1 and above no longer implicitly dereference variables
# or interpret keywords in an if() command argument when it is a
# Quoted Argument.
# See http://www.cmake.org/cmake/help/v3.1/policy/CMP0054.html
#
# New Policy 0053:
# CMake 3.1 introduced faster implementation of evaluation of the
# Variable References and Escape Sequences. This breaks compilation
# here.
# See http://www.cmake.org/cmake/help/v3.1/policy/CMP0053.html
#
# Set those policies to be treated the legacy (CMake < 3.1) way.
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 OLD)
endif(POLICY CMP0054)

if(POLICY CMP0053)
  cmake_policy(SET CMP0053 OLD)
endif(POLICY CMP0053)

# give the user the possibility, to append compiler flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CMAKE_CXX_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CMAKE_C_FLAGS}")

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the type of build, options are: Debug Release
  RelWithDebInfo MinSizeRel Coverage." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Check for clang, masquerade it as GNU
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_GNUCC true)
endif ()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_GNUCXX true)
endif()

## Command line options
#
# The set option will be converted to uppercase letters by cmake!! --> ON/OFF
# Note that the help string (second argument) cannot be printed by cmake.
#
option(BUILD_TESTS "Specify whether tests should be built" ON)
option(BUILD_EXAMPLES "Specify whether examples should be built" OFF)
option(USE_C11_AND_CXX11 "Specify whether the C11 and C++11 standard versions should be used instead of C99 and C++03" OFF)
option(USE_EXCEPTIONS "Specify whether exceptions should be activated in C++" ON)
option(INSTALL_DOCS "Specify whether Doxygen docs should be installed" ON)
option(WARNINGS_ARE_ERRORS "Specify whether warnings should be treated as errors" OFF)
option(USE_AUTOMATIC_INITIALIZATION "Specify whether the MTAPI C++ interface, algorithms and dataflow should automatically intialize the MTAPI node if no explicit initialization is present" ON)
option(BUILD_OPENCL_PLUGIN "Specify whether the MTAPI OpenCL plugin should be built" OFF)
option(BUILD_CUDA_PLUGIN "Specify whether the MTAPI CUDA plugin should be built" OFF)
option(THREADING_ANALYSIS_MODE "Replaces lock-free synchronization constructs by mutex-based implementations to support threading analysis tools" OFF)
option(BUILD_SHARED_LIBS "Specify whether shared libs should be built" OFF)

if (BUILD_CUDA_PLUGIN STREQUAL ON)
  include(FindCUDA)
endif()

## LOCAL INSTALLATION OF SUBPROJECT BINARIES
#
include(CMakeCommon/CopyInstallFiles.cmake) # Needed in all subprojects
set(local_install_dir ${CMAKE_CURRENT_BINARY_DIR}/binaries)

if (BUILD_SHARED_LIBS STREQUAL ON)
  message("-- Building shared libs enabled")
else()
  message("-- Building shared libs disabled (default)")
endif()
message("   (set with command line option -DBUILD_SHARED_LIBS=ON/OFF)")

if (WARNINGS_ARE_ERRORS STREQUAL ON)
  message("-- Warnings are treated as errors")
  set(EMBB_USE_EXCEPTIONS 1)
else()
  message("-- Warnings are not treated as errors (default)")
endif()
message("   (set with command line option -DWARNINGS_ARE_ERRORS=ON/OFF)")

if (USE_AUTOMATIC_INITIALIZATION STREQUAL ON)
  message("-- MTAPI automatic initialization enabled (default)")
else()
  message("-- MTAPI automatic initialization disabled")
endif()
message("   (set with command line option -DUSE_AUTOMATIC_INITIALIZATION=ON/OFF)")

if (THREADING_ANALYSIS_MODE STREQUAL ON)
  set(EMBB_THREADING_ANALYSIS_MODE 1)
  message("-- Threading analysis mode enabled")
else()
  message("-- Threading analysis mode disabled (default)")
endif()
message("   (set with command line option -DTHREADING_ANALYSIS_MODE=ON/OFF)")

include(CMakeCommon/SetCompilerFlags.cmake)
SetGNUCompilerFlags(compiler_libs compiler_flags)
SetVisualStudioCompilerFlags(compiler_libs compiler_flags)

## Exception handling in C++
#
if (USE_EXCEPTIONS STREQUAL ON)
  message("-- Exceptions enabled (default) ")
  set(EMBB_USE_EXCEPTIONS 1)
else()
  message("-- Exceptions disabled")
  set(EMBB_NO_EXCEPTIONS) # A preprocessor define
  if (CMAKE_COMPILER_IS_GNUCXX)
    LIST(APPEND ${CMAKE_CXX_FLAGS} "-fno-exceptions")
  elseif (MSVC)
    LIST(FIND ${CMAKE_CXX_FLAGS} "/EHsc" EXCEPTION_FLAG)
    if (EXCEPTION_FLAG)
      string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_HAS_EXCEPTIONS=0")
  endif()
endif()
message("   (set with command line option -DUSE_EXCEPTIONS=ON/OFF)")

# these are the test executables, we expect to be generated.
set(EXPECTED_EMBB_TEST_EXECUTABLES "embb_algorithms_cpp_test"
                "embb_base_c_test"
                "embb_base_cpp_test"
                "embb_containers_cpp_test"
                "embb_dataflow_cpp_test"
                "embb_mtapi_c_test"
                "embb_mtapi_cpp_test"
                "embb_mtapi_network_c_test"
                )

# if opencl is there, we also expect the mtapi opencl test to be generated
if (BUILD_OPENCL_PLUGIN STREQUAL ON)
  message("-- Building OpenCL plugin enabled")
else()
  message("-- Building OpenCL plugin disabled (default)")
endif()
message("   (set with command line option -DBUILD_OPENCL_PLUGIN=ON/OFF)")

if(BUILD_OPENCL_PLUGIN STREQUAL ON)
  list(APPEND EXPECTED_EMBB_TEST_EXECUTABLES "embb_mtapi_opencl_c_test")
endif()

if (BUILD_CUDA_PLUGIN STREQUAL ON)
  message("-- Building CUDA plugin enabled")
else()
  message("-- Building CUDA plugin disabled (default)")
endif()
message("   (set with command line option -DBUILD_CUDA_PLUGIN=ON/OFF)")

if (BUILD_CUDA_PLUGIN STREQUAL ON)
  if(CUDA_FOUND)
    list(APPEND EXPECTED_EMBB_TEST_EXECUTABLES "embb_mtapi_cuda_c_test")
  else()
    message("   Cannot build CUDA plugin: CUDA not found")
  endif()
endif()

## Copy test execution script to local binaries folder
if (DEFINED CYGWIN)
  set(test_script_in run_tests_cygwin.sh)
  set(test_script_out run_tests.sh)
elseif (DEFINED UNIX)
  set(test_script_in run_tests_unix.sh)
  set(test_script_out run_tests.sh)
else()
  set(test_script_in run_tests_windows.bat)
  set(test_script_out run_tests.bat)
endif()
configure_file( ${CMAKE_SOURCE_DIR}/scripts/${test_script_in}.cmake binaries/${test_script_out} )

## Test and Partest build
#
include(CMakeCommon/CheckEnableTests.cmake)
if (BUILD_TESTS STREQUAL ON)
  message("-- Building tests enabled (default)")
else()
  message("-- Building tests disabled")
endif()
message("   (set with command line option -DBUILD_TESTS=ON/OFF)")
CheckPartestInstall(${BUILD_TESTS} partest_includepath partest_libpath)

## SUBPROJECTS
#
add_subdirectory(base_c)
add_subdirectory(base_cpp)
add_subdirectory(mtapi_c)
add_subdirectory(mtapi_plugins_c/mtapi_network_c)
if(BUILD_OPENCL_PLUGIN STREQUAL ON)
  add_subdirectory(mtapi_plugins_c/mtapi_opencl_c)
endif()
if(BUILD_CUDA_PLUGIN STREQUAL ON)
  if(CUDA_FOUND)
    add_subdirectory(mtapi_plugins_c/mtapi_cuda_c)
  endif()
endif()
add_subdirectory(mtapi_cpp)
add_subdirectory(containers_cpp)
add_subdirectory(algorithms_cpp)
add_subdirectory(dataflow_cpp)
if (BUILD_EXAMPLES STREQUAL ON)
  message("-- Building examples enabled")
  add_subdirectory(doc/examples)
else()
  message("-- Building examples disabled (default)")
endif()
message("   (set with command line option -DBUILD_EXAMPLES=ON/OFF)")

## INSTALLATION
#
include(CMakeCommon/SetInstallPaths.cmake)
SetInstallPaths()

## DOXYGEN
#
if(EXISTS "${EMBB_SOURCE_DIR}/doc/reference/Doxyfile.in")
  include(CMakeCommon/CreateDoxygenDocumentationTarget.cmake)
  CreateDoxygenDocumentationTarget()
endif()

if (INSTALL_DOCS STREQUAL ON)
  message("-- Installing documentation enabled (default)")
else()
  message("-- Installing documentation disabled")
endif()
message("   (set with command line option -DINSTALL_DOCS=ON/OFF)")

if (INSTALL_DOCS STREQUAL ON)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/
          DESTINATION ${INSTALL_PREFIX_DOCS} FILES_MATCHING PATTERN "*.*" PATTERN "CMakeLists.txt" EXCLUDE)
endif()

## create cmake finder
configure_file("EMBBConfig.cmake.in" "EMBBConfig.cmake")
configure_file("UseEMBB.cmake.in" "UseEMBB.cmake")
install(FILES "${CMAKE_BINARY_DIR}/EMBBConfig.cmake" DESTINATION "CMake")
install(FILES "${CMAKE_BINARY_DIR}/UseEMBB.cmake" DESTINATION "CMake")
install(
   EXPORT EMBB-Targets
   DESTINATION "CMake"
   FILE EMBB-Targets.cmake
)
