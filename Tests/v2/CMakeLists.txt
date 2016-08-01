project(OSSIA_v2_tests)

add_executable(minuit_remote minuit_remote.cpp)
target_link_libraries(minuit_remote ossia)
