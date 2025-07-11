cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(stegtool
  VERSION 0.0.1
  LANGUAGES C
  DESCRIPTION "An image steganography tool")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(no-insource-builds)

add_subdirectory(src)
