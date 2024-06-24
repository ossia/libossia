project(ossia_DeviceBuilder LANGUAGES CXX)

find_package(${QT_VERSION} COMPONENTS Widgets)

if(NOT TARGET ${QT_PREFIX}::Widgets)
  return()
endif()

if(NOT OSSIA_PROTOCOL_OSCQUERY)
  return()
endif()

set(CMAKE_AUTOMOC ON)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_PREFIX}::Widgets ossia)
