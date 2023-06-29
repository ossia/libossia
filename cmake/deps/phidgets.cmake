find_package(Phidgets)

if(NOT "${LIBPHIDGETS_FOUND}")
  set(OSSIA_PROTOCOL_PHIDGETS FALSE CACHE INTERNAL "")
endif()

if(UNIX AND NOT APPLE)
  find_package(USB)
  if(NOT "${LIBUSB_FOUND}")
    set(OSSIA_PROTOCOL_PHIDGETS FALSE CACHE INTERNAL "")
  endif()
endif()
