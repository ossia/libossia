project(ossia_unity3d)

if(${CMAKE_SIZEOF_VOID_P} MATCHES "4")
  set(OSSIA_UNITY_PLUGIN_FOLDER "x86")
else()
  set(OSSIA_UNITY_PLUGIN_FOLDER "x86_64")
endif()

if(APPLE)
  set_target_properties(ossia PROPERTIES
      PREFIX ""
  )
  install(
      TARGETS ossia
      LIBRARY DESTINATION "ossia/Plugins/${OSSIA_PLATFORM}"
  )
elseif(WIN32)
  install(
      TARGETS ossia
      RUNTIME DESTINATION "ossia/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}"
  )
else() # Linux
  install(
      TARGETS ossia
      LIBRARY DESTINATION "ossia/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}"
  )
endif()

install(
  DIRECTORY .
  DESTINATION ossia/Scripts
  FILES_MATCHING PATTERN *.cs
)

install(
  FILES "README.md"
  DESTINATION ossia/
)

install(
  FILES "${CMAKE_CURRENT_LIST_DIR}/../../LICENSE"
  DESTINATION ossia/
)
