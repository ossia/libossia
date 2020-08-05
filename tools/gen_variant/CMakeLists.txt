
add_executable(ossia_genvar gen_variant.cpp)
target_include_directories(ossia_genvar PUBLIC
    "${PROJECT_SOURCE_DIR}/ossia"
    "${PROJECT_BINARY_DIR}/ossia"
    "${OSSIA_3RDPARTY_FOLDER}/brigand/include"
    "${OSSIA_3RDPARTY_FOLDER}/chobo-shl/include"
    "${OSSIA_3RDPARTY_FOLDER}/variant/include"
    "${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src"
    "${OSSIA_3RDPARTY_FOLDER}/fmt"
    )
