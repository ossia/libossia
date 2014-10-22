cmake_minimum_required(VERSION 2.8)
project(Network)

set(HDRS
${CMAKE_CURRENT_SOURCE_DIR}/Address.h
${CMAKE_CURRENT_SOURCE_DIR}/Device.h
${CMAKE_CURRENT_SOURCE_DIR}/Protocols.h
)

add_custom_target(${PROJECT_NAME} SOURCES ${HDRS})
