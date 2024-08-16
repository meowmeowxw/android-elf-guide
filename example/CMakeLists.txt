cmake_minimum_required(VERSION 3.10)

project(Example)

set(CMAKE_TOOLCHAIN_FILE $ENV{NDK}/build/cmake/android.toolchain.cmake)

set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
set(CMAKE_ANDROID_NDK $ENV{NDK})
set(CMAKE_ANDROID_STL_TYPE c++_shared)

# Set eventual shared libraries
# set(LIBRARIES_DIR $ENV{PWD}./lib)
# link_directories(${LIBRARIES_DIR})

add_executable(example example.cpp)

# Link against specific shared libraries
# target_link_libraries(harness ${LIBRARIES_DIR}/libname.so)

# Set ASAN compile and link flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

message(STATUS "CMake system name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMake Android ABI: ${CMAKE_ANDROID_ARCH_ABI}")
message(STATUS "CMake Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "CMake Android STL: ${CMAKE_ANDROID_STL_TYPE}")
message(STATUS "CMake Android platform: ${CMAKE_ANDROID_PLATFORM}")
