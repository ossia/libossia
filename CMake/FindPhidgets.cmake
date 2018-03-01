# This defines:
#
#  LIBPHIDGETS_FOUND - system has libphidgets
#  LIBPHIDGETS_INCLUDE_DIR - the libphidgets include directory
#  LIBPHIDGETS_LIBRARIES - Link these to use libphidgets

find_path(LIBPHIDGETS_INCLUDE_DIR phidget22.h
  HINTS
    ${PHIDGETS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libphidgets
  )

find_library(LIBPHIDGETS_LIBRARIES NAMES libphidget22.a
  HINTS
  ${PHIDGETS_LIB_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libphidgets/.libs)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBPHIDGETS DEFAULT_MSG LIBPHIDGETS_LIBRARIES LIBPHIDGETS_INCLUDE_DIR)
mark_as_advanced(LIBPHIDGETS_INCLUDE_DIR LIBPHIDGETS_LIBRARIES)
