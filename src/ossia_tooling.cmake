# Coverage
if(OSSIA_COVERAGE)
  include(CodeCoverage)
  target_link_libraries(ossia PUBLIC enable_coverage)
  setup_target_for_coverage(
      ossia_coverage
      "${CMAKE_COMMAND}"
      coverage
      "--build;.;--target;ExperimentalTest")
endif()
