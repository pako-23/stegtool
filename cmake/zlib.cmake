if(WIN32)
  FetchContent_Declare(
    ZLIB
    GIT_REPOSITORY https://github.com/madler/zlib
    GIT_TAG        v1.3.1
    OVERRIDE_FIND_PACKAGE
  )
  set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(ZLIB)

  set(ZLIB_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
  set(ZLIB_ROOT ${zlib_BINARY_DIR} CACHE INTERNAL "" FORCE)
  set(ZLIB_LIBRARY zlibstatic CACHE INTERNAL "" FORCE)
  set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} CACHE INTERNAL "" FORCE)
  set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} CACHE INTERNAL "" FORCE)

  include_directories(${ZLIB_INCLUDE_DIR})
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()
