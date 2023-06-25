cmake_minimum_required(VERSION 3.14)
project(ascii-art)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add flags for MSVC
if(MSVC)
  add_compile_options(/utf-8)
endif()

# change the output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<0:>${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<0:>${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<0:>${CMAKE_SOURCE_DIR}/lib")

# include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# essential packages
include(FetchContent)

# freetype
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype
  GIT_TAG        VER-2-13-0
)
FetchContent_MakeAvailable(freetype)

add_subdirectory(src)
add_subdirectory(resources)