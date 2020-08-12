# Copyright (c) 2015 - 2019, Jean-Michaël Celerier
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
# 2012-01-31, Lars Bilke
# - Enable Code Coverage
#
# 2013-09-17, Joakim Söderberg
# - Added support for Clang.
# - Some additional usage instructions.
#
# 2013-09-17, Jean-Michaël Celerier
# - Use fastcov instead of gcovr / lcov

# Check prereqs
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  message(FATAL_ERROR "Coverage only implemented for GCC 9+")
endif()

find_program(GCOV_PATH NAMES gcov-13 gcov-12 gcov-11 gcov-10 gcov-9 gcov)
if(NOT GCOV_PATH)
  message(FATAL_ERROR "gcov not found")
endif()

execute_process(
  COMMAND "${GCOV_PATH}" "--version"
  OUTPUT_VARIABLE GCOV_OUTPUT
)

if(NOT ("${GCOV_OUTPUT}" MATCHES ".* ([0-9]+\\.[0-9]+\\.[0-9]+).*"))
  message(FATAL_ERROR "cannot parse gcov version : ${GCOV_OUTPUT}")
endif()

if("${CMAKE_MATCH_1}" VERSION_LESS 9.0.0)
  message(FATAL_ERROR "gcov ${CMAKE_MATCH_1} is too old, gcc & gcov 9 required")
endif()

find_program(FASTCOV_PATH fastcov)

if(NOT FASTCOV_PATH)
  message("fastcov not found! No coverage...")
  return()
endif()

function(setup_target_for_coverage _targetname _testrunner _exclusions _outputname)
  add_custom_target(${_targetname}
          ${FASTCOV_PATH} --zerocounters

          COMMAND ${_testrunner} ${ARGV4} || (exit 0)

          # Capturing lcov counters and generating report
          COMMAND ${FASTCOV_PATH} -g ${GCOV_PATH} --lcov --exclude ${_exclusions} .. -o ${_outputname}.info

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

install(
  EXPORT enable_coverage-exports
  DESTINATION lib/cmake/enable_coverage
)

export(EXPORT enable_coverage-exports)

