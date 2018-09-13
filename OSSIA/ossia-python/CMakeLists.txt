project(ossia_python)

message(STATUS "Update ossia-python dependency: pybind11")
execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/pybind11
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${CMAKE_CURRENT_LIST_DIR}/setup.py
  ${CMAKE_CURRENT_LIST_DIR}/versioneer.py
  ${CMAKE_CURRENT_LIST_DIR}/setup.cfg
  ${CMAKE_CURRENT_LIST_DIR}/README.rst
DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/ossia-python)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/../../LICENSE
DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/ossia-python
RENAME LICENSE.txt)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/pyossia
DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/ossia_python)

if(CMAKE_CROSSCOMPILING)
    message(STATUS "We can't use pybind11 CMake module when cross-compiling.")

    include_directories("${OSSIA_3RDPARTY_FOLDER}/pybind11/include")
    include_directories("${PYTHON_INCLUDE_DIR}")
    include_directories("${PYTHON_INCLUDE_DIR}/../")

    message(STATUS "PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIR}")
    message(STATUS "PYTHON_EXECUTABLE - ${PYTHON_EXECUTABLE}")

    add_library(${PROJECT_NAME} SHARED ${PROJECT_NAME}.cpp)
    target_compile_options(${PROJECT_NAME} PRIVATE ${OSSIA_COMPILE_OPTIONS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ossia)

    add_linker_warnings_external(${PROJECT_NAME})

    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
    if ($ENV{PYTHON_VERSION} MATCHES "3.5")
      set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".cpython-35m-arm-linux-gnueabihf.so")
    endif()

    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel --plat-name linux-armv7l
                                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/OSSIA/ossia-python)"
    )

else()
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/pybind11" pybind11)
    message(STATUS "${PROJECT_NAME} binary is compiled and wrap in a python wheel named pyossia")
    pybind11_add_module(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
    use_gold(${PROJECT_NAME})
    add_linker_warnings_external(${PROJECT_NAME})

    message(STATUS "PYTHON_EXECUTABLE - ${PYTHON_EXECUTABLE}")
    message(STATUS "PYTHON_LIBRARY - ${PYTHON_LIBRARY}")

    target_link_libraries(${PROJECT_NAME} PRIVATE ossia)

    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel
                                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/OSSIA/ossia-python)"
    )
endif()

# file(COPY ${CMAKE_SOURCE_DIR}/OSSIA/ossia-python
#      DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/)
# file(COPY ${CMAKE_SOURCE_DIR}/LICENSE
#      DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/ossia-python/LICENSE.txt)
