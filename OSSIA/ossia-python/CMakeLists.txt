project(ossia_python)

add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/pybind11" pybind11)
    pybind11_add_module(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
    use_gold(${PROJECT_NAME})
    add_linker_warnings_external(${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME} PRIVATE ossia)
    file(COPY ${CMAKE_SOURCE_DIR}/OSSIA/ossia-python
         DESTINATION ${CMAKE_BINARY_DIR}/OSSIA/)
    add_custom_command(TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/OSSIA/ossia-python)