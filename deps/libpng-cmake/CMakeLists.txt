cmake_minimum_required(VERSION 3.16)

project(libpng-cmake)

# zlib
set(SKIP_INSTALL_ALL ON)
add_subdirectory(zlib)

# libpng
set(PNG_EXECUTABLES OFF CACHE INTERNAL "")
set(PNG_TESTS OFF CACHE INTERNAL "")
set(PNG_SHARED OFF CACHE INTERNAL "")
set(PNG_HARDWARE_OPTIMIZATIONS OFF CACHE INTERNAL "")
set(PNG_BUILD_ZLIB ON CACHE INTERNAL "")
set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/zlib/" "${CMAKE_BINARY_DIR}/deps/libpng-cmake/zlib" CACHE INTERNAL "")
set(ZLIB_LIBRARIES zlibstatic)
add_subdirectory(libpng)