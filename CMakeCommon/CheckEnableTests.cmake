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

## CheckPartestInstall: Checks if partest install path is defined
#  Reads: PARTEST_INSTALL_PREFIX
#  Sets: partest_includepath_var
#        partest_libpath_var
#
function(CheckPartestInstall BUILD_TESTS _partest_includepath _partest_libpath)  
  if (BUILD_TESTS STREQUAL ON)
    set (PARTEST_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/partest_install PARENT_SCOPE)
    set (PARTEST_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/partest_install)
     
    set(${_partest_includepath} ${PARTEST_INSTALL_PREFIX}/include PARENT_SCOPE)
    set(${_partest_libpath} ${PARTEST_INSTALL_PREFIX}/lib PARENT_SCOPE)
      
    set (tar_extraction_directory ${CMAKE_CURRENT_BINARY_DIR})
    message("-- Extracting partest to directoy ${tar_extraction_directory}/partest")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xf ${PROJECT_SOURCE_DIR}/partest.tar
      WORKING_DIRECTORY ${tar_extraction_directory}
    )
      
    if (TARGET PARTEST)
    else()
      add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/partest ${CMAKE_CURRENT_BINARY_DIR}/partest_build)  
	endif()
  endif()
endfunction()
