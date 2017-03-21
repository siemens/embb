project (project_embb_base_cpp)

## CODE FILE DETECTION
#
# Fetch all header and source files for C, C++, and test build separately
file(GLOB_RECURSE EMBB_BASE_CPP_SOURCES "src/*.cc" "src/*.h")
file(GLOB_RECURSE EMBB_BASE_CPP_HEADERS "include/embb/*.h")
if (BUILD_TESTS STREQUAL ON)
  file(GLOB_RECURSE EMBB_BASE_TEST_SOURCES "test/*.cc" "test/*.h")
endif()

# Create header file from input file
configure_file("include/embb/base/internal/cmake_config.h.in" 
               "include/embb/base/internal/cmake_config.h")

# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include/embb/base)
GroupSourcesMSVC(src)
if (BUILD_TESTS STREQUAL ON)
  GroupSourcesMSVC(test)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    )

## BUILD TARGETS
#
add_library (embb_base_cpp ${EMBB_BASE_CPP_SOURCES} ${EMBB_BASE_CPP_HEADERS})
target_link_libraries(embb_base_cpp embb_base_c)
if (BUILD_SHARED_LIBS STREQUAL ON)
  if (MSVC)
    if(EMBB_INTPTR_T_TYPE_SIZE EQUAL 8)
      set_target_properties(embb_base_cpp PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports_64.def\"")
    else()
      set_target_properties(embb_base_cpp PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports_32.def\"")
    endif()
    CopyBin(BIN embb_base_cpp DEST ${local_install_dir})
  endif()
endif()

if (BUILD_TESTS STREQUAL ON)
  include_directories(test/
                      ${CMAKE_CURRENT_BINARY_DIR}/../partest/include
                      )
  add_executable (embb_base_cpp_test ${EMBB_BASE_TEST_SOURCES})
  target_link_libraries(embb_base_cpp_test partest embb_base_cpp embb_base_c
                        ${compiler_libs})
  CopyBin(BIN embb_base_cpp_test DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS embb_base_cpp EXPORT EMBB-Targets DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/embb_base_cpp.pdb
          DESTINATION lib
          CONFIGURATIONS Debug)
endif()
