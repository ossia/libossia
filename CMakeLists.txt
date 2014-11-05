cmake_minimum_required(VERSION 2.8)
project(API)

set(API_HEADERS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Headers)

add_subdirectory(Headers)
add_subdirectory(Implementations)
# add_subdirectory(Tests)
