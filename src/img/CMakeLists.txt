include(coverage)
include(libpng)
include(libjpeg)

add_library(img_obj OBJECT img.c jpeg.c png.c)
target_include_directories(
  img_obj
  PUBLIC include
  PRIVATE ${libjpeg_INCLUDE_DIR})
set_target_properties(img_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(img_obj PRIVATE png_static libjpeg_static)

add_library(img_static STATIC)
target_link_libraries(img_static img_obj)
set_target_properties(img_static PROPERTIES OUTPUT_NAME img)

enablecoverage(img_obj)
cleancoverage(img_static)
