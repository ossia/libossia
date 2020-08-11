# Building examples

Most of those examples use the core libossia C++ implementation (libossia-native).

To build them, given a source tarball of libossia, you can either : 

## Use CMake

Given for instance: 

    /opt/libossia/lib
    /opt/libossia/include

You can write the following CMakeLists.txt

```CMake
cmake_minimum_required(VERSION 3.13)
project(minidaw)

find_package(ossia REQUIRED)

add_executable(minidaw MiniDAW.cpp)
target_link_libraries(minidaw PRIVATE ossia::ossia)
```

and call CMake with: 

    cmake -DCMAKE_PREFIX_PATH=/opt/libossia /path/to/your/source/dir

## Build manually

Given for instance: 

    /opt/libossia/lib
    /opt/libossia/include

You can build with

    g++ -I /opt/libossia/include -DASIO_DISABLE_CONCEPTS=1 -DASIO_STANDALONE=1 -DBOOST_ALL_NO_LIB -DRAPIDJSON_HAS_STDSTRING=1 -DTINYSPLINE_DOUBLE_PRECISION -L /opt/libossia/lib -lossia example.cpp -o example
