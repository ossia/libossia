cmake_minimum_required(VERSION 2.8)
project(API)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(API_HEADERS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Headers)

add_subdirectory(Headers)
add_subdirectory(Implementations)
add_subdirectory(Examples)
# add_subdirectory(Tests)
