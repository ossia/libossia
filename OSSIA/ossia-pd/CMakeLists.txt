cmake_minimum_required(VERSION 3.0)
project("ossia-pd")

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

option(OSSIA_PD_BENCHMARK "Add some process time computation and print it to the console" OFF)

if(OSSIA_PD_BENCHMARK)
    add_definitions(-DOSSIA_PD_BENCHMARK)
endif()

message(STATUS "Update ossia-pd dependencies: pure-data and CicmWrapper")
execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/pure-data
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/CicmWrapper
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/pd/bin/pd.lib")
    message("Downloading and extracting PureData windows binary")
    # First download Pd.
    if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
      file(DOWNLOAD "https://ci.appveyor.com/api/buildjobs/a0urdqx1xuve5fbh/artifacts/Pure%20Data%20(bin%20X64).zip" "${CMAKE_CURRENT_BINARY_DIR}/pd.zip")
    else()
      file(DOWNLOAD "http://msp.ucsd.edu/Software/pd-0.48-1test3.msw.zip" "${CMAKE_CURRENT_BINARY_DIR}/pd.zip")
    endif()

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
  src/attribute.cpp
  src/attribute.hpp
  src/client.cpp
  src/client.hpp
  src/device.cpp
  src/device.hpp
  src/device_base.cpp
  src/device_base.hpp
  src/logger.cpp
  src/logger.hpp
  src/model.cpp
  src/model.hpp
  src/node_base.cpp
  src/node_base.hpp
  src/ossia-pd.cpp
  src/ossia-pd.hpp
  src/object_base.cpp
  src/object_base.hpp
  src/parameter.cpp
  src/parameter.hpp
  src/parameter_base.cpp
  src/parameter_base.hpp
  src/remote.cpp
  src/remote.hpp
  src/view.cpp
  src/view.hpp
  src/utils.hpp
  src/utils.cpp
  src/assert.hpp
  src/assert.cpp
  )

include(GenerateExportHeader)
add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})

if(OSSIA_SANITIZE)
    sanitize_build(${PROJECT_NAME})
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${OSSIA_COMPILE_OPTIONS})
use_gold(${PROJECT_NAME})

generate_export_header(${PROJECT_NAME} BASE_NAME OSSIA_PD)

# These two lines are used to display the QMLs in the project view of IDEs
set(QMLS "examples/client-example.qml")
add_custom_target(pd_qml_examples SOURCES ${QMLS})

target_link_libraries(${PROJECT_NAME} PRIVATE CicmWrapper ossia)
if(MSVC)
  if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}\\pd.lib")
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}\\pd\\bin\\pd.lib")
  endif()
endif()

add_linker_warnings_external(${PROJECT_NAME})

if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)
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

if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-undefined,dynamic_lookup)
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

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE
        DESTINATION "${OSSIA_PD_INSTALL_FOLDER}"
        COMPONENT ${PROJECT_NAME})

if(OSSIA_TESTING)
  # need to call enable_testing() here
  # to update test on CMakeLists.txt changes
  # enable_testing()

  message(STATUS "Define Pd test")
  message("First a loading test")

  if(MSVC)
    add_test(NAME ossia-pd-loading-test
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/../../ci/pd-loading-test.cmd
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/unit-tests
        )
  else()
    add_test(NAME ossia-pd-loading-test
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/../../ci/pd-loading-test.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/unit-tests
        )
  endif()

  file(GLOB PD_UNIT_TEST
       LIST_DIRECTORIES false
       RELATIVE ${CMAKE_CURRENT_LIST_DIR}/unit-tests
       ${CMAKE_CURRENT_LIST_DIR}/unit-tests/*.pd
       )

  foreach(patcher ${PD_UNIT_TEST})
    message("add test patcher: ${patcher}")
    add_test(NAME ${patcher}
             COMMAND pd -nogui -lib ossia ${patcher}
             WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/unit-tests
             )
  endforeach()
  include(CTest)
endif()
