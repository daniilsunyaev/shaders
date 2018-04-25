cmake_minimum_required(VERSION 3.7)

project(pbr)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}${PROJECT_NAME}")

add_library(pbr_lib STATIC pbr.cpp)

add_executable(pbr main.cpp)

target_include_directories(pbr PRIVATE
  ${pbr_SOURCE_DIR})
target_link_libraries(pbr base)
target_link_libraries(pbr pbr_lib)
