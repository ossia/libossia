if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_path(READERWRITERQUEUE_INCLUDE_DIR readerwriterqueue.h)
endif()

add_library(readerwriterqueue INTERFACE IMPORTED GLOBAL)
add_library(readerwriterqueue::readerwriterqueue ALIAS readerwriterqueue)

if(READERWRITERQUEUE_INCLUDE_DIR)
  target_include_directories(readerwriterqueue INTERFACE "$<BUILD_INTERFACE:${READERWRITERQUEUE_INCLUDE_DIR}>")
else()
  target_include_directories(readerwriterqueue INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue>")
endif()
