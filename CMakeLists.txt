cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(stegtool
  VERSION 0.0.1
  LANGUAGES C CXX
  DESCRIPTION "An image steganography tool")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(no-insource-builds)

option(stegtool_BUILD_TESTS "Build test programs" ON)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

if(MSVC)
  add_compile_options(/W4 "$<$<CONFIG:Debug>:/Zi>")
else()
  add_compile_options("$<$<CONFIG:Debug>:-g3>" -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)

if(stegtool_BUILD_TESTS)
  include(testing)
  add_subdirectory(tests)
endif()
