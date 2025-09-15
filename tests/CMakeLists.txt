add_subdirectory(img)

add_executable(stegtool_test stegtool_test.cc)
target_link_libraries(stegtool_test PRIVATE stegtool_static)
AddTests(stegtool_test)
