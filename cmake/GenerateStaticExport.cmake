# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
# TODO: Install this macro separately?
macro(_check_cxx_compiler_attribute _ATTRIBUTE _RESULT)
  check_cxx_source_compiles("${_ATTRIBUTE} int somefunc() { return 0; }
    int main() { return somefunc();}" ${_RESULT}
  )
endmacro()

# TODO: Install this macro separately?
macro(_check_c_compiler_attribute _ATTRIBUTE _RESULT)
  check_c_source_compiles("${_ATTRIBUTE} int somefunc() { return 0; }
    int main() { return somefunc();}" ${_RESULT}
  )
endmacro()

macro(_test_compiler_hidden_visibility)
  if(NOT WIN32 AND NOT CYGWIN)
      check_cxx_compiler_flag(-fvisibility=hidden COMPILER_HAS_HIDDEN_VISIBILITY)
      check_cxx_compiler_flag(-fvisibility-inlines-hidden
        COMPILER_HAS_HIDDEN_INLINE_VISIBILITY)
  endif()
endmacro()

get_filename_component(_GENERATE_EXPORT_HEADER_MODULE_DIR
  "${CMAKE_CURRENT_LIST_FILE}" PATH)

macro(_DO_SET_MACRO_VALUES TARGET_LIBRARY)
  set(DEFINE_DEPRECATED "[[deprecated]]")
  set(DEFINE_EXPORT)
  set(DEFINE_IMPORT)
  set(DEFINE_NO_EXPORT)

  get_property(type TARGET ${TARGET_LIBRARY} PROPERTY TYPE)

  if(NOT ${type} STREQUAL "STATIC_LIBRARY" OR "${_GEH_ALWAYS_EXPORT}")
    if(WIN32 OR CYGWIN)
      set(DEFINE_EXPORT "__declspec(dllexport)")
      set(DEFINE_EXPORT_EXTERN_TEMPLATE "__declspec(dllexport)")
      set(DEFINE_EXPORT_INSTANTIATE_TEMPLATE "__declspec(dllexport)")
      set(DEFINE_IMPORT "")
    elseif(COMPILER_HAS_HIDDEN_VISIBILITY)
      set(DEFINE_EXPORT "__attribute__((visibility(\"default\")))")
      set(DEFINE_EXPORT_EXTERN_TEMPLATE "__attribute__((visibility(\"default\")))")
      set(DEFINE_EXPORT_INSTANTIATE_TEMPLATE "__attribute__((visibility(\"default\")))")
      set(DEFINE_IMPORT "__attribute__((visibility(\"default\")))")
      set(DEFINE_NO_EXPORT "__attribute__((visibility(\"hidden\")))")
    endif()
  endif()
endmacro()


function(GENERATE_EXPORT_HEADER TARGET_LIBRARY)
  # Option overrides
  set(options DEFINE_NO_DEPRECATED ALWAYS_EXPORT)
  set(oneValueArgs PREFIX_NAME BASE_NAME EXPORT_MACRO_NAME EXPORT_FILE_NAME
    DEPRECATED_MACRO_NAME NO_EXPORT_MACRO_NAME STATIC_DEFINE
    NO_DEPRECATED_MACRO_NAME CUSTOM_CONTENT_FROM_VARIABLE INCLUDE_GUARD_NAME)
  set(multiValueArgs)

  cmake_parse_arguments(_GEH "${options}" "${oneValueArgs}" "${multiValueArgs}"
    ${ARGN})

  _test_compiler_hidden_visibility()
  _do_set_macro_values(${TARGET_LIBRARY})

  set(BASE_NAME "${TARGET_LIBRARY}")

  if(_GEH_BASE_NAME)
    set(BASE_NAME ${_GEH_BASE_NAME})
  endif()

  string(TOUPPER ${BASE_NAME} BASE_NAME_UPPER)
  string(TOLOWER ${BASE_NAME} BASE_NAME_LOWER)

  # Default options
  set(EXPORT_MACRO_NAME "${_GEH_PREFIX_NAME}${BASE_NAME_UPPER}_EXPORT")
  set(NO_EXPORT_MACRO_NAME "${_GEH_PREFIX_NAME}${BASE_NAME_UPPER}_NO_EXPORT")
  set(EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME_LOWER}_export.h")
  set(DEPRECATED_MACRO_NAME "${_GEH_PREFIX_NAME}${BASE_NAME_UPPER}_DEPRECATED")
  set(STATIC_DEFINE "${_GEH_PREFIX_NAME}${BASE_NAME_UPPER}_STATIC_DEFINE")
  set(NO_DEPRECATED_MACRO_NAME
    "${_GEH_PREFIX_NAME}${BASE_NAME_UPPER}_NO_DEPRECATED")

  if(_GEH_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to GENERATE_EXPORT_HEADER(): \"${_GEH_UNPARSED_ARGUMENTS}\"")
  endif()

  if(_GEH_EXPORT_MACRO_NAME)
    set(EXPORT_MACRO_NAME ${_GEH_PREFIX_NAME}${_GEH_EXPORT_MACRO_NAME})
  endif()
  string(MAKE_C_IDENTIFIER ${EXPORT_MACRO_NAME} EXPORT_MACRO_NAME)
  if(_GEH_EXPORT_FILE_NAME)
    if(IS_ABSOLUTE ${_GEH_EXPORT_FILE_NAME})
      set(EXPORT_FILE_NAME ${_GEH_EXPORT_FILE_NAME})
    else()
      set(EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${_GEH_EXPORT_FILE_NAME}")
    endif()
  endif()
  if(_GEH_DEPRECATED_MACRO_NAME)
    set(DEPRECATED_MACRO_NAME ${_GEH_PREFIX_NAME}${_GEH_DEPRECATED_MACRO_NAME})
  endif()
  string(MAKE_C_IDENTIFIER ${DEPRECATED_MACRO_NAME} DEPRECATED_MACRO_NAME)
  if(_GEH_NO_EXPORT_MACRO_NAME)
    set(NO_EXPORT_MACRO_NAME ${_GEH_PREFIX_NAME}${_GEH_NO_EXPORT_MACRO_NAME})
  endif()
  string(MAKE_C_IDENTIFIER ${NO_EXPORT_MACRO_NAME} NO_EXPORT_MACRO_NAME)
  if(_GEH_STATIC_DEFINE)
    set(STATIC_DEFINE ${_GEH_PREFIX_NAME}${_GEH_STATIC_DEFINE})
  endif()
  string(MAKE_C_IDENTIFIER ${STATIC_DEFINE} STATIC_DEFINE)

  if(_GEH_DEFINE_NO_DEPRECATED)
    set(DEFINE_NO_DEPRECATED 1)
  else()
    set(DEFINE_NO_DEPRECATED 0)
  endif()

  if(_GEH_NO_DEPRECATED_MACRO_NAME)
    set(NO_DEPRECATED_MACRO_NAME
      ${_GEH_PREFIX_NAME}${_GEH_NO_DEPRECATED_MACRO_NAME})
  endif()
  string(MAKE_C_IDENTIFIER ${NO_DEPRECATED_MACRO_NAME} NO_DEPRECATED_MACRO_NAME)

  if(_GEH_INCLUDE_GUARD_NAME)
    set(INCLUDE_GUARD_NAME ${_GEH_INCLUDE_GUARD_NAME})
  else()
    set(INCLUDE_GUARD_NAME "${EXPORT_MACRO_NAME}_H")
  endif()

  get_target_property(EXPORT_IMPORT_CONDITION ${TARGET_LIBRARY} DEFINE_SYMBOL)

  if(NOT EXPORT_IMPORT_CONDITION)
    set(EXPORT_IMPORT_CONDITION ${TARGET_LIBRARY}_EXPORTS)
  endif()
  string(MAKE_C_IDENTIFIER ${EXPORT_IMPORT_CONDITION} EXPORT_IMPORT_CONDITION)

  if(_GEH_CUSTOM_CONTENT_FROM_VARIABLE)
    if(DEFINED "${_GEH_CUSTOM_CONTENT_FROM_VARIABLE}")
      set(CUSTOM_CONTENT "${${_GEH_CUSTOM_CONTENT_FROM_VARIABLE}}")
    else()
      set(CUSTOM_CONTENT "")
    endif()
  endif()

  configure_file("${_GENERATE_EXPORT_HEADER_MODULE_DIR}/exportheader.cmake.in"
    "${EXPORT_FILE_NAME}" @ONLY)

endfunction()
