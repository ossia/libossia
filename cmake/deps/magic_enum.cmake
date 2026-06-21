ossia_use_system(use_sys magic_enum)
if(use_sys)
  find_package(magic_enum CONFIG GLOBAL)
endif()

if(NOT TARGET magic_enum::magic_enum)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/magic_enum" magic_enum)
endif()

