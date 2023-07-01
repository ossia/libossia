if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_path(NANOSIGNAL_INCLUDE_DIR nano_observer.hpp)
endif()

add_library(nanosignal INTERFACE IMPORTED GLOBAL)
add_library(nanosignal::nanosignal ALIAS nanosignal)

if(NANOSIGNAL_INCLUDE_DIR)
  target_include_directories(nanosignal INTERFACE "$<BUILD_INTERFACE:${NANOSIGNAL_INCLUDE_DIR}>")
else()
  target_include_directories(nanosignal INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include>")
endif()
