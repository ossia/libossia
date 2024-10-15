if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(magic_enum CONFIG GLOBAL)
endif()

if(NOT TARGET magic_enum::magic_enum)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/magic_enum" magic_enum)
endif()

