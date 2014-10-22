cmake_minimum_required(VERSION 2.8)
PROJECT(RepartitionServerTest)

include_directories(${API_DEPENDENCIES_FOLDER}/oscpack)
include_directories(${API_DEPENDENCIES_FOLDER}/bonjour-qt/headers)
include_directories(${API_HEADERS_FOLDER}/Repartition)
include_directories(${API_HEADERS_FOLDER}/Editor)
include_directories(${API_HEADERS_FOLDER})

find_package(Qt5Core)
find_package(Qt5Network)

set(SRC 
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# TODO make it better
set(HEADERS
${API_DEPENDENCIES_FOLDER}/bonjour-qt/headers/bonjourserviceregister.h
${API_HEADERS_FOLDER}/Repartition/session/ZeroConfServer.h
${API_HEADERS_FOLDER}/Repartition/session/ZeroConfServerThread.h)



add_executable(Server ${SRC} ${HEADERS})

target_link_libraries(Server APIMock oscpack dns_sd pthread)
target_link_libraries(Server Qt5::Core Qt5::Network)
