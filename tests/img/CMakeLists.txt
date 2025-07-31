foreach(FILE empty unsupported-format cat.jpg invalid-magic.jpg cat.png invalid-magic.png)
  file(COPY ${FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_executable(img_test img_test.cc)
target_link_libraries(img_test PRIVATE img_static)
AddTests(img_test)

add_executable(png_test png_test.cc)
target_link_libraries(png_test PRIVATE img_static)
AddTests(png_test)

add_executable(jpeg_test jpeg_test.cc)
target_link_libraries(jpeg_test PRIVATE img_static)
AddTests(jpeg_test)
