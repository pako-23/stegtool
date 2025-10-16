file(GLOB_RECURSE test_images ${CMAKE_CURRENT_SOURCE_DIR}/*.png
     ${CMAKE_CURRENT_SOURCE_DIR}/*.jpg)

foreach(file empty unsupported-format ${test_images})
  file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_executable(img_test img/img_test.cc)
target_link_libraries(img_test PRIVATE img_static)
add_test(img_test)

add_executable(png_test img/png_test.cc)
target_link_libraries(png_test PRIVATE img_static)
add_test(png_test)

add_executable(jpeg_test img/jpeg_test.cc)
target_link_libraries(jpeg_test PRIVATE img_static)
add_test(jpeg_test)

add_executable(stegtool_test stegtool_test.cc)
target_link_libraries(stegtool_test PRIVATE stegtool_static)
add_test(stegtool_test)

add_executable(stegtool_run_test stegtool_run_test.cc)
target_link_libraries(stegtool_run_test PRIVATE stegtool_run)
add_test(stegtool_run_test)
