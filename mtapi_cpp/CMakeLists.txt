project (project_mtapi_cpp)

file(GLOB_RECURSE EMBB_MTAPI_CPP_SOURCES "src/*.cc" "src/*.h")
file(GLOB_RECURSE EMBB_MTAPI_CPP_HEADERS "include/*.h")
file(GLOB_RECURSE EMBB_MTAPI_CPP_TEST_SOURCES "test/*.cc" "test/*.h")

if (USE_AUTOMATIC_INITIALIZATION STREQUAL ON)
  set(MTAPI_CPP_AUTOMATIC_INITIALIZE 1)
else()
  set(MTAPI_CPP_AUTOMATIC_INITIALIZE 0)
endif()

configure_file("include/embb/mtapi/internal/cmake_config.h.in"
               "include/embb/mtapi/internal/cmake_config.h")

# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(src)
GroupSourcesMSVC(test)

set (EMBB_MTAPI_CPP_INCLUDE_DIRS "include" "src" "test")
include_directories(${EMBB_MTAPI_CPP_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../mtapi_c/include)

add_library (embb_mtapi_cpp ${EMBB_MTAPI_CPP_SOURCES} ${EMBB_MTAPI_CPP_HEADERS})
target_link_libraries(embb_mtapi_cpp embb_mtapi_c)
if (BUILD_SHARED_LIBS STREQUAL ON)
  if (MSVC)
    if(EMBB_INTPTR_T_TYPE_SIZE EQUAL 8)
      set_target_properties(embb_mtapi_cpp PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports_64.def\"")
    else()
      set_target_properties(embb_mtapi_cpp PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports_32.def\"")
    endif()
    CopyBin(BIN embb_mtapi_cpp DEST ${local_install_dir})
  endif()
endif()

if (BUILD_TESTS STREQUAL ON)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../partest/include)
  add_executable (embb_mtapi_cpp_test ${EMBB_MTAPI_CPP_TEST_SOURCES})
  target_link_libraries(embb_mtapi_cpp_test embb_mtapi_cpp embb_mtapi_c partest 
                        embb_base_cpp embb_base_c ${compiler_libs})
  CopyBin(BIN embb_mtapi_cpp_test DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS embb_mtapi_cpp EXPORT EMBB-Targets DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/embb_mtapi_cpp.pdb
          DESTINATION lib
          CONFIGURATIONS Debug)
endif()
