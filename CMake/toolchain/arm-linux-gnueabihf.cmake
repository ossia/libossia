#
# CMake defines to cross-compile to ARM/Linux on BCM2708 using glibc.
# It's used on the well known Raspberry Pi platform.
#

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER ${CROSS_COMPILER_PATH}arm-linux-gnueabihf-gcc-8)
SET(CMAKE_CXX_COMPILER ${CROSS_COMPILER_PATH}arm-linux-gnueabihf-g++-8)
SET(CMAKE_ASM_COMPILER ${CROSS_COMPILER_PATH}arm-linux-gnueabihf-gcc-8)
SET(CMAKE_SYSTEM_PROCESSOR arm)

SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)

SET(CMAKE_FIND_ROOT_PATH ${RPI_ROOT_PATH}/opt/vc ${RPI_ROOT_PATH} ${RPI_ROOT_PATH}/usr )

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_LIBRARY_PATH "${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/;${RPI_ROOT_PATH}/lib/arm-linux-gnueabihf/")

SET(CMAKE_SYSROOT ${RPI_ROOT_PATH})

# this doesn't seem to do much, at least it doesn't work on first try, we should export PKG_CONFIG_SYSROOT_DIR and PKG_CONFIG_LIBDIR before running cmake
#SET(ENV{PKG_CONFIG_LIBDIR} "${RPI_ROOT_PATH}/usr/lib/pkgconfig:${RPI_ROOT_PATH}/usr/share/pkgconfig:${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/pkgconfig/")
#SET(ENV{PKG_CONFIG_SYSROOT_DIR} ${RPI_ROOT_PATH})
#SET(ENV{PKG_CONFIG_SYSROOT_DIR} "/tmp/rpi/root")

# rdynamic means the backtrace should work
IF (CMAKE_BUILD_TYPE MATCHES "Debug")
   add_definitions(-rdynamic)
ENDIF()

# avoids annoying and pointless warnings from gcc
# SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U_FORTIFY_SOURCE ")
# SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -c")
