set(SDL_BUILDING_LIBRARY TRUE)
if(CMAKE_SYSTEM_NAME MATCHES Emscripten)
  add_library(ossia_sdl3 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl3 ALIAS ossia_sdl3)

  target_compile_options(ossia_sdl3 INTERFACE "SHELL:-s USE_SDL=3")
  target_link_options(ossia_sdl3 INTERFACE "SHELL:-s USE_SDL=3")
  return()
else()
  find_package(SDL3 CONFIG GLOBAL)
endif()

if(TARGET SDL3::SDL3)
  add_library(ossia_sdl3 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl3 ALIAS ossia_sdl3)

  target_link_libraries(ossia_sdl3 INTERFACE SDL3::SDL3)
elseif(TARGET SDL3::SDL3-static)
  add_library(ossia_sdl3 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl3 ALIAS ossia_sdl3)

  target_link_libraries(ossia_sdl3 INTERFACE SDL3::SDL3-static)
elseif(SDL3_LIBRARIES AND SDL3_INCLUDE_DIRS)
  add_library(ossia_sdl3 INTERFACE IMPORTED GLOBAL)
  add_library(ossia::sdl3 ALIAS ossia_sdl3)

  target_include_directories(ossia_sdl3 PUBLIC "${SDL3_INCLUDE_DIRS}")
  target_link_libraries(ossia_sdl3 INTERFACE "${SDL3_LIBRARIES}")
endif()
