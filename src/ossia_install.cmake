target_include_directories(ossia PUBLIC $<INSTALL_INTERFACE:include>)

set(3RDPARTY_INSTALL_PATTERN
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp"
  PATTERN "*.tpp"
  PATTERN "*.ipp"
  PATTERN "*.def"
  PATTERN "license*"
  PATTERN "License*"
  PATTERN "LICENSE*"
  PATTERN "authors*"
  PATTERN "Authors*"
  PATTERN "AUTHORS*"
)

if(OSSIA_QML)
    install(
        TARGETS ossia
        LIBRARY DESTINATION Ossia/
        RUNTIME DESTINATION Ossia/
        ARCHIVE DESTINATION Ossia/
        )
    install(
        FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/qmldir"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Node.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Binding.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Callback.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/MidiSink.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/MidiSource.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSC.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Property.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSCQueryClient.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSCQueryServer.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Reader.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Writer.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
        DESTINATION Ossia)
endif()


if(NOT OSSIA_QML_ONLY AND NOT OSSIA_UNITY3D_ONLY AND NOT OSSIA_MAX_ONLY)
# Default case, C / C++ library
# Install
install(TARGETS ossia
    COMPONENT Devel
    EXPORT ossia-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    RUNTIME DESTINATION bin)

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

if(NOT OSSIA_CPP_ONLY)
  install_headers_rec("${API_HEADERS}")
endif()

if(OSSIA_EDITOR)
  install_headers_rec("${OSSIA_EDITOR_HEADERS}")
  install_headers_rec("${OSSIA_EXECLOG_HEADERS}")
endif()
if(OSSIA_PROTOCOL_OSC)
  install_headers_rec("${OSSIA_OSC_HEADERS}")
endif()
if(OSSIA_PROTOCOL_MINUIT)
  install_headers_rec("${OSSIA_MINUIT_HEADERS}")
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
if(OSSIA_PROTOCOL_OSCQUERY AND NOT OSSIA_CPP_ONLY)
  install_headers_rec("${OSSIA_OSCQUERY_HEADERS}")
endif()
if(OSSIA_PROTOCOL_WEBSOCKETS)
  install_headers_rec("${OSSIA_WS_CLIENT_HEADERS}")
endif()
if(OSSIA_PROTOCOL_PHIDGETS)
  install_headers_rec("${OSSIA_PHIDGETS_HEADERS}")
endif()
if(OSSIA_C)
  install_headers_rec("${OSSIA_C_HEADERS}")
endif()
if(OSSIA_CPP)
  install_headers_rec("${OSSIA_CPP_HEADERS}")
endif()
if(OSSIA_DATAFLOW)
  install_headers_rec("${OSSIA_DATAFLOW_HEADERS}")
  install_headers_rec("${OSSIA_GFX_HEADERS}")
endif()
if(OSSIA_QT)
  install_headers_rec("${OSSIA_QT_HEADERS}")
endif()
if(OSSIA_ENABLE_FFT)
  install_headers_rec("${OSSIA_FFT_HEADERS}")
endif()

if (OSSIA_PROTOCOL_OSC OR OSSIA_PROTOCOL_MINUIT OR OSSIA_PROTOCOL_OSCQUERY)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/oscpack/oscpack"
        DESTINATION include
        COMPONENT Devel
        ${3RDPARTY_INSTALL_PATTERN}
)
endif()

# Install export header
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ossia_export.h"
        "${CMAKE_CURRENT_BINARY_DIR}/ossia-config.hpp"
        DESTINATION include/
        COMPONENT Devel)

# Install used libraries headers
if(NOT OSSIA_CPP_ONLY AND NOT OSSIA_C_ONLY)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/libremidi/include/libremidi"
        DESTINATION include
        COMPONENT Devel
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include/"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/mdspan/include/"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/tuplet/include/"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/spdlog/include/"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/rapidjson/include/"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/unordered_dense/include/ankerl"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/magic_enum/include/magic_enum"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/rnd/include/rnd"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/tuplet/include/tuplet"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/fmt/include/fmt"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/websocketpp/websocketpp"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(FILES "${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include/smallfun.hpp"
        DESTINATION include/
        COMPONENT Devel
)

if(EXISTS "${OSSIA_3RDPARTY_FOLDER}/dr_libs/dr_wav.h")
  install(FILES
            "${OSSIA_3RDPARTY_FOLDER}/dr_libs/dr_wav.h"
            "${OSSIA_3RDPARTY_FOLDER}/dr_libs/dr_flac.h"
            "${OSSIA_3RDPARTY_FOLDER}/dr_libs/dr_mp3.h"
          DESTINATION include/
          COMPONENT Devel)
endif()


install(
  FILES
     "${OSSIA_3RDPARTY_FOLDER}/verdigris/src/wobjectdefs.h"
     "${OSSIA_3RDPARTY_FOLDER}/verdigris/src/wobjectcpp.h"
     "${OSSIA_3RDPARTY_FOLDER}/verdigris/src/wobjectimpl.h"
     "${OSSIA_3RDPARTY_FOLDER}/verdigris/src/verdigris"
  DESTINATION include/
  COMPONENT Devel
)

install(
    FILES
      "${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue/readerwriterqueue.h"
      "${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue/readerwritercircularbuffer.h"
      "${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue/atomicops.h"
    DESTINATION include/
    COMPONENT Devel
)

install(
      FILES
        "${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/concurrentqueue.h"
        "${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/blockingconcurrentqueue.h"
        "${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/lightweightsemaphore.h"
      DESTINATION include/
      COMPONENT Devel
)

install(
      FILES
        "${OSSIA_3RDPARTY_FOLDER}/Flicks/flicks.h"
      DESTINATION include/
      COMPONENT Devel
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/span/include/tcb"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/rubberband/rubberband"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)

install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/weakjack"
        DESTINATION include
        COMPONENT Devel
        MESSAGE_NEVER
        ${3RDPARTY_INSTALL_PATTERN}
)
find_program(BOOST_BCP "bcp")
if(BOOST_BCP)
   # install a subset of boost thanks to bcp
   install(CODE "execute_process(COMMAND bcp --boost=${Boost_INCLUDE_DIR} boost/any.hpp boost/container/flat_map.hpp boost/container/flat_set.hpp boost/container/small_vector.hpp boost/container/static_vector.hpp boost/lexical_cast.hpp boost/algorithm/string/replace.hpp boost/endian/conversion.hpp boost/predef.h boost/variant2.hpp ${CMAKE_INSTALL_PREFIX}/include)")
else()
  if(OSSIA_MUST_INSTALL_BOOST)
    install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/boost"
            DESTINATION include
            COMPONENT Devel
            MESSAGE_NEVER
            ${3RDPARTY_INSTALL_PATTERN}
    )
  endif(OSSIA_MUST_INSTALL_BOOST)
endif()
endif(NOT OSSIA_CPP_ONLY AND NOT OSSIA_C_ONLY)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaConfigVersion.cmake"
  VERSION ${ossia_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(../cmake/ossiaConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaConfig.cmake"
    INSTALL_DESTINATION lib/cmake/ossia
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ossia/ossiaConfigVersion.cmake
    DESTINATION lib/cmake/ossia
    COMPONENT Devel
)

set(ConfigPackageLocation lib/cmake/ossia)
install(EXPORT ossia-targets
        DESTINATION "${ConfigPackageLocation}"
        NAMESPACE ossia::
        COMPONENT Devel)
endif()
