cmake_minimum_required(VERSION 2.8.10)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

project(kappaio C CXX)


set(LIBS "kapi;hal;kdev;kintf;aps;zdo")
foreach(LIB ${LIBS})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${LIB})
endforeach(LIB)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -D__LITTLE_ENDIAN__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -D__LITTLE_ENDIAN__")

include_directories(include)
file(GLOB CPP_FILES *.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

target_link_libraries(${PROJECT_NAME} dl ${LIBS})



