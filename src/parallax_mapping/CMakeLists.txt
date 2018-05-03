cmake_minimum_required(VERSION 3.7)

project(parallax_mapping)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}${PROJECT_NAME}")

add_library(parallax_mapping_lib STATIC parallax_mapping.cpp)

add_executable(parallax_mapping main.cpp)

target_include_directories(parallax_mapping PRIVATE
  ${parallax_mapping_SOURCE_DIR})
target_link_libraries(parallax_mapping base)
target_link_libraries(parallax_mapping parallax_mapping_lib)
