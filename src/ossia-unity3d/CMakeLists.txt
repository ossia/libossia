project(ossia_unity3d)

if(${CMAKE_SIZEOF_VOID_P} MATCHES "4")
  set(OSSIA_UNITY_ARCH_SUFFIX "x86")
else()
  set(OSSIA_UNITY_ARCH_SUFFIX "x86_64")
endif()

set(OSSIA_UNITY_PACKAGE_FOLDER ossia)

if(APPLE)
  set(OSSIA_UNITY_PLUGIN_FOLDER ${OSSIA_PLATFORM})
  set_target_properties(ossia PROPERTIES
      PREFIX ""
  )
  install(
      TARGETS ossia
      LIBRARY DESTINATION "${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}"
  )
elseif(WIN32)
  set(OSSIA_UNITY_PLUGIN_FOLDER win_${OSSIA_UNITY_ARCH_SUFFIX})
  install(
      TARGETS ossia
      RUNTIME DESTINATION "${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}"
  )
else() # Linux
  set(OSSIA_UNITY_PLUGIN_FOLDER linux_${OSSIA_UNITY_ARCH_SUFFIX})
  install(
      TARGETS ossia
      LIBRARY DESTINATION "${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}"
  )
endif()

configure_file( ${CMAKE_CURRENT_LIST_DIR}/package.json.in ${CMAKE_BINARY_DIR}/package.json @ONLY)

install(
  DIRECTORY .
  DESTINATION ${OSSIA_UNITY_PACKAGE_FOLDER}/Scripts
  FILES_MATCHING PATTERN *.cs
)

install(
  FILES 
    "${CMAKE_CURRENT_LIST_DIR}/README.md"
    "${CMAKE_BINARY_DIR}/package.json"
    "${CMAKE_CURRENT_LIST_DIR}/../../LICENSE"
  DESTINATION ${OSSIA_UNITY_PACKAGE_FOLDER}/
)

# create Unity's meta files for platform specific libraries
# this should be at the very end of this CMakeLists.txt to be executed 
# after the installation of the libraries themselves
install(CODE "
message(\"-- Search for libraries in ${CMAKE_INSTALL_PREFIX}/${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}\")

file(GLOB_RECURSE installed_libs 
  LIST_DIRECTORIES false 
  \"${CMAKE_INSTALL_PREFIX}/${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}/*.a\"
  \"${CMAKE_INSTALL_PREFIX}/${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}/*.dylib\"
  \"${CMAKE_INSTALL_PREFIX}/${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}/*.so\"
  \"${CMAKE_INSTALL_PREFIX}/${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}/*.dll\")

foreach(lib IN LISTS installed_libs)
  file(RELATIVE_PATH path ${CMAKE_INSTALL_PREFIX}/${OSSIA_UNITY_PACKAGE_FOLDER}/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER} \${lib})
  string(UUID GUID 
    NAMESPACE 09EC9332-39A2-4937-AF47-7C62ABC1AB89
    NAME \${path}
    TYPE MD5)

  string(REPLACE \"-\" \"\" GUID \${GUID})
  string(TOLOWER \${GUID} GUID)

  message(\"-- Creating META file: \${path}.meta with guid: \${GUID}\")

  configure_file(
   ${CMAKE_CURRENT_LIST_DIR}/${OSSIA_UNITY_PLUGIN_FOLDER}-template.meta.in
   \${lib}.meta 
   @ONLY)
endforeach()
")
