cmake_minimum_required(VERSION 2.8)
project(Editor)

file(GLOB_RECURSE HDRS *.h)

add_custom_target(${PROJECT_NAME} SOURCES ${HDRS})
