# Install
install(TARGETS ossia
    EXPORT ossia-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

# Install headers
foreach(file ${API_HEADERS})
    get_filename_component( dir ${file} DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" dir_clean "${dir}")
    install(
      FILES "${file}" 
      DESTINATION "include/${dir_clean}" 
      COMPONENT Devel)
endforeach()

# Install export header
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ossia_export.h
        DESTINATION include/
        COMPONENT Devel)

# Install used libraries headers
install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/variant/include/
        DESTINATION include/variant
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include/
        DESTINATION include/nano-signal-slot
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/RapidJSON/include/
        DESTINATION include/RapidJSON
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/oscpack/oscpack
        DESTINATION include
        COMPONENT Devel)


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaConfigVersion.cmake"
  VERSION ${ossia_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ossia-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaTargets.cmake"
  NAMESPACE ossia::
)

configure_file(../CMake/ossiaConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/ossia)
install(EXPORT ossia-targets
        DESTINATION "${ConfigPackageLocation}"
        NAMESPACE ossia::)
        
