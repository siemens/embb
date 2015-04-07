project (project_embb_algorithms)

file(GLOB_RECURSE EMBB_ALGORITHMS_CPP_SOURCES "src/*.cc" "src/*.h")
file(GLOB_RECURSE EMBB_ALGORITHMS_CPP_HEADERS "include/*.h")
file(GLOB_RECURSE EMBB_ALGORITHMS_CPP_TEST_SOURCES "test/*.cc" "test/*.h")
file(GLOB_RECURSE EMBB_ALGORITHMS_CPP_PERF_SOURCES "perf/*.cc" "perf/*.h")

# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(src)
GroupSourcesMSVC(test)
GroupSourcesMSVC(perf)

set (EMBB_ALGORITHMS_CPP_INCLUDE_DIRS "include" "src" "test" "perf")
include_directories(${EMBB_ALGORITHMS_CPP_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../mtapi_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../tasks_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../tasks_cpp/include)

add_library(embb_algorithms_cpp ${EMBB_ALGORITHMS_CPP_SOURCES}
            ${EMBB_ALGORITHMS_CPP_HEADERS})
target_link_libraries(embb_algorithms_cpp embb_tasks_cpp)

if (BUILD_TESTS STREQUAL ON)
  # Unit tests
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../partest/include)
  add_executable (embb_algorithms_cpp_test ${EMBB_ALGORITHMS_CPP_TEST_SOURCES})
  target_link_libraries(embb_algorithms_cpp_test embb_algorithms_cpp
                        embb_tasks_cpp embb_mtapi_c partest embb_base_cpp
                        embb_base_c ${compiler_libs})
  CopyBin(BIN embb_algorithms_cpp_test DEST ${local_install_dir})
  # Performance tests
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../partest/include
                      ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/perf/include)
  add_executable (embb_algorithms_cpp_perf ${EMBB_ALGORITHMS_CPP_PERF_SOURCES})
  target_link_libraries(embb_algorithms_cpp_perf embb_algorithms_cpp
                        embb_tasks_cpp embb_mtapi_c partest embb_base_cpp
                        embb_base_c embb_base_cpp_perf ${compiler_libs})
  CopyBin(BIN embb_algorithms_cpp_perf DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS embb_algorithms_cpp DESTINATION lib)
