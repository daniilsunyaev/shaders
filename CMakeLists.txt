cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(shaders)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall --pedantic -Wconversion -O3")
  message(info ${CMAKE_CXX_FLAGS})
endif()

set(BIN_DIR "${PROJECT_SOURCE_DIR}/bin/")
include_directories(src/shared)
include_directories(src/base)
include_directories(include)
add_subdirectory(src/shared)
add_subdirectory(src/base)

add_subdirectory(src/reflective_shadow_maps)
add_subdirectory(src/ssao)
add_subdirectory(src/pbr)
