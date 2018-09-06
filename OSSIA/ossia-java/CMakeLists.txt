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

set(CMAKE_JAVA_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/jna.jar")

# compile ossia.java
add_jar(ossia-java Ossia.java Vec2F.java Vec3F.java Vec4F.java Type.java SizeT.java)

set(CMAKE_JAVA_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/jna.jar:${CMAKE_CURRENT_BINARY_DIR}/ossia-java.jar")
add_jar(ossia-java-example example.java)

set(JAVA_FOLDER "")
set(JAVA_NAME "$<TARGET_FILE_NAME:ossia>")
# TODO arm
if(${CMAKE_SIZEOF_VOID_P} MATCHES "8")
  if(APPLE)
    set(JAVA_FOLDER darwin)
    set(JAVA_NAME libossia.jnilib)
  elseif(WIN32)
    set(JAVA_FOLDER win32-x86-64)
  else()
    set(JAVA_FOLDER linux-x86-64)
  endif()
else()
  if(APPLE)
    set(JAVA_FOLDER darwin)
    set(JAVA_NAME libossia.jnilib)
  elseif(WIN32)
    set(JAVA_FOLDER win32-x86)
  else()
    set(JAVA_FOLDER linux-x86)
  endif()
endif()
add_custom_command(TARGET ossia-java POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E make_directory $<TARGET_FILE_DIR:ossia>/${JAVA_FOLDER}
  COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:ossia> $<TARGET_FILE_DIR:ossia>/${JAVA_FOLDER}/${JAVA_NAME}
  COMMAND "${Java_JAR_EXECUTABLE}" -uf ossia-java.jar -C $<TARGET_FILE_DIR:ossia> ${JAVA_FOLDER}/${JAVA_NAME})
