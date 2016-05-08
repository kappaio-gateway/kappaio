cmake_minimum_required(VERSION 2.8.10)

if(NOT TARGET kdev)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

project(kdev C CXX)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set( CMAKE_SKIP_BUILD_RPATH true )
#link_directories( bin )
#include(FindPkgConfig)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../kapi build/kapi)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")

include_directories(include)
file(GLOB CPP_FILES source/*.cpp)
add_library(${PROJECT_NAME} SHARED ${CPP_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC kapi)
target_include_directories(kdev PUBLIC include)
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION "0.1.0")
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)
else()
endif(NOT TARGET kdev)
