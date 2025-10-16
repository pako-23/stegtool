set(libjpeg_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/libjpeg-turbo)
set(libjpeg_BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/libjpeg-turbo-build)
set(libjpeg_INCLUDE_DIR ${libjpeg_SOURCE_DIR}/src ${libjpeg_BINARY_DIR})

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -DENABLE_STATIC=ON -DENABLE_SHARED=OFF -DENABLE_SHARED=OFF
    -DWITH_TESTS=OFF -S ${libjpeg_SOURCE_DIR} -B ${libjpeg_BINARY_DIR})

set(_libjpeg_location)
if(MSVC)
  set(_libjpeg_location ${libjpeg_BINARY_DIR}/jpeg-static.lib)
else()
  set(_libjpeg_location ${libjpeg_BINARY_DIR}/libjpeg.a)
endif()

add_custom_target(
  libjpeg_build ALL
  COMMAND ${CMAKE_COMMAND} --build ${libjpeg_BINARY_DIR}
  COMMENT "Built target libjpeg")

add_library(libjpeg_static STATIC IMPORTED)
add_dependencies(libjpeg_static libjpeg_build)
set_target_properties(libjpeg_static PROPERTIES IMPORTED_LOCATION
                                                ${_libjpeg_location})
