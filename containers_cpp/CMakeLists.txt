project (project_embb_containers_cpp)

file(GLOB_RECURSE EMBB_CONTAINERS_CPP_SOURCES "src/*.cc" "src/*.h")
file(GLOB_RECURSE EMBB_CONTAINERS_CPP_HEADERS "include/*.h")
file(GLOB_RECURSE EMBB_CONTAINERS_CPP_TEST_SOURCES "test/*.cc" "test/*.h")
   
# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(src)
GroupSourcesMSVC(test)

set (EMBB_CONTAINERS_CPP_INCLUDE_DIRS "include" "src" "test")
include_directories(${EMBB_CONTAINERS_CPP_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include)

add_subdirectory(generator)
set(GENERATOR_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/embb/containers/internal)
set(CHROMATIC_TREE_GENERATED_HEADER ${GENERATOR_OUTPUT_DIR}/lock_free_chromatic_tree-rebalance.h)

file(MAKE_DIRECTORY ${GENERATOR_OUTPUT_DIR})
add_custom_command (
  OUTPUT ${CHROMATIC_TREE_GENERATED_HEADER}
  COMMAND chromatic_tree_generator ${CHROMATIC_TREE_GENERATED_HEADER}
  DEPENDS chromatic_tree_generator
)

add_library (embb_containers_cpp ${EMBB_CONTAINERS_CPP_SOURCES} ${EMBB_CONTAINERS_CPP_HEADERS}
                                 ${CHROMATIC_TREE_GENERATED_HEADER})
target_link_libraries(embb_containers_cpp embb_base_cpp)

if (BUILD_TESTS STREQUAL ON)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../partest/include)
  add_executable (embb_containers_cpp_test ${EMBB_CONTAINERS_CPP_TEST_SOURCES})
  target_link_libraries(embb_containers_cpp_test embb_containers_cpp 
                        partest embb_base_cpp embb_base_c ${compiler_libs})
  CopyBin(BIN embb_containers_cpp_test DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/embb
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(FILES ${CHROMATIC_TREE_GENERATED_HEADER} DESTINATION include/embb/containers/internal)
install(TARGETS embb_containers_cpp DESTINATION lib)
