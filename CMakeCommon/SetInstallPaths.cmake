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

## Sets the install base path for headers, libraries, and the documentation
# 
function(SetInstallPaths)
  if(WIN32)
    if (DEFINED INSTALL_PREFIX)
      # User given install path given when calling cmake as "-DINSTALL_PREFIX=...".
      set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    else()
      file(TO_CMAKE_PATH "$ENV{ProgramFiles}" _PROG_FILES) # 32-bit dir on win32, useless to us on win64
      file(TO_CMAKE_PATH "$ENV{ProgramFiles(x86)}" _PROG_FILES_X86) # 32-bit dir: only set on win64
      file(TO_CMAKE_PATH "$ENV{ProgramW6432}" _PROG_FILES_W6432) # 64-bit dir: only set on win64

      if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
        # 64-bit build on win64
        set(_PROGFILESDIR "${_PROG_FILES_W6432}")
      else()
        if(_PROG_FILES_W6432)
          # 32-bit build on win64
          set(_PROGFILESDIR "${_PROG_FILES_X86}")
        else()
          # 32-bit build on win32
          set(_PROGFILESDIR "${_PROG_FILES}")
        endif()
      endif()
      set(CMAKE_INSTALL_PREFIX "${_PROGFILESDIR}/${CMAKE_PROJECT_NAME}-${EMBB_BASE_VERSION_MAJOR}.${EMBB_BASE_VERSION_MINOR}.${EMBB_BASE_VERSION_PATCH}")
    endif()
    set(INSTALL_PREFIX_DOCS "${CMAKE_INSTALL_PREFIX}/doc")
    #STRING(REPLACE "\\" "\\\\" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
    #STRING(REPLACE "/" "\\\\" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
    #STRING(REPLACE "/" "\\\\" INSTALL_PREFIX_DOCS ${INSTALL_PREFIX_DOCS} )
    #STRING(REPLACE "\\" "\\\\" INSTALL_PREFIX_DOCS ${INSTALL_PREFIX_DOCS} )
  else()
    if (DEFINED INSTALL_PREFIX)
      # User given install path given when calling cmake as "-DINSTALL_PREFIX=...".
      set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
    else()
      set(CMAKE_INSTALL_PREFIX "/usr/local")
    endif()
    set(INSTALL_PREFIX_DOCS "${CMAKE_INSTALL_PREFIX}/share/doc/${CMAKE_PROJECT_NAME}-${EMBB_BASE_VERSION_MAJOR}.${EMBB_BASE_VERSION_MINOR}.${EMBB_BASE_VERSION_PATCH}")
  endif()
  
  set(INSTALL_PREFIX ${INSTALL_PREFIX} PARENT_SCOPE)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} PARENT_SCOPE)
  set(INSTALL_PREFIX_DOCS ${INSTALL_PREFIX_DOCS} PARENT_SCOPE)
  
  message("-- Installation path is ${CMAKE_INSTALL_PREFIX}")
  if (INSTALL_DOCS STREQUAL "ON")
    message("-- Installation path for documentation is ${INSTALL_PREFIX_DOCS}")
  else()
    message("-- Disabled installation of documentation")
  endif()
endfunction()
