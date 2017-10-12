project(ossia_python)

add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/pybind11" pyossia)
    pybind11_add_module(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE ossia)
    use_gold(${PROJECT_NAME})
    add_linker_warnings_external(${PROJECT_NAME})
    message(STATUS "PYTHON - ${PYTHON_EXECUTABLE}")
    install(CODE "MESSAGE(\"Installing the Python Wheel.\")")
    message(STATUS "${PROJECT_NAME} - ${CMAKE_CURRENT_SOURCE_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME}
      COMMAND ${PYTHON_EXECUTABLE} -m pip wheel --wheel-dir .
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    install(TARGETS ${PROJECT_NAME} DESTINATION pyossia COMPONENT ossia-python)


