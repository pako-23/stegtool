include(coverage)

add_subdirectory(img)

add_library(stegtool_obj OBJECT stegtool.c)
target_include_directories(stegtool_obj PUBLIC include)
set_target_properties(stegtool_obj PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

add_library(stegtool_static STATIC)
target_link_libraries(stegtool_static stegtool_obj)
set_target_properties(stegtool_static PROPERTIES OUTPUT_NAME stegtool)

if(NOT WIN32)
  add_library(stegtool_shared SHARED)
  target_link_libraries(stegtool_shared img_obj)
  set_target_properties(stegtool_shared PROPERTIES OUTPUT_NAME stegtool)
endif()

EnableCoverage(stegtool_obj)
CleanCoverage(stegtool_static)
