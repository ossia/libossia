cmake_minimum_required(VERSION 3.8)

project(ossia-java Java)

find_package(Java REQUIRED)
include(UseJava)

# download JNA jar
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/jna.jar)
  file(DOWNLOAD
    http://repo1.maven.org/maven2/net/java/dev/jna/jna/5.2.0/jna-5.2.0.jar
  ${CMAKE_CURRENT_BINARY_DIR}/jna.jar
  SHOW_PROGRESS
  )
endif()

set(CMAKE_JAVA_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/jna.jar;${CMAKE_CURRENT_SOURCE_DIR}")

# compile ossia.java
set(CMAKE_JAR_CLASSES_PREFIX "io/ossia")
add_jar(ossia-java
  io/ossia/Device.java
  io/ossia/Node.java
  io/ossia/OscQueryServer.java
  io/ossia/OscQueryMirror.java
  io/ossia/Parameter.java
  io/ossia/Protocol.java
  io/ossia/Value.java
  io/ossia/Domain.java
  io/ossia/MessageQueue.java

  io/ossia/Ossia.java
  io/ossia/Vec2F.java
  io/ossia/Vec3F.java
  io/ossia/Vec4F.java
  io/ossia/Type.java
  io/ossia/SizeT.java
  io/ossia/SizeTByReference.java

)


set(CMAKE_JAR_CLASSES_PREFIX "")
set(CMAKE_JAVA_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/jna.jar;${CMAKE_CURRENT_SOURCE_DIR}")
add_jar(ossia-java-example example.java ENTRY_POINT example)
add_jar(ossia-java-test test.java ENTRY_POINT test)


# Copy the ossia shared object into the .jar
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
