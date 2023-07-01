if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(websocketpp CONFIG GLOBAL)
endif()

if(NOT TARGET websocketpp::websocketpp)
  add_library(websocketpp INTERFACE IMPORTED GLOBAL)
  add_library(websocketpp::websocketpp ALIAS websocketpp)
  target_include_directories(websocketpp INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/websocketpp>")
endif()
