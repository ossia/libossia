cmake_minimum_required(VERSION 2.8)

project(APIMockImplementation)

include_directories(${API_HEADERS_FOLDER}/Editor)
include_directories(${API_HEADERS_FOLDER}/Misc)
include_directories(${API_HEADERS_FOLDER}/Network)

file(GLOB_RECURSE SRCS *.h *.cpp)

add_library(APIMock ${SRCS})

