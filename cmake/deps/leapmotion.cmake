find_package(LeapMotion)

if(NOT LeapMotion_FOUND)
  set(OSSIA_PROTOCOL_LEAPMOTION FALSE CACHE INTERNAL "")
endif()
