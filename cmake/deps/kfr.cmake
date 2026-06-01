if(OSSIA_USE_SYSTEM_LIBRARIES)
  # KFR
  if(OSSIA_ENABLE_KFR)
    find_package(KFR)
    if(NOT TARGET kfr)
      find_library(KFR_LIBRARY NAMES kfr_dft)
      find_path(KFR_INCLUDE_DIR kfr/version.hpp)

      if(KFR_LIBRARY AND KFR_INCLUDE_DIR)
        add_library(kfr INTERFACE IMPORTED GLOBAL)
        target_include_directories(kfr SYSTEM INTERFACE ${KFR_INCLUDE_DIR})
        target_link_libraries(kfr INTERFACE ${KFR_LIBRARY})
        set(KFR_ENABLE_DFT 0 CACHE INTERNAL "")
      endif()
    endif()
  endif()
endif()

function(ossia_disable_unity_on_targets)
  foreach(_libname ${ARGN})
    if(TARGET ${_libname})
      set_target_properties(${_libname} PROPERTIES UNITY_BUILD 0)
    endif()
  endforeach()
endfunction()

if(NOT TARGET kfr)
  block()
    set(CMAKE_CXX_STANDARD 20)
    if(NOT MSVC)
      add_compile_options(-w)
    endif()

    set(KFR_ENABLE_TESTS OFF)
    set(KFR_ENABLE_TESTS OFF CACHE INTERNAL "")
    set(KFR_ENABLE_EXAMPLES OFF)
    set(KFR_ENABLE_EXAMPLES OFF CACHE INTERNAL "")
    set(KFR_ENABLE_AUDIO OFF)
    set(KFR_ENABLE_AUDIO OFF CACHE INTERNAL "")
    set(KFR_ENABLE_IO OFF)
    set(KFR_ENABLE_IO OFF CACHE INTERNAL "")
    set(KFR_USE_BOOST_MATH OFF)
    set(KFR_USE_BOOST_MATH OFF CACHE INTERNAL "")
    set(KFR_USE_BOOST OFF)
    set(KFR_USE_BOOST OFF CACHE INTERNAL "")
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/kfr" "${CMAKE_CURRENT_BINARY_DIR}/kfr_build" SYSTEM)
    ossia_disable_unity_on_targets(
      kfr
      kfr_dft kfr_dft_sse2 kfr_dft_sse41 kfr_dft_sse42 kfr_dft_avx kfr_dft_avx2 kfr_dft_avx512 kfr_dft_neon kfr_dft_neon64 kfr_dft_rvv
      kfr_dsp kfr_dsp_sse2 kfr_dsp_sse41 kfr_dsp_sse42 kfr_dsp_avx kfr_dsp_avx2 kfr_dsp_avx512 kfr_dsp_neon kfr_dsp_neon64 kfr_dsp_rvv
      kfr_io
      kfr_audio
    )
  endblock()
endif()

if(KFR_ENABLE_DFT)
  set(OSSIA_FFT KFR_DOUBLE CACHE INTERNAL "")
  set(OSSIA_FFT_KFR 1 CACHE INTERNAL "")
else()
  set(OSSIA_FFT_KFR 0 CACHE INTERNAL "")
endif()
