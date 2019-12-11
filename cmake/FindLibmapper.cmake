# This defines:
#
#  LIBMAPPER_FOUND - system has LIBMAPPER
#  LIBMAPPER_INCLUDE_DIR - the LIBMAPPER include directory
#  LIBMAPPER_LIBRARIES - Link these to use LIBMAPPER

find_path(LIBMAPPER_INCLUDE_DIR
    mapper/mapper.h
  HINTS
    /opt/libmapper/include
)

find_library(LIBMAPPER_LIBRARIES mapper
    HINTS
      /opt/libmapper/lib
)

if(LIBMAPPER_INCLUDE_DIR AND LIBMAPPER_LIBRARIES)
  add_library(mapper INTERFACE)
  target_include_directories(mapper INTERFACE "${LIBMAPPER_INCLUDE_DIR}")
  target_link_libraries(mapper INTERFACE "${LIBMAPPER_LIBRARIES}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBMAPPER DEFAULT_MSG LIBMAPPER_LIBRARIES LIBMAPPER_INCLUDE_DIR)
mark_as_advanced(LIBMAPPER_INCLUDE_DIR LIBMAPPER_LIBRARIES)
