set(_libjpeg_prefix ${CMAKE_BINARY_DIR}/_deps)
set(libjpeg_SOURCE_DIR ${_libjpeg_prefix}/libjpeg-turbo-src)
set(libjpeg_BINARY_DIR ${_libjpeg_prefix}/libjpeg-turbo-build)
set(libjpeg_INCLUDE_DIR ${libjpeg_SOURCE_DIR}/src ${libjpeg_BINARY_DIR})

ExternalProject_Add(
  libjpeg
  GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
  GIT_TAG 3.1.2
  PREFIX ${_libjpeg_prefix}
  SOURCE_DIR ${libjpeg_SOURCE_DIR}
  BINARY_DIR ${libjpeg_BINARY_DIR}
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -DENABLE_STATIC=ON -DENABLE_SHARED=OFF
                    ${libjpeg_SOURCE_DIR}
  BUILD_COMMAND ${CMAKE_COMMAND} --build ${libjpeg_BINARY_DIR} --target
                jpeg-static
  INSTALL_COMMAND ""
  STEP_TARGETS libjpeg)

add_library(libjpeg_static STATIC IMPORTED)
if(MSVC)
  set_target_properties(libjpeg_static PROPERTIES IMPORTED_LOCATION
    ${libjpeg_BINARY_DIR}/Debug/jpeg-static.lib)
else()
  set_target_properties(libjpeg_static PROPERTIES IMPORTED_LOCATION
    ${libjpeg_BINARY_DIR}/libjpeg.a)
endif()

add_dependencies(libjpeg_static libjpeg)
