if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(rapidfuzz CONFIG REQUIRED GLOBAL)
endif()

if(NOT TARGET rapidfuzz::rapidfuzz)
  add_library(rapidfuzz INTERFACE IMPORTED GLOBAL)
  add_library(rapidfuzz::rapidfuzz ALIAS rapidfuzz)
  target_include_directories(rapidfuzz INTERFACE "${OSSIA_3RDPARTY_FOLDER}/rapidfuzz-cpp")
endif()

set(OSSIA_HAS_RAPIDFUZZ 1 CACHE INTERNAL "")
