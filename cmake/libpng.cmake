include(zlib)

FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng
  GIT_TAG v1.6.50)

set(PNG_SHARED
    OFF
    CACHE BOOL "" FORCE)
set(PNG_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(ZLIB_INCLUDE_DIRS
    ${ZLIB_INCLUDE_DIR}
    CACHE INTERNAL "" FORCE)
set(SKIP_INSTALL_CONFIG_FILE
    ON
    CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpng)

set_target_properties(png_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
