add_executable(img_test img_test.cc)
target_link_libraries(img_test PRIVATE img_static)
AddTests(img_test)

add_executable(png_test png_test.cc)
target_link_libraries(png_test PRIVATE img_static)
AddTests(png_test)
