project(ossia-node CXX)
include(node_modules/@mapbox/cmake-node-module/module.cmake)

add_node_module(ossia-node
  INSTALL_PATH build/ossia-node.node
  NAN_VERSION 2.11.0
  MINIMUM_NODE_ABI 64
  )

target_sources(ossia-node INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ossia-node.cpp
)
target_link_libraries(ossia-node INTERFACE
    ossia
)
target_include_directories(ossia-node INTERFACE
    ossia
    node_modules/nan
)

if(APPLE)
  target_link_libraries(ossia-node INTERFACE -Wl,-undefined,dynamic_lookup)
elseif(UNIX)
  if(LLD_LINKER_SUPPORTED)
    target_link_libraries(ossia-node INTERFACE -Wl,--unresolved-symbols=ignore-all)
  else()
    target_link_libraries(ossia-node INTERFACE -Wl,--warn-unresolved-symbols)
  endif()
endif()

if(OSSIA_SANITIZE)
  target_link_libraries(ossia-node INTERFACE
      -fsanitize=address
      -fsanitize=undefined
      asan
      ubsan
  )
endif()
