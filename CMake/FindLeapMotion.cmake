if(NOT WIN32)
  # only supported on windows
  return()
endif()

# LEAP_SDK : should point to for instance
# LeapDeveloperKit_3.2.1+45911_win/LeapSDK
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  set(LeapMotion_ARCH_SUFFIX x64)
else()
  set(LeapMotion_ARCH_SUFFIX x86)
endif()

find_path(LeapMotion_INCLUDE_DIRS
    Leap.h
    HINTS
        ${LEAP_SDK}/include
        "C:/LeapSDK/include")
find_library(LeapMotion_LIBRARIES
    NAMES Leap.lib
    HINTS
        ${LEAP_SDK}/lib/${LeapMotion_ARCH_SUFFIX}
        "C:/LeapSDK/lib/${LeapMotion_ARCH_SUFFIX}"
)


if(NOT LeapMotion_LIBRARIES)
  return()
endif()

get_filename_component(LeapMotion_LIB_FOLDER ${LeapMotion_LIBRARIES} DIRECTORY)
set(LeapMotion_DLLS ${LeapMotion_LIB_FOLDER}/Leap.dll)

add_library(LeapMotion SHARED IMPORTED)
set_property(TARGET LeapMotion PROPERTY IMPORTED_LOCATION ${LeapMotion_DLLS})
set_property(TARGET LeapMotion PROPERTY IMPORTED_IMPLIB ${LeapMotion_LIBRARIES})
target_include_directories(LeapMotion INTERFACE ${LeapMotion_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LeapMotion DEFAULT_MSG
  LeapMotion_LIBRARIES LeapMotion_INCLUDE_DIRS)
