cmake_minimum_required(VERSION 2.8.10)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

project(sbl C CXX)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set( CMAKE_SKIP_BUILD_RPATH true )
#link_directories( bin )
include(FindPkgConfig)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../kapi ${CMAKE_CURRENT_SOURCE_DIR}/build/kapi)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../hal ${CMAKE_CURRENT_SOURCE_DIR}/build/hal)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -D__LITTLE_ENDIAN__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")

include_directories(include)
file(GLOB CPP_FILES source/*.cpp source/*.c)
add_library(${PROJECT_NAME} MODULE ${CPP_FILES})
target_link_libraries(${PROJECT_NAME} kapi hal)
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION "1.0.0")
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 1)

