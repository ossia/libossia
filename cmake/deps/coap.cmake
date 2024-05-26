if(OSSIA_USE_SYSTEM_LIBRARIES)
    find_path(LIBCOAP_INCLUDEDIR coap3/libcoap.h)
    find_library(LIBCOAP_LIBRARIES coap-3)
    add_library(coap-3 IMPORTED SHARED GLOBAL)
    add_library(libcoap::coap-3 ALIAS coap-3)
    target_include_directories(coap-3 INTERFACE "${LIBCOAP_INCLUDEDIR}")
    set_target_properties(coap-3 PROPERTIES IMPORTED_LOCATION "${LIBCOAP_LIBRARIES}")
endif()

if(NOT TARGET libcoap::coap-3)
    include(FetchContent)
    FetchContent_Declare(
      libcoap
      GIT_REPOSITORY "https://github.com/obgm/libcoap"
      GIT_TAG develop
      GIT_PROGRESS true
    )

    set(ENABLE_DTLS OFF)
    FetchContent_MakeAvailable(libcoap)
endif()

