project(ossia_python)

if(CMAKE_CROSSCOMPILING)
    message(STATUS "We can't use pybind11 CMake module when cross-compiling.")

    include_directories("${OSSIA_3RDPARTY_FOLDER}/pybind11/include")
    include_directories("${PYTHON_INCLUDE_DIR}")
    include_directories("${PYTHON_INCLUDE_DIR}/../")

    message(STATUS "PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIR}")

    add_library(${PROJECT_NAME} SHARED ${PROJECT_NAME}.cpp)
    target_compile_options(${PROJECT_NAME} PRIVATE ${OSSIA_COMPILE_OPTIONS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ossia)

    add_linker_warnings_external(${PROJECT_NAME})

else()
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/pybind11" pybind11)
    message(STATUS "${PROJECT_NAME} binary is compiled and wrap in a python wheel named pyossia")
    pybind11_add_module(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
    use_gold(${PROJECT_NAME})
    add_linker_warnings_external(${PROJECT_NAME})
    message(STATUS "PYTHON_EXECUTABLE - ${PYTHON_EXECUTABLE}")
    message(STATUS "PYTHON_LIBRARY - ${PYTHON_EXECUTABLE}")
    target_link_libraries(${PROJECT_NAME} PRIVATE ossia)
    file(COPY ${CMAKE_SOURCE_DIR}/OSSIA/ossia-python
         DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/)
    add_custom_command(TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/OSSIA/ossia-python)
    #add_custom_command(TARGET ${PROJECT_NAME}
    #  POST_BUILD
    #  COMMAND ${PYTHON_EXECUTABLE} -m pip install pyossia*.whl --user --upgrade
    #  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/OSSIA/ossia-python/dist)
    #message(STATUS "PYOSSIA INSTALL - ${PYTHON_EXECUTABLE} -m pip install --user --upgrade)")
endif()

