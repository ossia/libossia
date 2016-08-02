cmake_minimum_required(VERSION 3.4)

project(PluginJamomaUnity CXX)
enable_testing()
find_package(Boost REQUIRED)
find_package(Jamoma COMPONENTS Foundation)
# Setup
set(BUILD_SHARED_LIBS On)
set(CMAKE_POSITION_INDEPENDENT_CODE True)

set(JAMOMA_LIBS "Modular;Foundation;DSP")
set(JAMOMA_PLUGINS "Minuit;OSC;AnalysisLib;DataspaceLib;FunctionLib;System;NetworkLib")

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../Frameworks/jamoma;@loader_path/../Frameworks/jamoma/extensions;@executable_path")
endif()


function(fixup_lib_bundle_jamoma theBundle theExecutable theLibs)
  install(CODE "EXECUTE_PROCESS(COMMAND install_name_tool -add_rpath @loader_path/../Frameworks/jamoma ${theBundle}/Contents/MacOS/${theExecutable})" )

  foreach(library ${theLibs})
    install(CODE "EXECUTE_PROCESS(COMMAND install_name_tool -change @executable_path/../MacOS/libJamoma${library}.6.dylib @loader_path/../Frameworks/jamoma/libJamoma${library}.6.dylib ${theBundle}/Contents/MacOS/${theExecutable})" )
  endforeach()
endfunction()


add_subdirectory(API)
set(SRCS 
  src/ossia/ossia.h
  src/ossia/ossia_utils.hpp
  src/ossia/ossia_protocol.cpp
  src/ossia/ossia_node.cpp
  src/ossia/ossia_device.cpp
  src/ossia/ossia_address.cpp
  src/ossia/ossia_value.cpp
  src/ossia/ossia_domain.cpp

  src/common/ossia_log.h
  src/common/ossia_log.cpp

  src/preset/preset.h
  src/preset/preset.hpp
  src/preset/preset.cpp
  src/preset/exception.hpp
  src/preset/exception.cpp
  src/preset/result.cs
)

if(APPLE)
add_library(ossia MODULE ${SRCS})
set_target_properties(ossia PROPERTIES BUNDLE TRUE)
add_custom_command(TARGET ossia POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:APIJamoma> ${targetfile} ossia.bundle/Contents/Frameworks/libAPIJamoma.dylib)
copy_in_bundle_jamoma(ossia ${CMAKE_CURRENT_BINARY_DIR}/ossia.bundle "Modular;Foundation;DSP" "Minuit;OSC;AnalysisLib;DataspaceLib;FunctionLib;System;NetworkLib")
fixup_lib_bundle_jamoma(${CMAKE_CURRENT_BINARY_DIR}/ossia.bundle ossia "Modular;Foundation;DSP")

else()
add_library(ossia SHARED ${SRCS})
endif()
# Main library
set_property(TARGET ossia PROPERTY CXX_STANDARD 14)
target_link_libraries(ossia PUBLIC APIJamoma Jamoma::Foundation)
target_compile_definitions(ossia PUBLIC RAPIDJSON_HAS_STDSTRING=1)
target_include_directories(ossia PUBLIC API/Headers)
target_include_directories(ossia PUBLIC src)
target_include_directories(ossia PUBLIC ${Boost_INCLUDE_DIRS})


# Tests
if(NOT APPLE)
function(add_ossia_lib_example TheTarget TheFile)
  add_executable(${TheTarget} ${TheFile})
  set_property(TARGET ${TheTarget} PROPERTY CXX_STANDARD 14)
  target_link_libraries(${TheTarget} PRIVATE ossia)
endfunction()
function(add_ossia_lib_test  TheTarget TheFile)
  add_ossia_lib_example(${TheTarget} ${TheFile})
  add_test(${TheTarget}_test ${TheTarget})
endfunction()



add_ossia_lib_example(ParsingExample examples/preset/ParsingExample.cpp)
add_ossia_lib_example(DeviceExample examples/preset/DeviceExample.cpp)

add_ossia_lib_test(test_reception test/ossia/test_reception.cpp)

add_ossia_lib_test(unit-tests-devices test/preset/unit-tests/unit-tests-devices.cpp)
add_ossia_lib_test(unit-tests-presets test/preset/unit-tests/unit-tests-presets.cpp)
add_ossia_lib_test(unit-tests-c-api test/preset/unit-tests/unit-tests-c-api.cpp)

include(CTest)
endif()
