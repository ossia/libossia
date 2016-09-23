# Install
install(TARGETS ossia
    EXPORT ossia-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

# Install headers
function(install_headers_rec theHeaders)

    foreach(file ${theHeaders})
        get_filename_component( dir ${file} DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" dir_clean "${dir}")
        install(
          FILES "${file}"
          DESTINATION "include/${dir_clean}"
          COMPONENT Devel)
    endforeach()

endfunction()

install_headers_rec("${API_HEADERS}")
if(OSSIA_PROTOCOL_MIDI)
    install_headers_rec("${MIDI_HEADERS}")
endif()
if(OSSIA_PROTOCOL_HTTP)
    install_headers_rec("${HTTP_HEADERS}")
endif()
if(OSSIA_PROTOCOL_SERIAL)
    install_headers_rec("${SERIAL_HEADERS}")
endif()
if(OSSIA_PROTOCOL_WEBSOCKETS)
    install_headers_rec("${WS_CLIENT_HEADERS}")
endif()

# Install export header
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ossia_export.h
        DESTINATION include/
        COMPONENT Devel)

# Install used libraries headers
install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/variant/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include/
        DESTINATION include/nano-signal-slot
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/oscpack/oscpack
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/spdlog/include/
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

