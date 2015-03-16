#add_executable(CurveMock curve.cpp)
#target_link_libraries(CurveMock APIMock)

add_executable(CompleteMock complete.cpp)
target_link_libraries(CompleteMock APIMock)

add_executable(StateEx stateEx.cpp)
target_link_libraries(StateEx APIMock)

