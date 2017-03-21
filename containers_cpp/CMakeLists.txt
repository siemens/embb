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
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include)

add_library (embb_containers_cpp STATIC ${EMBB_CONTAINERS_CPP_SOURCES} ${EMBB_CONTAINERS_CPP_HEADERS})
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
install(TARGETS embb_containers_cpp EXPORT EMBB-Targets DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/embb_containers_cpp.pdb
          DESTINATION lib
          CONFIGURATIONS Debug)
endif()
