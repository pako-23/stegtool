include(coverage)

FetchContent_Declare(
  ZLIB
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG        v1.3.1
)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ZLIB)

set(ZLIB_ROOT "${zlib_BINARY_DIR}" CACHE STRING "" FORCE)
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR};${zlib_BINARY_DIR}"
  CACHE STRING "" FORCE)

if(WIN32)
  set(ZLIB_LIBRARY "${zlib_BINARY_DIR}/zlibstatic.lib" CACHE STRING "" FORCE)
else()
  set(ZLIB_LIBRARY "${zlib_BINARY_DIR}/zlib.a" CACHE STRING "" FORCE)
endif()

set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)

FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng
  GIT_TAG        v1.6.50
)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpng)

set_target_properties(png_static PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(img_obj OBJECT img.c jpeg.c png.c)
target_include_directories(img_obj PUBLIC include)
set_target_properties(img_obj PROPERTIES
  POSITION_INDEPENDENT_CODE ON)
target_link_libraries(img_obj PRIVATE zlibstatic png_static)

add_library(img_static STATIC)
target_link_libraries(img_static img_obj)
set_target_properties(img_static PROPERTIES OUTPUT_NAME img)

if(WIN32)
  target_link_libraries(img_obj PRIVATE ws2_32)
else()
  add_library(img_shared SHARED)
  target_link_libraries(img_shared img_obj)
  set_target_properties(img_shared PROPERTIES OUTPUT_NAME img)
endif()

EnableCoverage(img_obj)
CleanCoverage(img_static)
