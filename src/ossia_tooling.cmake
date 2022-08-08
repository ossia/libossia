# Coverage
if(OSSIA_COVERAGE)
  include(CodeCoverage)
  target_link_libraries(ossia PUBLIC enable_coverage)
  setup_target_for_coverage(
      ossia_coverage
      "${CMAKE_COMMAND}"
      "/usr;/opt;libossia/3rdparty;moc_;ui_;.moc;qrc_"
      coverage
      "--build;.;--target;test"
  )
endif()
