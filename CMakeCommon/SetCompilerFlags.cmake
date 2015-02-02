# Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

## DETERMINE COMPILER AND LINKER FLAGS
#
function(SetGNUCompilerFlags compiler_libs)
  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(compiler_libs pthread rt PARENT_SCOPE)
    # -Wall   -> All warnings
    # -Wextra -> Even more warnings
    # -Werror -> Warnings are errors
    set(warning_flags "-Wall -Wextra")
    if (WARNINGS_ARE_ERRORS STREQUAL ON)
      set(warning_flags "${warning_flags} -Werror")
    endif()
    if(CMAKE_COMPILER_IS_GNUCC)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -std=c99 ${warning_flags}"
          PARENT_SCOPE)
      set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DEMBB_DEBUG"
          PARENT_SCOPE)
      set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG"
          PARENT_SCOPE)
      set(CMAKE_C_FLAGS_COVERAGE
         "${CMAKE_C_FLAGS_COVERAGE} -O0 -fprofile-arcs -ftest-coverage"
         PARENT_SCOPE)
    endif()
    if(CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++03 ${warning_flags}"
          PARENT_SCOPE)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DEMBB_DEBUG"
          PARENT_SCOPE)
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG"
          PARENT_SCOPE)
      set(CMAKE_CXX_FLAGS_COVERAGE
         "${CMAKE_CXX_FLAGS_COVERAGE} -O0 -fprofile-arcs -ftest-coverage"
         PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(SetVisualStudioCompilerFlags)
  if(MSVC)
    # /Wall   -> All warnings
    # /WX     -> Warnings as errors
    #
    # Globally deactivated warning numbers (by flag \wd#):
    # 4820 -> Deactivates warning "2/4/... bytes padding added after some type"
    # 4514 -> Deactivates warning "'fct': unreferenced inline function has
    #         been removed"
    # 4668 -> Deactivates warning "'macro' is not defined as preprocessor macro,
    #         replacing with '0' in #if/#elif"
    # 4710 -> Deactivates warning "Function not inlined"
    # 4350 -> Deactivates warning "Behavior change ...", which warns a
    #         behavior change since VS C++ 2002, when using R-values as
    #         L-value arguments. This warning occurs a lot in the VC libs.
    # 4571 -> Deactivates warning that when compiling with /EHs,
    #         a catch(...) block will not catch a structured exception.
    # 4625 -> Deactivates warning for derived classes
    #         when copy constructor could not be generated because
    #         a base class copy constructor is inaccessible
    # 4626 -> Deactivates warning for derived classes
    #         when assignment operator could not be generated because
    #         a base class assignment operator is inaccessible
    # 4711 -> Deactivates warning for inline functions
    #         This is only an informational warning about which functions
    #         have been inlined by the compiler.
    # 4255 -> Deactivates warning "no function prototype given converting () to (void)"
    #
    # Locally suppressed warnings (should not be globally suppressed):
    # 4640 -> Information that local static variable initialization is not
    #         thread-safe.
    set(warning_flags "/Wall /wd4820 /wd4514 /wd4668 /wd4710 /wd4350 /wd4571 /wd4625 /wd4626 /wd4711 /wd4255")
	if (WARNINGS_ARE_ERRORS STREQUAL ON)
      set(warning_flags "${warning_flags} /WX")
	endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warning_flags}" PARENT_SCOPE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warning_flags}" PARENT_SCOPE)
  endif()
endfunction()
