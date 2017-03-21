project (project_embb_mtapi_cuda_c)

file(GLOB_RECURSE EMBB_MTAPI_CUDA_C_SOURCES "src/*.c" "src/*.h")
file(GLOB_RECURSE EMBB_MTAPI_CUDA_C_HEADERS "include/*.h")

file(GLOB_RECURSE EMBB_MTAPI_CUDA_TEST_SOURCES "test/*.cc" "test/*.cu" "test/*.h")

IF(MSVC8 OR MSVC9 OR MSVC10 OR MSVC11)
FOREACH(src_tmp ${EMBB_MTAPI_CUDA_TEST_SOURCES})
    SET_PROPERTY(SOURCE ${src_tmp} PROPERTY LANGUAGE CXX)
ENDFOREACH(src_tmp)
FOREACH(src_tmp ${EMBB_MTAPI_CUDA_C_SOURCES})
    SET_PROPERTY(SOURCE ${src_tmp} PROPERTY LANGUAGE CXX)
ENDFOREACH(src_tmp)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCC)
  set (EMBB_MTAPI_CUDA_C_LIBS dl)
ENDIF()

# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(src)
GroupSourcesMSVC(test)

set (EMBB_MTAPI_CUDA_INCLUDE_DIRS "include" "src" "test")
include_directories(${EMBB_MTAPI_CUDA_INCLUDE_DIRS}
                    ${CUDA_TOOLKIT_INCLUDE}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../mtapi_c/src
                    )

add_library(embb_mtapi_cuda_c ${EMBB_MTAPI_CUDA_C_SOURCES} ${EMBB_MTAPI_CUDA_C_HEADERS})
target_link_libraries(embb_mtapi_cuda_c embb_mtapi_c embb_base_c ${CUDA_CUDA_LIBRARY})
if (BUILD_SHARED_LIBS STREQUAL ON)
  if (MSVC)
    set_target_properties(embb_mtapi_cuda_c PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports.def\"")
    CopyBin(BIN embb_mtapi_cuda_c DEST ${local_install_dir})
  endif()
endif()

if (BUILD_TESTS STREQUAL ON)
  add_custom_command(
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/embb_mtapi_cuda_test_kernel.cu"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.ptx"
    COMMAND ${CUDA_NVCC_EXECUTABLE} -ptx -m 32
      "${CMAKE_CURRENT_SOURCE_DIR}/test/embb_mtapi_cuda_test_kernel.cu"
      -o "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.ptx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  add_custom_command(
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.ptx"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.h"
    COMMAND ${CUDA_TOOLKIT_ROOT_DIR}/bin/bin2c -p 0
      "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.ptx" >
      "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../../partest/include)
  add_executable (embb_mtapi_cuda_c_test ${EMBB_MTAPI_CUDA_TEST_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/embb_mtapi_cuda_test_kernel.h")
  target_link_libraries(embb_mtapi_cuda_c_test embb_mtapi_cuda_c embb_mtapi_c partest embb_base_c ${compiler_libs} ${EMBB_MTAPI_CUDA_C_LIBS} ${CUDA_CUDA_LIBRARY})
  CopyBin(BIN embb_mtapi_cuda_c_test DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS embb_mtapi_cuda_c EXPORT EMBB-Targets DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/embb_mtapi_cuda_c.pdb
          DESTINATION lib
          CONFIGURATIONS Debug)
endif()
