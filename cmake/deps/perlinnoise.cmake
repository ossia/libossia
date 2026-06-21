ossia_use_system(use_sys perlinnoise)
if(use_sys)
  find_path(PERLINNOISE_INCLUDE_DIR PerlinNoise.hpp)
endif()

add_library(perlinnoise INTERFACE IMPORTED GLOBAL)
add_library(perlinnoise::perlinnoise ALIAS perlinnoise)

if(PERLINNOISE_INCLUDE_DIR)
  target_include_directories(perlinnoise INTERFACE "$<BUILD_INTERFACE:${PERLINNOISE_INCLUDE_DIR}>")
else()
  target_include_directories(perlinnoise INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/PerlinNoise>")
endif()
