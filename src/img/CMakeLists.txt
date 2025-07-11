add_library(img_obj OBJECT file.c)
target_include_directories(img_obj PUBLIC include)
set_target_properties(img_obj PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

add_library(img_static STATIC)
target_link_libraries(img_static img_obj)
set_target_properties(img_static PROPERTIES OUTPUT_NAME img)

add_library(img_shared SHARED)
target_link_libraries(img_shared img_obj)
set_target_properties(img_shared PROPERTIES OUTPUT_NAME img)
