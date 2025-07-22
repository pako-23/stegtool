foreach(FILE empty unsupported-format cat.jpg invalid-magic.jpg cat.png invalid-magic.png)
  file(COPY ${FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_executable(img_test img_test.cc)
target_link_libraries(img_test PRIVATE img_static)
AddTests(img_test)