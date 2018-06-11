if(OSSIA_UNITY3D)
    if(${CMAKE_SIZEOF_VOID_P} MATCHES "4")
        set(OSSIA_UNITY_PLUGIN_FOLDER "x86")
    else()
        set(OSSIA_UNITY_PLUGIN_FOLDER "x86_64")
    endif()

    if(APPLE)
        set_target_properties(ossia PROPERTIES
            PREFIX ""
            SUFFIX "bundle"
            )
        install(
            TARGETS ossia
            LIBRARY DESTINATION ossia-unity/Assets/Plugins/
            )
    elseif(WIN32)
        install(
            TARGETS ossia
            RUNTIME DESTINATION ossia-unity/Assets/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}
            )
    else() # Linux
        install(
            TARGETS ossia
            LIBRARY DESTINATION ossia-unity/Assets/Plugins/${OSSIA_UNITY_PLUGIN_FOLDER}
            )
    endif()

    file(GLOB_RECURSE UNITY3D_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR} "ossia-unity3d/*.cs")

    install(
        FILES ${UNITY3D_FILES}
        DESTINATION ossia-unity/Assets/ossia
    )

    install(
        FILES ossia-unity3d/README.md
        DESTINATION ossia-unity/Assets/
    )

    install(
        FILES "${CMAKE_CURRENT_LIST_DIR}/../LICENSE"
        DESTINATION ossia-unity/Assets
    )
endif()

if(OSSIA_QML)
    install(
        TARGETS ossia
        LIBRARY DESTINATION Ossia/
        RUNTIME DESTINATION Ossia/
        )
    install(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/qmldir
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Node.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Binding.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Callback.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/MidiSink.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/MidiSource.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSC.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Property.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSCQueryClient.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSCQueryServer.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Reader.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Writer.qml
        ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
        DESTINATION Ossia)
endif()


# Default case, C / C++ library
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

if(OSSIA_EDITOR)
  install_headers_rec("${OSSIA_EDITOR_HEADERS}")
endif()
if(OSSIA_PROTOCOL_MIDI)
  install_headers_rec("${OSSIA_MIDI_HEADERS}")
endif()
if(OSSIA_PROTOCOL_HTTP)
  install_headers_rec("${OSSIA_HTTP_HEADERS}")
endif()
if(OSSIA_PROTOCOL_SERIAL)
  install_headers_rec("${OSSIA_SERIAL_HEADERS}")
endif()
if(OSSIA_PROTOCOL_OSCQUERY)
  install_headers_rec("${OSSIA_OSCQUERY_HEADERS}")
endif()
if(OSSIA_PROTOCOL_WEBSOCKETS)
  install_headers_rec("${OSSIA_WS_CLIENT_HEADERS}")
endif()
if(OSSIA_PROTOCOL_PHIDGETS)
  install_headers_rec("${OSSIA_PHIDGETS_HEADERS}")
endif()
if(OSSIA_PROTOCOL_LEAPMOTION)
  install_headers_rec("${OSSIA_LEAPMOTION_HEADERS}")
endif()
if(OSSIA_C)
  install_headers_rec("${OSSIA_C_HEADERS}")
endif()
if(OSSIA_CPP)
  install_headers_rec("${OSSIA_CPP_HEADERS}")
endif()
if(OSSIA_DATAFLOW)
  install_headers_rec("${OSSIA_DATAFLOW_HEADERS}")
endif()
if(OSSIA_QT)
  install_headers_rec("${OSSIA_QT_HEADERS}")
endif()
# Install export header
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ossia_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/ossia-config.hpp
        DESTINATION include/
        COMPONENT Devel)

# Install used libraries headers
install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/RtMidi17/rtmidi17
  DESTINATION include/
  COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/variant/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/flat/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/multi_index/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/spdlog/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/asio/asio/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/rapidjson/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/chobo-shl/include/
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/brigand/include/brigand
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/fmt/fmt
        DESTINATION include
        COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/websocketpp/websocketpp
        DESTINATION include
        COMPONENT Devel)
install(FILES ${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include/smallfun.hpp
        DESTINATION include/
        COMPONENT Devel)
install(FILES ${OSSIA_3RDPARTY_FOLDER}/flat_hash_map/flat_hash_map.hpp
        DESTINATION include/
        COMPONENT Devel)
install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/flat/include/flat
        DESTINATION include
        COMPONENT Devel)
install(FILES
     ${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src/hopscotch_map.h
     ${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src/hopscotch_sc_map.h
     ${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src/hopscotch_hash.h
     ${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src/hopscotch_set.h
     ${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src/hopscotch_sc_set.h
        DESTINATION include/
        COMPONENT Devel)

install(FILES
     ${OSSIA_3RDPARTY_FOLDER}/verdigris/src/wobjectdefs.h
     ${OSSIA_3RDPARTY_FOLDER}/verdigris/src/wobjectimpl.h
        DESTINATION include/
        COMPONENT Devel)

install(
    FILES
      ${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue/readerwriterqueue.h
      ${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue/atomicops.h
    DESTINATION include/
    COMPONENT Devel)

install(
      FILES
        ${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/concurrentqueue.h
        ${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/blockingconcurrentqueue.h
      DESTINATION include/
      COMPONENT Devel)

install(DIRECTORY ${OSSIA_3RDPARTY_FOLDER}/GSL/include/gsl
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
