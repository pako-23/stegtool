add_executable(png_test png_test.cc)
target_link_libraries(png_test PRIVATE img_static)
AddTests(png_test)
