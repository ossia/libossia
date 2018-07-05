cmake_minimum_required(VERSION 3.8)

project(ossia-java Java)

find_package(Java REQUIRED)
include(UseJava)

# download JNA jar
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/jna.jar)
  file(DOWNLOAD
    http://repo1.maven.org/maven2/net/java/dev/jna/jna/4.5.1/jna-4.5.1.jar
  ${CMAKE_CURRENT_BINARY_DIR}/jna.jar
  SHOW_PROGRESS
  )
endif()

set(CMAKE_JAVA_INCLUDE_PATH ${CMAKE_CURRENT_BINARY_DIR}/jna.jar)

# compile ossia.java
add_jar(ossia-java Ossia.java Vec2F.java Vec3F.java Vec4F.java Type.java SizeT.java)

set(CMAKE_JAVA_INCLUDE_PATH ${CMAKE_CURRENT_BINARY_DIR}/jna.jar)
add_jar(ossia-java-example example.java)
