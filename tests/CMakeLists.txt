foreach(file empty unsupported-format cat.jpg invalid-magic.jpg cat.png
             invalid-magic.png)
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
