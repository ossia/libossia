set(SDL_BUILDING_LIBRARY TRUE)
if(CMAKE_SYSTEM_NAME MATCHES Emscripten)
  add_library(ossia_sdl2 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl2 ALIAS ossia_sdl2)

  target_compile_options(ossia_sdl2 INTERFACE "SHELL:-s USE_SDL=2")
  target_link_options(ossia_sdl2 INTERFACE "SHELL:-s USE_SDL=2")
  return()
else()
  find_package(SDL2 CONFIG GLOBAL)
endif()

if(TARGET SDL2::SDL2)
  add_library(ossia_sdl2 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl2 ALIAS ossia_sdl2)

  target_link_libraries(ossia_sdl2 INTERFACE SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
  add_library(ossia_sdl2 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl2 ALIAS ossia_sdl2)

  target_link_libraries(ossia_sdl2 INTERFACE SDL2::SDL2-static)
elseif(SDL2_LIBRARIES AND SDL2_INCLUDE_DIRS)
  add_library(ossia_sdl2 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl2 ALIAS ossia_sdl2)

  target_include_directories(ossia_sdl2 PUBLIC "${SDL2_INCLUDE_DIRS}")
  target_link_libraries(ossia_sdl2 INTERFACE "${SDL2_LIBRARIES}")
endif()
