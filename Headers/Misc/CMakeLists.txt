cmake_minimum_required(VERSION 2.8)
project(Misc)

set(HDRS
${CMAKE_CURRENT_SOURCE_DIR}/IObservable.h
${CMAKE_CURRENT_SOURCE_DIR}/IObserver.h
)

add_custom_target(${PROJECT_NAME} SOURCES ${HDRS})
