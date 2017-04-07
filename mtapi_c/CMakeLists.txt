project (project_embb_mtapi_c)

file(GLOB_RECURSE EMBB_MTAPI_C_SOURCES "src/*.c" "src/*.h")
file(GLOB_RECURSE EMBB_MTAPI_C_HEADERS "include/*.h")

file(GLOB_RECURSE EMBB_MTAPI_TEST_SOURCES "test/*.cc" "test/*.h")
  
IF(MSVC8 OR MSVC9 OR MSVC10 OR MSVC11)
FOREACH(src_tmp ${EMBB_MTAPI_TEST_SOURCES})
    SET_PROPERTY(SOURCE ${src_tmp} PROPERTY LANGUAGE CXX)
ENDFOREACH(src_tmp)
FOREACH(src_tmp ${EMBB_MTAPI_C_SOURCES})
    SET_PROPERTY(SOURCE ${src_tmp} PROPERTY LANGUAGE CXX)
ENDFOREACH(src_tmp)
ENDIF()
   
# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(src)
GroupSourcesMSVC(test)

set (EMBB_MTAPI_INCLUDE_DIRS "include" "src" "test")
include_directories(${EMBB_MTAPI_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    )

add_library(embb_mtapi_c ${EMBB_MTAPI_C_SOURCES} ${EMBB_MTAPI_C_HEADERS})
target_link_libraries(embb_mtapi_c embb_base_c)
if (BUILD_SHARED_LIBS STREQUAL ON)
  if (MSVC)
    set_target_properties(embb_mtapi_c PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/exports.def\"")
    CopyBin(BIN embb_mtapi_c DEST ${local_install_dir})
  endif()
endif()

if (BUILD_TESTS STREQUAL ON)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../partest/include)
  add_executable (embb_mtapi_c_test ${EMBB_MTAPI_TEST_SOURCES})
  target_link_libraries(embb_mtapi_c_test embb_mtapi_c partest embb_base_c ${compiler_libs})
  CopyBin(BIN embb_mtapi_c_test DEST ${local_install_dir})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS embb_mtapi_c EXPORT EMBB-Targets DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/embb_mtapi_c.pdb
          DESTINATION lib
          CONFIGURATIONS Debug)
endif()
