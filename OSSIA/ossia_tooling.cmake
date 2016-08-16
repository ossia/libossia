
# Coverage
if(OSSIA_COVERAGE)
    setup_target_for_coverage(
        ossia_coverage
        "$ENV{CMAKE_BIN}"
        coverage
        "--build;.;--target;ExperimentalTest")
endif()

# Taken from http://www.labri.fr/perso/fleury/posts/programming/using-clang-tidy-and-clang-format.html
# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT}
        -i
        ${SRCS}
        )
endif()

# Adding clang-tidy target if executable is found
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    add_custom_target(
        clang-tidy
        COMMAND ${CLANG_TIDY}
        ${SRCS}
        -config=''
        --
        -std=c++14
        ${INCLUDE_DIRECTORIES}
        )
endif()
