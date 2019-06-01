# Copyright (c) 2012 - 2015, Lars Bilke
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#
#
# 2012-01-31, Lars Bilke
# - Enable Code Coverage
#
# 2013-09-17, Joakim Söderberg
# - Added support for Clang.
# - Some additional usage instructions.
#
# USAGE:

# 0. (Mac only) If you use Xcode 5.1 make sure to patch geninfo as described here:
#      http://stackoverflow.com/a/22404544/80480
#
# 1. Copy this file into your cmake modules path.
#
# 2. Add the following line to your CMakeLists.txt:
#      INCLUDE(CodeCoverage)
#
# 3. Set compiler flags to turn off optimization and enable coverage:
#    SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#	 SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#
# 3. Use the function SETUP_TARGET_FOR_COVERAGE to create a custom make target
#    which runs your test executable and produces a lcov code coverage report:
#    Example:
#	 SETUP_TARGET_FOR_COVERAGE(
#				my_coverage_target  # Name for custom target.
#				test_driver         # Name of the test driver executable that runs the tests.
#									# NOTE! This should always have a ZERO as exit code
#									# otherwise the coverage generation will not complete.
#				coverage            # Name of output directory.
#				)
#
# 4. Build a Debug build:
#	 cmake -DCMAKE_BUILD_TYPE=Debug ..
#	 make
#	 make my_coverage_target
#
#

# Check prereqs
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  message(FATAL_ERROR "Coverage only implemented for GCC 9+")
endif()

find_program(GCOV_PATH NAMES gcov-9 gcov)
if(NOT GCOV_PATH)
  message(FATAL_ERROR "gcov not found")
endif()

execute_process(
  COMMAND "${GCOV_PATH}" "--version"
  OUTPUT_VARIABLE GCOV_OUTPUT
)

if(NOT ("${GCOV_OUTPUT}" MATCHES ".*([0-9]+\.[0-9]+\.[0-9]+).*"))
  message(FATAL_ERROR "cannot parse gcov version")
endif()

if("${CMAKE_MATCH_1}" VERSION_LESS 9.0.0)
  message(FATAL_ERROR "gcov ${CMAKE_MATCH_1} is too old, gcc & gcov 9 required")
endif()

find_program(FASTCOV_PATH fastcov)
find_program(GENHTML_PATH genhtml)

if(NOT FASTCOV_PATH)
  message("fastcov not found! No coverage...")
  return()
endif()

if(NOT GENHTML_PATH)
  message("genhtml not found! No coverage...")
  return()
endif()

# Param _targetname     The name of new the custom make target
# Param _testrunner     The name of the target which runs the tests.
#						MUST return ZERO always, even on errors.
#						If not, no coverage report will be created!
# Param _outputname     lcov output is generated as _outputname.info
#                       HTML report is generated in _outputname/index.html
# Optional fourth parameter is passed as arguments to _testrunner
#   Pass them in list form, e.g.: "-j;2" for -j 2
function(setup_target_for_coverage _targetname _testrunner _outputname)
  add_custom_target(${_targetname}
          ${FASTCOV_PATH} --zerocounters

          COMMAND ${_testrunner} ${ARGV3}

          # Capturing lcov counters and generating report
          COMMAND ${FASTCOV_PATH} --lcov --exclude /usr /opt libossia/3rdparty moc_ ui_ .moc qrc_  .. -o ${_outputname}.info

          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
          COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
  )
endfunction()

add_library(enable_coverage INTERFACE)
target_compile_options(enable_coverage INTERFACE -g -O0 --coverage)
target_link_libraries(enable_coverage INTERFACE --coverage)

install(
  TARGETS enable_coverage
  EXPORT enable_coverage-exports
)

install(EXPORT enable_coverage-exports
        DESTINATION lib/cmake/enable_coverage
)

export(EXPORT enable_coverage-exports)

