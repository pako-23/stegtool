find_program(VALGRIND valgrind OPTIONAL)

set(WITH_MEMCHECK ON)
if(NOT VALGRIND)
  message(WARNING "Disabling memcheck")
  set(WITH_MEMCHECK OFF)
endif()

if(WITH_MEMCHECK)
  FetchContent_Declare(memcheck-cover
    GIT_REPOSITORY https://github.com/Farigh/memcheck-cover.git
    GIT_TAG        release-1.2)
  FetchContent_MakeAvailable(memcheck-cover)

  set(MEMCHECK_REPORTER ${memcheck-cover_SOURCE_DIR}/bin)

  if(NOT TARGET memcheck)
    set(MEMCHECK_REPORT ${CMAKE_BINARY_DIR}/memcheck)
    file(MAKE_DIRECTORY ${MEMCHECK_REPORT})
    add_custom_target(memcheck
      COMMAND ${MEMCHECK_REPORTER}/generate_html_report.sh
        -i ${MEMCHECK_REPORT}
        -o ${MEMCHECK_REPORT}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  endif()
endif()


function(AddMemcheck target)
  if (WITH_MEMCHECK)
    add_custom_target(memcheck-${target}
      COMMAND ${MEMCHECK_REPORTER}/memcheck_runner.sh
        -o ${MEMCHECK_REPORT}/${target} -- $<TARGET_FILE:${target}>
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    add_dependencies(memcheck memcheck-${target})
  endif()
endfunction()
