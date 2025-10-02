find_program(GCOV gcov OPTIONAL)
find_program(LCOV lcov OPTIONAL)
find_program(GENHTML genhtml OPTIONAL)

set(WITH_COVERAGE ON)
if(NOT GCOV OR NOT LCOV OR NOT GENHTML)
  message(WARNING "Disabling coverage")
  set(WITH_COVERAGE OFF)
endif()

function(EnableCoverage target)
  if(WITH_COVERAGE)
    target_compile_options(${target}
      PRIVATE --coverage -fno-inline -fprofile-update=atomic
    )
    target_link_libraries(${target} PRIVATE gcov)
  endif()
endfunction()

function(CleanCoverage target)
  if(WITH_COVERAGE)
    add_custom_command(TARGET ${target} PRE_BUILD COMMAND
      find ${CMAKE_BINARY_DIR} -type f -name '*.gcda' -delete
    )
  endif()
endfunction()

if(WITH_COVERAGE AND NOT TARGET coverage)
  add_custom_target(coverage
    COMMAND ${LCOV} -d . --zerocounters
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    COMMAND ${LCOV} -d . --capture -o coverage.info
    COMMAND ${LCOV} -r coverage.info '/usr/include/*'
     -o filtered.info
     --ignore-errors unused version
    COMMAND ${GENHTML} -o coverage filtered.info --legend
    COMMAND rm -rf coverage.info filtered.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

function(AddCoverage target)
  if(WITH_COVERAGE)
    add_custom_target(coverage-${target}
      COMMAND ${LCOV} -d . --zerocounters
      COMMAND $<TARGET_FILE:${target}>
      COMMAND ${LCOV} -d . --capture -o coverage.info
      COMMAND ${LCOV} -r coverage.info '/usr/include/*'
        -o filtered.info
        --ignore-errors unused version
      COMMAND ${GENHTML} -o coverage-${target} filtered.info --legend
      COMMAND rm -rf coverage.info filtered.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  endif()
endfunction()
