project("ossia-pd")

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

option(OSSIA_PD_BENCHMARK "Add some process time computation and print it to the console" OFF)
option(OSSIA_PURR_DATA "Build against Purr Data sources" OFF)

if(OSSIA_PD_BENCHMARK)
    add_definitions(-DOSSIA_PD_BENCHMARK)
endif()

message(STATUS "Update ossia-pd dependencies: pure-data and CicmWrapper")
if(OSSIA_PURR_DATA)
  if(NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/purr-data")
    execute_process(COMMAND git clone --recursive -j16 https://github.com/jcelerier/purr-data
                    WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}"
                    COMMAND_ERROR_IS_FATAL ANY)
  endif()
else()
  if(NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/pure-data")
    execute_process(COMMAND git clone --recursive -j16 https://github.com/pure-data/pure-data
                    WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}"
                    COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND git checkout 0.49-0
                    WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/pure-data"
                    COMMAND_ERROR_IS_FATAL ANY)
  endif()
endif()

if(NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/CicmWrapper")
  execute_process(COMMAND git clone --recursive -j16 https://github.com/jcelerier/CicmWrapper
                  WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}"
                  COMMAND_ERROR_IS_FATAL ANY)
endif()


if(MSVC)
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/pd/bin/pd.lib")
  if(OSSIA_PURR_DATA)
    # Purr-data's Windows release is an Inno Setup installer wrapped in a zip;
    # the raw zip never contained bin/pd.lib usable at link time. The MinGW
    # build (packages/win64_inno/Makefile.nmake -> pd/src/makefile.mingw)
    # produces pd.dll plus a renamed `pd.a` import library installed as
    # bin/pd.lib -- which the MSVC linker can consume for purr-data-specific
    # symbols (sys_vguid, s__X, ...) absent from vanilla PureData's pd.lib.
    # So: download the installer and run it silently to extract its tree.
    set(PURR_VERSION "2.20.1")
    set(PURR_URL "https://github.com/agraef/purr-data/releases/download/${PURR_VERSION}/purr-data-${PURR_VERSION}-mingw-x86_64.zip")
    message("Downloading PurrData windows installer (${PURR_VERSION})")
    file(DOWNLOAD "${PURR_URL}" "${CMAKE_CURRENT_BINARY_DIR}/purr.zip" SHOW_PROGRESS)

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_BINARY_DIR}/purr.zip"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/")

    # The zip extracts to a single Purr-Data-*-x86_64.exe installer alongside
    # the zip itself. Locate it (the rev hash varies between releases).
    file(GLOB PURR_INSTALLER "${CMAKE_CURRENT_BINARY_DIR}/Purr-Data-*x86_64.exe")
    list(GET PURR_INSTALLER 0 PURR_INSTALLER)
    if(NOT PURR_INSTALLER)
      message(FATAL_ERROR "Could not locate Purr Data installer in ${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    # Inno Setup silent install. The installer declares
    # PrivilegesRequired=admin, but GitHub Actions Windows runners already
    # run as admin so this is fine.
    # /VERYSILENT no UI, /SUPPRESSMSGBOXES no boxes, /NORESTART skip reboot,
    # /DIR= install destination. Use a Windows-native path.
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/purr-install" PURR_INSTALL_NATIVE)
    message("Running PurrData installer silently into ${PURR_INSTALL_NATIVE}")
    execute_process(
        COMMAND "${PURR_INSTALLER}"
                /VERYSILENT /SUPPRESSMSGBOXES /NORESTART
                "/DIR=${PURR_INSTALL_NATIVE}"
        TIMEOUT 300
        RESULT_VARIABLE PURR_RC
        OUTPUT_VARIABLE PURR_OUT
        ERROR_VARIABLE  PURR_ERR)
    if(NOT PURR_RC EQUAL 0)
      message(FATAL_ERROR "PurrData installer failed (rc=${PURR_RC}):\n${PURR_OUT}\n${PURR_ERR}")
    endif()

    # The installer drops the Pd tree directly under DIR. Move it into the
    # `pd/` directory the build expects.
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/purr-install/bin/pd.lib")
      file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/purr-install" "${CMAKE_CURRENT_BINARY_DIR}/pd")
    elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/purr-install/pd/bin/pd.lib")
      file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/purr-install/pd" "${CMAKE_CURRENT_BINARY_DIR}/pd")
    else()
      file(GLOB_RECURSE FOUND_PDLIB "${CMAKE_CURRENT_BINARY_DIR}/purr-install/*pd.lib")
      message(FATAL_ERROR "PurrData installer did not produce a usable pd.lib at the expected paths. Found: ${FOUND_PDLIB}")
    endif()
  else()
    message("Downloading and extracting PureData windows binary")
    set(PD_VERSION "0.49-0")
    # First download Pd.
    if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
      file(DOWNLOAD "http://msp.ucsd.edu/Software/pd-${PD_VERSION}.msw.zip" "${CMAKE_CURRENT_BINARY_DIR}/pd.zip")
    else()
      file(DOWNLOAD "http://msp.ucsd.edu/Software/pd-${PD_VERSION}-i386.msw.zip" "${CMAKE_CURRENT_BINARY_DIR}/pd.zip")
    endif()

    # Extract Pd.
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_BINARY_DIR}/pd.zip"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/")

    if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
      file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/pd-${PD_VERSION}" "${CMAKE_CURRENT_BINARY_DIR}/pd")
    else()
      file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/pd-${PD_VERSION}-i386" "${CMAKE_CURRENT_BINARY_DIR}/pd")
    endif()
  endif()
endif()
endif()

if(OSSIA_PURR_DATA)
  set(OSSIA_PD_PATH "${OSSIA_3RDPARTY_FOLDER}/purr-data/pd")
  add_definitions(-DPURR_DATA)
else()
  set(OSSIA_PD_PATH "${OSSIA_3RDPARTY_FOLDER}/pure-data")
endif()
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
  src/explorer.cpp
  src/explorer.hpp
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
  src/ZeroconfOscqueryListener.cpp
  src/ZeroconfOscqueryListener.hpp
  src/ZeroconfMinuitListener.cpp
  src/ZeroconfMinuitListener.hpp
  )

include(GenerateExportHeader)
add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE ${OSSIA_COMPILE_OPTIONS})

generate_export_header(${PROJECT_NAME} BASE_NAME OSSIA_PD)

# These two lines are used to display the QMLs in the project view of IDEs
set(QMLS "examples/client-example.qml")
add_custom_target(pd_qml_examples SOURCES ${QMLS})

target_link_libraries(${PROJECT_NAME} PRIVATE CicmWrapper ossia Servus_iface)
if(MSVC)
  target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}\\pd\\bin\\pd.lib")
endif()

add_linker_warnings_external(${PROJECT_NAME})

if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)
endif()

# Win32 uses DLL as a suffix for pd externals
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ossia")

if(APPLE)
  set(PLATFORM_SUFFIX "d") # d for Darwin
elseif(UNIX)
  set(PLATFORM_SUFFIX "l") # l for Linux
else()
  set(PLATFORM_SUFFIX "m") # m for Microsoft
endif()


if(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*arm.*")
  set(ARCH_SUFFIX "arm")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*aarch64.*")
  set(ARCH_SUFFIX "aarch64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*64.*")
  set(ARCH_SUFFIX "amd64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*86.*")
  set(ARCH_SUFFIX "i386")
else()
  message(FATAL "Could not determine target architecture")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".${PLATFORM_SUFFIX}_${ARCH_SUFFIX}")

if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-undefined,dynamic_lookup)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  target_link_libraries(${PROJECT_NAME} PRIVATE "-static-libgcc -static-libstdc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
  # TODO
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
  elseif(APPLE)
    add_test(NAME ossia-pd-loading-test
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/../../ci/pd-loading-test-osx.sh
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
             COMMAND pd -nogui -lib ossia -open ${patcher} -send "ossia testing 1;"
             WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/unit-tests
             )
  endforeach()
  include(CTest)
endif()
