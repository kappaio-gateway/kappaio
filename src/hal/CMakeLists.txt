cmake_minimum_required(VERSION 2.8.10)
if(NOT TARGET hal)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

project(hal C CXX)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set( CMAKE_SKIP_BUILD_RPATH true )
#link_directories( bin )
include(FindPkgConfig)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../kapi build/kapi)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../kdev build/kdev)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")

include_directories(include)
file(GLOB CPP_FILES source/*.cpp)
file(GLOB C_FILES source/*.c)
set_source_files_properties(${C_FILES} PROPERTIES COMPILE_FLAGS "-x c++")
add_library(${PROJECT_NAME} SHARED ${CPP_FILES} ${C_FILES})
target_include_directories(hal PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC kapi kdev stdc++)
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION "0.1.0")
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)
else()
message("-- kappaio-dev : hal is included more than once")
endif(NOT TARGET hal)


