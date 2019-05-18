#
# CMake defines to cross-compile to ARM/Linux on BCM2708 using glibc.
# It's used on the well known Raspberry Pi platform.
#

SET(CMAKE_SYSTEM_NAME Windows)
set(tools /opt/mingw-llvm-toolchain/llvm-mingw/bin)
set(CMAKE_SYSROOT /opt/mingw-llvm-toolchain/llvm-mingw)

SET(CMAKE_C_COMPILER ${tools}/x86_64-w64-mingw32-clang)
SET(CMAKE_CXX_COMPILER ${tools}/x86_64-w64-mingw32-clang++)

SET(CMAKE_SYSTEM_PROCESSOR amd64)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH /opt/mingw-llvm-toolchain/llvm-mingw)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)

SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)