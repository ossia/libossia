cmake_minimum_required(VERSION 2.8)

project(APIMockImplementation)

include_directories(${OSSIA_HEADERS_FOLDER}/Editor)
include_directories(${OSSIA_HEADERS_FOLDER}/Misc)
include_directories(${OSSIA_HEADERS_FOLDER})

file(GLOB_RECURSE SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp")

add_library(APIMock ${SRCS})

