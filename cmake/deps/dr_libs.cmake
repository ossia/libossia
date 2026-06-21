ossia_use_system(use_sys dr_libs)
if(use_sys)
  find_path(DR_LIBS_INCLUDE_DIR dr_wav.h)
endif()

add_library(dr_libs INTERFACE IMPORTED GLOBAL)
add_library(dr_libs::dr_libs ALIAS dr_libs)

if(DR_LIBS_INCLUDE_DIR)
  target_include_directories(dr_libs INTERFACE "$<BUILD_INTERFACE:${DR_LIBS_INCLUDE_DIR}>")
else()
  target_include_directories(dr_libs INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/dr_libs>")
endif()
