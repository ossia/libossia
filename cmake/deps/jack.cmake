find_path(Jack_INCLUDE_DIR
  NAMES jack/jack.h
  HINTS
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${OSSIA_SDK}/jack/include"
)

if(Jack_INCLUDE_DIR)
  add_library(jack INTERFACE IMPORTED GLOBAL)
  add_library(jack::jack ALIAS jack)

  target_include_directories(jack SYSTEM INTERFACE
    $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/weakjack>
    $<BUILD_INTERFACE:${Jack_INCLUDE_DIR}>
  )
  target_link_libraries(jack INTERFACE ${CMAKE_DL_LIBS})
endif()
