if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_path(EXPRTK_INCLUDE_DIR exprtk.hpp)
endif()

if(NOT EXPRTK_INCLUDE_DIR)
  set(EXPRTK_INCLUDE_DIR "${OSSIA_3RDPARTY_FOLDER}/exprtk")
endif()

if(NOT TARGET exprtk::exprtk)
  add_library(exprtk INTERFACE IMPORTED GLOBAL)
  add_library(exprtk::exprtk ALIAS exprtk)
  target_include_directories(exprtk INTERFACE "${EXPRTK_INCLUDE_DIR}")
endif()
