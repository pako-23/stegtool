include(coverage)
include(libpng)

add_library(img_obj OBJECT img.c png.c ppm.c)
target_include_directories(img_obj PUBLIC include)
set_target_properties(img_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(img_obj PRIVATE png_static)

add_library(img_static STATIC)
target_link_libraries(img_static img_obj)
set_target_properties(img_static PROPERTIES OUTPUT_NAME img)

enable_coverage(img_obj)
clean_coverage(img_static)
