add_subdirectory(img)

add_executable(stegtool_test stegtool_test.cc)
target_link_libraries(stegtool_test PRIVATE stegtool_static)
add_test(stegtool_test)
