cmake_minimum_required(VERSION 3.0)
project("ossia-pd")

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

if(MSVC)
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/pd.lib")
    message("Downloading and extracting PureData windows binary")
    # On windows we have to create a .lib file for Pd since the original one is incomplete

    # First download Pd.
    file(DOWNLOAD "https://ci.appveyor.com/api/projects/avilleret/pure-data/artifacts/Pure%20Data%20(bin%20X64).zip" "${CMAKE_CURRENT_BINARY_DIR}/pd.zip")

    # Extract Pd.
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_BINARY_DIR}/pd.zip"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/")

endif()
endif()

set(OSSIA_PD_PATH "${OSSIA_3RDPARTY_FOLDER}/pure-data")
set(OSSIA_CICM_PATH "${OSSIA_3RDPARTY_FOLDER}/CicmWrapper")
include_directories("${OSSIA_PD_PATH}/src")

set(OLD_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${OSSIA_CICM_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/Cicm-build")
set(BUILD_SHARED_LIBS ${OLD_BUILD_SHARED_LIBS})

include_directories("${OSSIA_CICM_PATH}/Sources")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty")
include_directories(SYSTEM "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include")

set(PROJECT_SRCS
  src/client.cpp
  src/client.hpp
  src/device.cpp
  src/device.hpp
  src/model.cpp
  src/model.hpp
  src/ossia-pd.cpp
  src/ossia-pd.hpp
  src/ossia_obj_base.cpp
  src/ossia_obj_base.hpp
  src/parameter.cpp
  src/parameter.hpp
  src/remote.cpp
  src/remote.hpp
  src/view.cpp
  src/view.hpp
  src/utils.hpp
  )

include(GenerateExportHeader)
add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})
use_gold(${PROJECT_NAME})

generate_export_header(${PROJECT_NAME} BASE_NAME OSSIA_PD)

# These two lines are used to display the QMLs in the project view of IDEs
set(QMLS "examples/client-example.qml")
add_custom_target(pd_qml_examples SOURCES ${QMLS})

target_link_libraries(${PROJECT_NAME} PRIVATE CicmWrapper ossia)
if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}\\pd.lib")
endif()

add_linker_warnings_external(${PROJECT_NAME})

if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++1z)
endif()

# Win32 uses DLL as a suffix for pd externals
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ossia")
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
elseif(UNIX)
    if(${OSSIA_ARCHITECTURE} MATCHES "arm")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_arm")
    elseif(${OSSIA_ARCHITECTURE} MATCHES "x86")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_i386")
    elseif(${OSSIA_ARCHITECTURE} MATCHES "amd64")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_ia64")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
    endif()
endif()

set(OSSIA_PD_INSTALL_FOLDER "ossia-pd-package/ossia" CACHE STRING "PD Package Install folder")

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/helps"
        DESTINATION ${OSSIA_PD_INSTALL_FOLDER}
        COMPONENT ${PROJECT_NAME})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        DESTINATION ${OSSIA_PD_INSTALL_FOLDER}
        COMPONENT ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${OSSIA_PD_INSTALL_FOLDER}"
        COMPONENT ${PROJECT_NAME})
