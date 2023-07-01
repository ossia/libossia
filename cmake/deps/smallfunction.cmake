if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_path(SMALLFUN_INCLUDE_DIR smallfun.hpp)
endif()

add_library(smallfun INTERFACE IMPORTED GLOBAL)
add_library(smallfun::smallfun ALIAS smallfun)

if(SMALLFUN_INCLUDE_DIR)
  target_include_directories(smallfun INTERFACE "$<BUILD_INTERFACE:${SMALLFUN_INCLUDE_DIR}>")
else()
  target_include_directories(smallfun INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include>")
endif()
