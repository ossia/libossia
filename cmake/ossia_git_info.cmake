
function(get_commit_date _var)
  if(NOT GIT_FOUND)
    find_package(Git QUIET)
  endif()
  if(NOT GIT_FOUND)
    set(${_var} "GIT-NOTFOUND" PARENT_SCOPE)
    return()
  endif()

  execute_process(COMMAND
    "${GIT_EXECUTABLE}"
    log --format=%cd -1 --date=short
    WORKING_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE
    res
    OUTPUT_VARIABLE
    out
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT res EQUAL 0)
    set(out "${out}-${res}-NOTFOUND")
  endif()

  set(${_var} "${out}" PARENT_SCOPE)
endfunction()

function(get_commit_SHA _var)
  if(NOT GIT_FOUND)
    find_package(Git QUIET)
  endif()
  if(NOT GIT_FOUND)
    set(${_var} "GIT-NOTFOUND" PARENT_SCOPE)
    return()
  endif()

  # TODO sanitize
  #if((${ARGN}" MATCHES "&&") OR
  # (ARGN MATCHES "||") OR
  # (ARGN MATCHES "\\;"))
  # message("Please report the following error to the project!")
  # message(FATAL_ERROR "Looks like someone's doing something nefarious with git_describe! Passed arguments ${ARGN}")
  #endif()

  #message(STATUS "Arguments to execute_process: ${ARGN}")

  execute_process(COMMAND
    "${GIT_EXECUTABLE}"
    log -1 --format=%h
    WORKING_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE
    res
    OUTPUT_VARIABLE
    out
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT res EQUAL 0)
    set(out "${out}-${res}-NOTFOUND")
  endif()

  set(${_var} "${out}" PARENT_SCOPE)
endfunction()
