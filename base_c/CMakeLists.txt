project (project_embb_base_c)

include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)


## CODE FILE DETECTION
#
# Fetch all header and source files for lib and test build separately
file(GLOB_RECURSE EMBB_BASE_SOURCES "src/*.c" "src/*.h" "src/*.cc")
file(GLOB_RECURSE EMBB_BASE_HEADERS "include/embb/base/c/*.h")
if (BUILD_TESTS STREQUAL ON)
  file(GLOB_RECURSE EMBB_BASE_TEST_SOURCES "test/*.cc" "test/*.h")
  IF(MSVC8 OR MSVC9 OR MSVC10 OR MSVC11)
  FOREACH(src_tmp ${EMBB_BASE_TEST_SOURCES})
      SET_PROPERTY(SOURCE ${src_tmp} PROPERTY LANGUAGE CXX)
  ENDFOREACH(src_tmp)
  ENDIF()
endif()

IF(MSVC8 OR MSVC9 OR MSVC10 OR MSVC11)
FOREACH(src_tmp ${EMBB_BASE_SOURCES})
    SET_PROPERTY(SOURCE ${src_tmp} PROPERTY LANGUAGE CXX)
ENDFOREACH(src_tmp)
ENDIF()

include(CheckTypeSize)
check_type_size(char EMBB_CHAR_TYPE_SIZE)
check_type_size(short EMBB_SHORT_TYPE_SIZE)
check_type_size("unsigned short" EMBB_UNSIGNED_SHORT_TYPE_SIZE)
check_type_size(int EMBB_INT_TYPE_SIZE)
check_type_size("unsigned int" EMBB_UNSIGNED_INT_TYPE_SIZE)
check_type_size(long EMBB_LONG_TYPE_SIZE)
check_type_size("unsigned long" EMBB_UNSIGNED_LONG_TYPE_SIZE)
check_type_size("long long" EMBB_LONG_LONG_TYPE_SIZE)
check_type_size("unsigned long long" EMBB_UNSIGNED_LONG_LONG_TYPE_SIZE)
check_type_size(intptr_t EMBB_INTPTR_T_TYPE_SIZE)
check_type_size(uintptr_t EMBB_UINTPTR_T_TYPE_SIZE)
check_type_size(size_t EMBB_SIZE_T_TYPE_SIZE)
check_type_size(ptrdiff_t EMBB_PTRDIFF_T_TYPE_SIZE)
check_type_size(uintmax_t EMBB_UINTMAX_T_TYPE_SIZE)

# Create header file from input file
configure_file("include/embb/base/c/internal/atomic/atomic_sizes.h.in" 
               "include/embb/base/c/internal/atomic/atomic_sizes.h")

## Compiling and linking assembler code for atomic operations in MSVC
#
# The assembler implementation for atomic operations in MSVC is
# contained in external asm files (inline assembler not possible). 
# The lines below are used for compiling those file to object files 
# and linking them to the embb library.
#
if(MSVC)
  if(EMBB_INTPTR_T_TYPE_SIZE EQUAL 8)
    set(EMBB_BASE_ASM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/atomicfunc_64.asm")
    set(EMBB_BASE_ASM_OBJS "${CMAKE_CURRENT_BINARY_DIR}/atomicfunc_64.obj")
    set(MSVC_ML ml64)
    set(MSVC_SAFESEH "")
  else()
    set(EMBB_BASE_ASM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/atomicfunc_32.asm")
    set(EMBB_BASE_ASM_OBJS "${CMAKE_CURRENT_BINARY_DIR}/atomicfunc_32.obj")
    set(MSVC_ML ml)
    set(MSVC_SAFESEH "/safeseh")
  endif()
  add_custom_command(
    DEPENDS ${EMBB_BASE_ASM_SOURCES}
    OUTPUT ${EMBB_BASE_ASM_OBJS}
    COMMAND ${MSVC_ML} /c /Zi ${MSVC_SAFESEH}
      ${EMBB_BASE_ASM_SOURCES}
      /Fo ${EMBB_BASE_ASM_OBJS}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

## CONFIGURATION
#
# Check headers and create configure file with preprocessor defines
include(CheckIncludeFiles) # Includes module to perform checks
include(CheckSymbolExists) # Includes module to perform symbol checks 
check_include_files("pthread_np.h" EMBB_PLATFORM_HAS_HEADER_PTHREAD_NP)
check_include_files("sys/syscall.h" EMBB_PLATFORM_HAS_HEADER_SYSCALL)
check_include_files("sys/sysinfo.h" EMBB_PLATFORM_HAS_HEADER_SYSINFO)
check_include_files("sys/param.h;sys/cpuset.h" EMBB_PLATFORM_HAS_HEADER_CPUSET)
check_symbol_exists("_SC_NPROCESSORS_ONLN" "unistd.h" EMBB_PLATFORM_HAS_SC_NPROCESSORS_ONLN)
link_libraries(${link_libraries}  ${gnu_libs})
set(CMAKE_EXTRA_INCLUDE_FILES sched.h)
  check_type_size(cpu_set_t EMBB_PLATFORM_HAS_GLIB_CPU)
set(CMAKE_EXTRA_INCLUDE_FILES)
if(DEFINED EMBB_PLATFORM_HAS_GLIB_CPU)
  add_definitions(-D_GNU_SOURCE) # Needed to activate CPU_ macros
endif()
if (USE_C11_AND_CXX11 STREQUAL ON)
  set(EMBB_PLATFORM_USE_C11_CXX11 1)
endif()

# Create header file from input file
configure_file("include/embb/base/c/internal/cmake_config.h.in" 
               "include/embb/base/c/internal/cmake_config.h")
   
# Execute the GroupSources macro
GroupSourcesMSVC(include/embb/base/c)
GroupSourcesMSVC(src)
if (BUILD_TESTS STREQUAL ON)
  GroupSourcesMSVC(test)
endif()

## BUILD TARGETS
#
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    )

add_library(embb_base_c ${EMBB_BASE_SOURCES} ${EMBB_BASE_HEADERS} 
             ${EMBB_BASE_ASM_SOURCES} ${EMBB_BASE_ASM_OBJS})
if (BUILD_SHARED_LIBS STREQUAL ON)
  if (MSVC)
    if(EMBB_INTPTR_T_TYPE_SIZE EQUAL 8)
      set_target_properties(embb_base_c PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports_64.def\"")
    else()
      set_target_properties(embb_base_c PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports_32.def\"")
    endif()
    CopyBin(BIN embb_base_c DEST ${local_install_dir})
  endif()
endif()

if (BUILD_TESTS STREQUAL ON)
  include_directories(test/ ${CMAKE_CURRENT_BINARY_DIR}/../partest/include)
  add_executable (embb_base_c_test ${EMBB_BASE_TEST_SOURCES})
  target_link_libraries(embb_base_c_test partest embb_base_c 
                        ${compiler_libs}) 
  CopyBin(BIN embb_base_c_test DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS embb_base_c EXPORT EMBB-Targets DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/embb_base_c.pdb
          DESTINATION lib
          CONFIGURATIONS Debug)
endif()
