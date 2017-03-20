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

## DOXYGEN DOCUMENTATION
#
#  Is only generated when option(.* ON) is set...
function (CreateDoxygenDocumentationTarget)
  option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
  if (BUILD_DOCUMENTATION STREQUAL ON)
    message("-- Building documentation enabled")
  else()
    message("-- Building documentation disabled (default)")
  endif()
  message("   (set with command line option -DBUILD_DOCUMENTATION=ON/OFF)")
  if(BUILD_DOCUMENTATION)
    FIND_PACKAGE(Doxygen)
    #if (NOT DOXYGEN_FOUND)
    #  message(FATAL_ERROR
    #    "Doxygen is needed to build the documentation. Please install it correctly")
    #endif()
    #-- Configure the Template Doxyfile for our specific project
    configure_file(doc/reference/Doxyfile.in
                   ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
    #-- Add a custom target to run Doxygen when ever the project is built
    if (TARGET doxygen)
      # Do nothing, since the repeated adding causes an error
    else()

    FILE(WRITE ${CMAKE_BINARY_DIR}/doxygen_makefile.cmake "
MESSAGE(STATUS \"Running Doxygen\")
EXECUTE_PROCESS(
COMMAND \${EXE} \${IN}
ERROR_VARIABLE  DOXYGEN_OUT_ERR
RESULT_VARIABLE DOXYGEN_OUT_RESULT)
STRING(LENGTH \"\${DOXYGEN_OUT_ERR}\" LENGTH_ERR)
IF ( NOT \${LENGTH_ERR} STREQUAL \"0\" )
  MESSAGE (WARNING \"Doxygen produced following warnings and or/errors: \${DOXYGEN_OUT_ERR}\")
  IF ( \${WARNINGS_ARE_ERRORS} STREQUAL ON OR NOT \${DOXYGEN_OUT_RESULT} STREQUAL \"0\" )
    MESSAGE (FATAL_ERROR \"Exiting doxygen generation due to errors (or warnings, if WARNINGS_ARE_ERRORS is enabled)\")
  ENDIF ()
ENDIF ()
")

      add_custom_target(doxygen)
      if (NOT DOXYGEN_FOUND)
        add_custom_command(
          TARGET doxygen
          COMMAND echo "Doxygen is needed to build the documentation. Please install it correctly")
      else()
        add_custom_command(
          TARGET doxygen
          COMMAND ${CMAKE_COMMAND}
            -DEXE=${DOXYGEN_EXECUTABLE}
            -DIN=${PROJECT_BINARY_DIR}/Doxyfile
            -DWARNINGS_ARE_ERRORS=${WARNINGS_ARE_ERRORS}
            -P ${CMAKE_BINARY_DIR}/doxygen_makefile.cmake)
      endif()
    endif()
  endif()
endfunction()
