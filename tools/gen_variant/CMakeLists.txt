
add_executable(ossia_genvar gen_variant.cpp)
target_include_directories(ossia_genvar PUBLIC
    "${CMAKE_CURRENT_PROJECT_DIR}/OSSIA"
    "${OSSIA_3RDPARTY_FOLDER}/brigand/include"
    "${OSSIA_3RDPARTY_FOLDER}/chobo-shl/include"
    "${OSSIA_3RDPARTY_FOLDER}/variant/include"
    "${OSSIA_3RDPARTY_FOLDER}/fmt"
    )
