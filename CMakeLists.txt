project(API)
cmake_minimum_required(VERSION 3.0)

set(API_HEADERS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Headers)
set(API_DEPENDENCIES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Deps)

add_subdirectory(Deps)
add_subdirectory(Implementations)
add_subdirectory(Tests)
