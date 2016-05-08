cmake_minimum_required(VERSION 2.8.10)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

project(rsserial-watch C CXX)

#
include(FindPkgConfig)
#pkg_check_modules(openssl openssl>=1.0.1)
#find_package(libkapi)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../kapi build/kapi)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kapi/include)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")
file(GLOB CPP_FILES source/*.cpp)
#include_directories(include)
add_executable(${PROJECT_NAME} ${CPP_FILES})
target_link_libraries(${PROJECT_NAME} kapi)

#set(${PROJECT_NAME}_DEFINITIONS CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)
#set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
