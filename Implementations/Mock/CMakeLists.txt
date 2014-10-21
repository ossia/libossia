cmake_minimum_required(VERSION 3.0)

project(APIMockImplementation)

include_directories(${API_HEADERS_FOLDER}/Editor)
include_directories(${API_HEADERS_FOLDER}/Misc)
include_directories(${API_HEADERS_FOLDER})

set(SRCS 
#${CMAKE_CURRENT_SOURCE_DIR}/Sources/Event.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/IObservable.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/later.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/Scenario.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/TimeBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/TimeNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/TimeProcess.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sources/TimeValue.cpp)

set(HEADERS
${CMAKE_CURRENT_SOURCE_DIR}/Sources/later.h)

add_library(APIMock ${SRCS} ${HEADERS})

