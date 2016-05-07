cmake_minimum_required(VERSION 2.8.10)

if(NOT TARGET kapi)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

project(kapi C CXX)

#
include(FindPkgConfig)
pkg_check_modules(openssl REQUIRED openssl>=1.0.1)
pkg_check_modules(jansson REQUIRED jansson>=2.5)
#cxxflags cflags

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")

file(GLOB CPP_FILES source/*.cpp)
include_directories(include)
add_library(${PROJECT_NAME} SHARED ${CPP_FILES})
target_include_directories(kapi PUBLIC include)
target_link_libraries(${PROJECT_NAME} ${openssl_LIBRARIES} ${jansson_LIBRARIES} pthread)
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION "0.1.0")
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)
else()
message("-- kappaio-dev : kapi is included more then once")
endif()
